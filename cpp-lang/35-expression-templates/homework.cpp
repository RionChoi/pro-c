#include <iostream>
#include <vector>
#include <array>

// 표현식 템플릿을 위한 벡터 클래스
template <typename T>
class Vector {
private:
    std::vector<T> data;

public:
    Vector(size_t n = 0) : data(n) {}

    Vector(const std::vector<T>& v) : data(v) {}

    size_t size() const { return data.size(); }

    T& operator[](size_t i) { return data[i]; }
    const T& operator[](size_t i) const { return data[i]; }

    void display() const {
        std::cout << "[";
        for (size_t i = 0; i < data.size(); ++i) {
            std::cout << data[i];
            if (i < data.size() - 1) std::cout << ", ";
        }
        std::cout << "]";
    }
};

// 표현식 기본 템플릿
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

// 벡터 리터럴 표현식
template <typename T>
class VectorLiteral : public VectorExpr<VectorLiteral<T>> {
private:
    const Vector<T>& vec;

public:
    VectorLiteral(const Vector<T>& v) : vec(v) {}

    double operator[](size_t i) const { return vec[i]; }
    size_t size() const { return vec.size(); }
};

// 벡터 덧셈 표현식 (Lazy Evaluation)
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

// 스칼라 곱셈 표현식
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

template <typename E>
auto operator*(double scalar, const VectorExpr<E>& expr) {
    return VectorScale<E>(expr.derived(), scalar);
}

int main() {
    std::cout << "=== 표현식 템플릿 기초 ===\n\n";

    Vector<double> v1({1.0, 2.0, 3.0});
    Vector<double> v2({4.0, 5.0, 6.0});

    std::cout << "1. 벡터 초기값:\n";
    std::cout << "   v1 = ";
    v1.display();
    std::cout << "\n   v2 = ";
    v2.display();
    std::cout << "\n\n";

    std::cout << "2. 표현식 템플릿 활용:\n";

    // 직접 계산
    Vector<double> result1(3);
    auto expr1 = VectorLiteral(v1);
    auto expr2 = VectorLiteral(v2);
    auto sum_expr = expr1 + expr2;

    std::cout << "   v1 + v2 = [";
    for (size_t i = 0; i < 3; ++i) {
        std::cout << sum_expr[i];
        if (i < 2) std::cout << ", ";
    }
    std::cout << "]\n";

    // 스칼라 곱셈
    auto scaled_expr = expr1 * 2.0;
    std::cout << "   2.0 * v1 = [";
    for (size_t i = 0; i < 3; ++i) {
        std::cout << scaled_expr[i];
        if (i < 2) std::cout << ", ";
    }
    std::cout << "]\n";

    // 복합 표현식
    auto complex_expr = (expr1 + expr2) * 0.5;
    std::cout << "   (v1 + v2) * 0.5 = [";
    for (size_t i = 0; i < 3; ++i) {
        std::cout << complex_expr[i];
        if (i < 2) std::cout << ", ";
    }
    std::cout << "]\n\n";

    std::cout << "3. 표현식 템플릿의 이점:\n";
    std::cout << "   - 임시 객체 제거 (메모리 효율)\n";
    std::cout << "   - 지연 평가 (Lazy Evaluation)\n";
    std::cout << "   - 컴파일 타임 최적화 가능\n";
    std::cout << "   - 연산 융합으로 성능 향상\n";

    return 0;
}
