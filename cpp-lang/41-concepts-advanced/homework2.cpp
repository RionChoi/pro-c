// Session 41: C++20 Concepts 심화 — 커스텀 제약 라이브러리 & 제약 기반 설계
#include <concepts>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <span>
#include <numeric>
#include <cmath>
#include <type_traits>

// ─────────────────────────────────────────────
// Concept 라이브러리: math_concepts.hpp 역할
// ─────────────────────────────────────────────

// 스칼라 수치 타입
template <typename T>
concept Scalar = std::is_arithmetic_v<T> && !std::is_same_v<T, bool>;

// 벡터 공간 원소 (덧셈 + 스칼라 곱 지원)
template <typename T>
concept VectorSpaceElement = requires(T a, T b, double s) {
    { a + b    } -> std::convertible_to<T>;
    { a - b    } -> std::convertible_to<T>;
    { s * a    } -> std::convertible_to<T>;
    { a * s    } -> std::convertible_to<T>;
    requires std::default_initializable<T>;
};

// 내적(dot product) 계산 가능한 타입
template <typename T>
concept InnerProductSpace = VectorSpaceElement<T> && requires(T a, T b) {
    { a.dot(b) } -> std::floating_point;
    { a.norm() } -> std::floating_point;
};

// 반복자 범주 제약
template <typename It>
concept ForwardIteratable = requires(It it) {
    { *it   };
    { ++it  } -> std::same_as<It&>;
    { it++  };
    requires std::equality_comparable<It>;
};

// ─────────────────────────────────────────────
// Vec3: InnerProductSpace 만족하는 3D 벡터
// ─────────────────────────────────────────────

struct Vec3 {
    double x, y, z;

    Vec3 operator+(const Vec3& o) const { return {x+o.x, y+o.y, z+o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x-o.x, y-o.y, z-o.z}; }
    Vec3 operator*(double s)      const { return {x*s,   y*s,   z*s}; }
    friend Vec3 operator*(double s, const Vec3& v) { return v * s; }

    double dot(const Vec3& o) const { return x*o.x + y*o.y + z*o.z; }
    double norm()             const { return std::sqrt(dot(*this)); }
    Vec3   normalize()        const {
        double n = norm();
        return (n > 1e-12) ? (*this * (1.0/n)) : Vec3{};
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec3& v) {
        return os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
    }
};

// ─────────────────────────────────────────────
// Concept 기반 수치 알고리즘
// ─────────────────────────────────────────────

// 임의 InnerProductSpace에 대해 작동하는 코사인 유사도
template <InnerProductSpace V>
double cosine_similarity(const V& a, const V& b) {
    double na = a.norm();
    double nb = b.norm();
    if (na < 1e-12 || nb < 1e-12) return 0.0;
    return a.dot(b) / (na * nb);
}

// Gram-Schmidt 직교화 (VectorSpaceElement 컨테이너에 대해)
template <InnerProductSpace V>
std::vector<V> gram_schmidt(std::vector<V> vecs) {
    std::vector<V> ortho;
    for (auto& v : vecs) {
        V proj{};
        for (const auto& u : ortho) {
            double coeff = v.dot(u) / u.dot(u);
            proj = proj + (u * coeff);
        }
        V result = v - proj;
        if (result.norm() > 1e-12)
            ortho.push_back(result.normalize());
    }
    return ortho;
}

// ─────────────────────────────────────────────
// Concept 기반 컨테이너 어댑터
// ─────────────────────────────────────────────

template <typename T>
concept NumericRange = requires(T r) {
    { r.begin() } -> std::input_or_output_iterator;
    { r.end()   } -> std::input_or_output_iterator;
    requires Scalar<typename T::value_type>;
};

template <NumericRange R>
auto mean(const R& range) {
    using V = typename R::value_type;
    if (range.empty()) return V{};
    auto s = std::accumulate(range.begin(), range.end(), V{});
    return static_cast<V>(s) / static_cast<V>(range.size());
}

template <NumericRange R>
double stddev(const R& range) {
    if (range.size() < 2) return 0.0;
    double m = static_cast<double>(mean(range));
    double sq_sum = 0.0;
    for (auto v : range) {
        double d = static_cast<double>(v) - m;
        sq_sum += d * d;
    }
    return std::sqrt(sq_sum / static_cast<double>(range.size() - 1));
}

// ─────────────────────────────────────────────
// 타입 특성 기반 Concept (고급 패턴)
// ─────────────────────────────────────────────

// CRTP 기반 Concept: 파생 클래스가 특정 인터페이스를 구현했는지
template <typename Derived, typename Base>
concept DerivedFrom = std::derived_from<Derived, Base>;

// 복사/이동 가능 여부를 세밀하게 제약
template <typename T>
concept EfficientlyMovable = std::movable<T> && requires {
    requires !std::is_trivially_copyable_v<T>
          || sizeof(T) <= 64;  // 64바이트 이하이거나 이동 가능
};

// 직렬화 가능 + 버전 관리
template <typename T>
concept VersionedSerializable = requires(T t, int version) {
    { t.serialize(version)        } -> std::same_as<std::string>;
    { T::deserialize(std::string{}, version) } -> std::same_as<T>;
    { T::current_version()        } -> std::integral;
};

struct Document {
    std::string title;
    std::string content;
    int version_ = 1;

    std::string serialize(int /*ver*/) const {
        return "v" + std::to_string(version_) + "|" + title + "|" + content;
    }
    static Document deserialize(const std::string& s, int /*ver*/) {
        (void)s;
        return {"untitled", "", 1};
    }
    static int current_version() { return 2; }
};

// ─────────────────────────────────────────────
// main
// ─────────────────────────────────────────────

int main() {
    std::cout << "=== Concept 라이브러리 검증 ===\n";
    std::cout << "Scalar<int>:               " << Scalar<int>               << '\n';
    std::cout << "Scalar<bool>:              " << Scalar<bool>              << '\n';
    std::cout << "Scalar<double>:            " << Scalar<double>            << '\n';
    std::cout << "VectorSpaceElement<Vec3>:  " << VectorSpaceElement<Vec3>  << '\n';
    std::cout << "InnerProductSpace<Vec3>:   " << InnerProductSpace<Vec3>   << '\n';
    std::cout << "VersionedSerializable<Document>: "
              << VersionedSerializable<Document> << '\n';
    std::cout << '\n';

    std::cout << "=== Vec3 연산 ===\n";
    Vec3 a{1, 0, 0};
    Vec3 b{0, 1, 0};
    Vec3 c{1, 1, 0};

    std::cout << "a = " << a << ", b = " << b << ", c = " << c << '\n';
    std::cout << "a + b = " << (a + b) << '\n';
    std::cout << "2.0 * c = " << (2.0 * c) << '\n';
    std::cout << "a.dot(b) = " << a.dot(b) << '\n';
    std::cout << "c.norm() = " << c.norm() << '\n';
    std::cout << "c.normalize() = " << c.normalize() << '\n';
    std::cout << '\n';

    std::cout << "=== 코사인 유사도 ===\n";
    std::cout << "cosine_similarity(a, b) = " << cosine_similarity(a, b) << '\n';
    std::cout << "cosine_similarity(a, c) = " << cosine_similarity(a, c) << '\n';
    std::cout << "cosine_similarity(c, c) = " << cosine_similarity(c, c) << '\n';
    std::cout << '\n';

    std::cout << "=== Gram-Schmidt 직교화 ===\n";
    std::vector<Vec3> basis{{1,1,0},{1,0,0},{0,1,1}};
    auto ortho = gram_schmidt(basis);
    std::cout << "직교 기저 (" << ortho.size() << "개):\n";
    for (std::size_t i = 0; i < ortho.size(); ++i) {
        std::cout << "  e" << i << " = " << ortho[i]
                  << " (norm=" << ortho[i].norm() << ")\n";
    }
    std::cout << '\n';

    std::cout << "=== 수치 통계 (NumericRange) ===\n";
    std::vector<double> data{2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
    std::cout << "data: ";
    for (auto v : data) std::cout << v << ' ';
    std::cout << '\n';
    std::cout << "mean   = " << mean(data)   << '\n';
    std::cout << "stddev = " << stddev(data) << '\n';

    return 0;
}
