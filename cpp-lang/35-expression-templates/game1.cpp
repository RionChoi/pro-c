#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <iomanip>

// 간단한 벡터 클래스
template <typename T>
class Vector {
private:
    std::vector<T> data;

public:
    Vector(size_t n = 0, T init = 0) : data(n, init) {}

    explicit Vector(const std::vector<T>& v) : data(v) {}

    size_t size() const { return data.size(); }

    T& operator[](size_t i) { return data[i]; }
    const T& operator[](size_t i) const { return data[i]; }

    T dot_product(const Vector& other) const {
        T result = 0;
        for (size_t i = 0; i < data.size(); ++i) {
            result += data[i] * other[i];
        }
        return result;
    }

    double magnitude() const {
        return std::sqrt(dot_product(*this));
    }

    void display() const {
        std::cout << "[";
        for (size_t i = 0; i < data.size(); ++i) {
            std::cout << std::fixed << std::setprecision(1) << data[i];
            if (i < data.size() - 1) std::cout << ", ";
        }
        std::cout << "]";
    }
};

// 벡터 표현식
template <typename E>
class VectorExpr {
public:
    const E& derived() const {
        return static_cast<const E&>(*this);
    }

    double operator[](size_t i) const {
        return derived()[i];
    }

    size_t size() const {
        return derived().size();
    }
};

// 벡터 리터럴
template <typename T>
class VectorLiteral : public VectorExpr<VectorLiteral<T>> {
private:
    const Vector<T>& vec;

public:
    VectorLiteral(const Vector<T>& v) : vec(v) {}

    double operator[](size_t i) const { return vec[i]; }
    size_t size() const { return vec.size(); }
};

// 벡터 덧셈
template <typename E1, typename E2>
class VectorSum : public VectorExpr<VectorSum<E1, E2>> {
private:
    const E1& left;
    const E2& right;

public:
    VectorSum(const E1& l, const E2& r) : left(l), right(r) {}

    double operator[](size_t i) const {
        return left[i] + right[i];
    }

    size_t size() const { return left.size(); }
};

// 벡터 스칼라 곱셈
template <typename E>
class VectorScale : public VectorExpr<VectorScale<E>> {
private:
    const E& expr;
    double scalar;

public:
    VectorScale(const E& e, double s) : expr(e), scalar(s) {}

    double operator[](size_t i) const {
        return expr[i] * scalar;
    }

    size_t size() const { return expr.size(); }
};

// 연산자 오버로딩
template <typename E1, typename E2>
auto operator+(const VectorExpr<E1>& lhs, const VectorExpr<E2>& rhs) {
    return VectorSum<E1, E2>(lhs.derived(), rhs.derived());
}

template <typename E>
auto operator*(const VectorExpr<E>& expr, double scalar) {
    return VectorScale<E>(expr.derived(), scalar);
}

int main() {
    std::cout << "=== 벡터 수학 게임: 힘의 합성 ===\n\n";

    // 세 개의 힘(벡터) 정의
    Vector<double> force1(std::vector<double>({3.0, 4.0}));
    Vector<double> force2(std::vector<double>({1.0, 2.0}));
    Vector<double> force3(std::vector<double>({2.0, 1.0}));

    std::cout << "1. 주어진 힘(벡터):\n";
    std::cout << "   Force 1 = ";
    force1.display();
    std::cout << " (크기: " << force1.magnitude() << "N)\n";
    std::cout << "   Force 2 = ";
    force2.display();
    std::cout << " (크기: " << force2.magnitude() << "N)\n";
    std::cout << "   Force 3 = ";
    force3.display();
    std::cout << " (크기: " << force3.magnitude() << "N)\n\n";

    std::cout << "2. 표현식 템플릿으로 합성 힘 계산:\n";

    // 표현식 생성 (지연 평가)
    auto f1_lit = VectorLiteral(force1);
    auto f2_lit = VectorLiteral(force2);
    auto f3_lit = VectorLiteral(force3);

    // 중간 계산
    auto sum_f1_f2 = f1_lit + f2_lit;
    auto total_force = sum_f1_f2 + f3_lit;

    // 최종 벡터 생성
    Vector<double> result_force(2);
    for (size_t i = 0; i < 2; ++i) {
        result_force[i] = total_force[i];
    }

    std::cout << "   F1 + F2 + F3 = ";
    result_force.display();
    std::cout << " (크기: " << result_force.magnitude() << "N)\n\n";

    std::cout << "3. 가중치를 포함한 합성:\n";
    auto weighted_f1 = f1_lit * 0.5;
    auto weighted_f2 = f2_lit * 0.3;
    auto weighted_f3 = f3_lit * 0.2;

    Vector<double> weighted_result(2);
    for (size_t i = 0; i < 2; ++i) {
        weighted_result[i] = weighted_f1[i] + weighted_f2[i] + weighted_f3[i];
    }

    std::cout << "   0.5*F1 + 0.3*F2 + 0.2*F3 = ";
    weighted_result.display();
    std::cout << " (크기: " << weighted_result.magnitude() << "N)\n\n";

    std::cout << "4. 내적 계산:\n";
    double dot_f1_f2 = force1.dot_product(force2);
    double dot_f1_f3 = force1.dot_product(force3);
    std::cout << "   F1 · F2 = " << dot_f1_f2 << "\n";
    std::cout << "   F1 · F3 = " << dot_f1_f3 << "\n\n";

    std::cout << "게임 완료! 표현식 템플릿으로 효율적 벡터 계산 완성!\n";

    return 0;
}
