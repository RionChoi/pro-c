#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <numeric>

// Type traits for expression template optimization
template<typename T>
struct is_vector : std::false_type {};

template<>
struct is_vector<class Vector> : std::true_type {};

class Vector {
    std::vector<double> data;
public:
    explicit Vector(size_t n = 0, double val = 0.0) : data(n, val) {}

    Vector(const std::vector<double>& v) : data(v) {}

    size_t size() const { return data.size(); }

    double operator[](size_t i) const { return data[i]; }
    double& operator[](size_t i) { return data[i]; }

    Vector& operator=(const Vector& v) {
        data = v.data;
        return *this;
    }

    template<typename E>
    Vector& operator=(const E& expr) {
        for (size_t i = 0; i < data.size(); ++i) {
            data[i] = expr[i];
        }
        return *this;
    }

    double magnitude() const {
        return std::sqrt(std::inner_product(data.begin(), data.end(), data.begin(), 0.0));
    }

    double dot(const Vector& v) const {
        return std::inner_product(data.begin(), data.end(), v.data.begin(), 0.0);
    }

    void print(const std::string& name = "Vector") const {
        std::cout << name << " = [";
        for (size_t i = 0; i < data.size(); ++i) {
            std::cout << std::fixed << std::setprecision(3) << data[i];
            if (i < data.size() - 1) std::cout << ", ";
        }
        std::cout << "]\n";
    }
};

struct Add {
    double operator()(double a, double b) const { return a + b; }
};

struct Subtract {
    double operator()(double a, double b) const { return a - b; }
};

struct Multiply {
    double operator()(double a, double b) const { return a * b; }
};

struct Divide {
    double operator()(double a, double b) const { return a / b; }
};

template<typename Left, typename Right, typename Op>
class BinaryExpr {
    const Left& left;
    const Right& right;
    Op op;
public:
    BinaryExpr(const Left& l, const Right& r) : left(l), right(r) {}

    double operator[](size_t i) const {
        return op(left[i], right[i]);
    }

    size_t size() const { return left.size(); }
};

template<typename V, typename Op>
class ScalarExpr {
    const V& vec;
    double scalar;
    Op op;
public:
    ScalarExpr(const V& v, double s) : vec(v), scalar(s) {}

    double operator[](size_t i) const {
        return op(vec[i], scalar);
    }

    size_t size() const { return vec.size(); }
};

template<typename L, typename R>
auto operator+(const L& l, const R& r) {
    return BinaryExpr<L, R, Add>(l, r);
}

template<typename L, typename R>
auto operator-(const L& l, const R& r) {
    return BinaryExpr<L, R, Subtract>(l, r);
}

template<typename L, typename R>
auto operator*(const L& l, const R& r) {
    return BinaryExpr<L, R, Multiply>(l, r);
}

template<typename L, typename R>
auto operator/(const L& l, const R& r) {
    return BinaryExpr<L, R, Divide>(l, r);
}

template<typename V>
auto operator*(const V& v, double s) {
    return ScalarExpr<V, Multiply>(v, s);
}

template<typename V>
auto operator*(double s, const V& v) {
    return ScalarExpr<V, Multiply>(v, s);
}

template<typename V>
auto operator+(const V& v, double s) {
    return ScalarExpr<V, Add>(v, s);
}

template<typename V>
auto operator+(double s, const V& v) {
    return ScalarExpr<V, Add>(v, s);
}

template<typename V>
auto operator/(const V& v, double s) {
    return ScalarExpr<V, Divide>(v, s);
}

template<typename V>
auto operator-(const V& v, double s) {
    return ScalarExpr<V, Subtract>(v, s);
}

// 성능 비교: Expression Template vs 임시 객체
void performance_comparison() {
    std::cout << "\n=== 성능 비교: Expression Template vs 임시 객체 ===\n\n";

    const size_t n = 10000000;
    Vector v1(n, 1.5), v2(n, 2.5), v3(n, 3.5);

    // Expression Template 방식
    auto start1 = std::chrono::high_resolution_clock::now();
    Vector result1(n);
    result1 = v1 * 2.0 + v2 * 3.0 - v3 * 0.5;
    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);

    // 임시 객체 방식 (비효율적)
    auto start2 = std::chrono::high_resolution_clock::now();
    Vector temp1(n), temp2(n), temp3(n);
    temp1 = v1 * 2.0;
    temp2 = v2 * 3.0;
    temp3 = v3 * 0.5;
    Vector result2(n);
    result2 = temp1 + temp2;
    result2 = result2 - temp3;
    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);

    std::cout << "벡터 크기: " << n << "\n";
    std::cout << "Expression Template: " << duration1.count() << " μs\n";
    std::cout << "임시 객체 방식:      " << duration2.count() << " μs\n";
    std::cout << "성능 향상:          " << std::fixed << std::setprecision(2)
              << (double)duration2.count() / (double)duration1.count() << "배\n";
}

int main() {
    std::cout << "=== Expression Templates 고급 기법 ===\n\n";

    // 작은 벡터로 시연
    Vector v1({1.0, 2.0, 3.0});
    Vector v2({4.0, 5.0, 6.0});
    Vector v3({7.0, 8.0, 9.0});

    v1.print("v1");
    v2.print("v2");
    v3.print("v3");
    std::cout << "\n";

    // 선형 조합 (Linear Combination)
    std::cout << "-- 선형 조합 --\n";
    Vector lc(3);
    lc = 2.0 * v1 + 3.0 * v2 - v3;
    lc.print("2.0*v1 + 3.0*v2 - v3");

    std::cout << "\n크기 (Magnitude): " << lc.magnitude() << "\n\n";

    // 복합 표현식
    std::cout << "-- 복합 표현식 --\n";
    Vector complex(3);
    complex = (v1 + v2) * 2.0 + v3 / 2.0;
    complex.print("(v1 + v2) * 2.0 + v3 / 2.0");

    std::cout << "\n크기 (Magnitude): " << complex.magnitude() << "\n\n";

    // 스칼라 곱
    std::cout << "-- 스칼라 곱 --\n";
    double dp = v1.dot(v2);
    std::cout << "v1 · v2 = " << dp << "\n\n";

    // 벡터 정규화 표현식
    std::cout << "-- 정규화 (Normalized Vector) --\n";
    Vector normalized(3);
    double v1_mag = v1.magnitude();
    normalized = v1 * (1.0 / v1_mag);
    normalized.print("v1 / ||v1||");
    std::cout << "크기: " << normalized.magnitude() << " (1.0이어야 함)\n\n";

    // 성능 비교
    performance_comparison();

    std::cout << "\n표현식 템플릿은 선언적(declarative) 스타일로 복잡한 연산을 표현할 수 있으며,\n";
    std::cout << "컴파일러가 최적화하여 임시 객체 없이 효율적으로 계산합니다.\n";

    return 0;
}
