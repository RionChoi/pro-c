#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>

// Vector 클래스
class Vector {
private:
    std::vector<double> data;

public:
    Vector() = default;
    explicit Vector(size_t size) : data(size, 0.0) {}

    size_t size() const { return data.size(); }

    double operator[](size_t i) const { return data[i]; }
    double& operator[](size_t i) { return data[i]; }

    void print(int max_elements = 5) const {
        std::cout << "[";
        size_t limit = std::min(static_cast<size_t>(max_elements), data.size());
        for (size_t i = 0; i < limit; i++) {
            if (i > 0) std::cout << ", ";
            std::cout << data[i];
        }
        if (data.size() > limit) std::cout << ", ...";
        std::cout << "]";
    }
};

// 연산 정의
struct Add { static double apply(double a, double b) { return a + b; } };
struct Sub { static double apply(double a, double b) { return a - b; } };
struct Mul { static double apply(double a, double b) { return a * b; } };
struct Div { static double apply(double a, double b) { return b != 0 ? a / b : 0; } };

// 이진 연산 표현식
template <typename L, typename R, typename Op>
class BinOp {
private:
    const L& lhs;
    const R& rhs;

public:
    BinOp(const L& l, const R& r) : lhs(l), rhs(r) {}
    double operator[](size_t i) const { return Op::apply(lhs[i], rhs[i]); }
    size_t size() const { return lhs.size(); }
};

// 스칼라 연산 래퍼
class Scalar {
private:
    double value;

public:
    Scalar(double v) : value(v) {}
    double operator[](size_t) const { return value; }
};

// Vector 연산자 오버로드
BinOp<Vector, Vector, Add> operator+(const Vector& a, const Vector& b) {
    return BinOp<Vector, Vector, Add>(a, b);
}

BinOp<Vector, Vector, Sub> operator-(const Vector& a, const Vector& b) {
    return BinOp<Vector, Vector, Sub>(a, b);
}

BinOp<Vector, Vector, Mul> operator*(const Vector& a, const Vector& b) {
    return BinOp<Vector, Vector, Mul>(a, b);
}

// 스칼라 곱셈
BinOp<Vector, Scalar, Mul> operator*(const Vector& a, double s) {
    return BinOp<Vector, Scalar, Mul>(a, Scalar(s));
}

BinOp<Scalar, Vector, Mul> operator*(double s, const Vector& a) {
    return BinOp<Scalar, Vector, Mul>(Scalar(s), a);
}

// BinOp + Vector
template <typename L, typename R, typename Op>
BinOp<BinOp<L, R, Op>, Vector, Add> operator+(const BinOp<L, R, Op>& a, const Vector& b) {
    return BinOp<BinOp<L, R, Op>, Vector, Add>(a, b);
}

// BinOp + BinOp
template <typename L1, typename R1, typename Op1, typename L2, typename R2, typename Op2>
BinOp<BinOp<L1, R1, Op1>, BinOp<L2, R2, Op2>, Add> operator+(const BinOp<L1, R1, Op1>& a, const BinOp<L2, R2, Op2>& b) {
    return BinOp<BinOp<L1, R1, Op1>, BinOp<L2, R2, Op2>, Add>(a, b);
}

// BinOp * Vector
template <typename L, typename R, typename Op>
BinOp<BinOp<L, R, Op>, Vector, Mul> operator*(const BinOp<L, R, Op>& a, const Vector& b) {
    return BinOp<BinOp<L, R, Op>, Vector, Mul>(a, b);
}

// 할당 연산
template <typename E>
Vector& assign_expr(Vector& result, const E& expr) {
    for (size_t i = 0; i < result.size(); i++) {
        result[i] = expr[i];
    }
    return result;
}

int main() {
    std::cout << "=== 고급 표현식 템플릿 예제 ===\n\n";

    const size_t n = 1000;
    Vector a(n), b(n), c(n), d(n);

    // 벡터 초기화
    for (size_t i = 0; i < n; i++) {
        a[i] = i + 1;
        b[i] = std::sin(i * 0.01);
        c[i] = std::cos(i * 0.01);
        d[i] = i % 10;
    }

    std::cout << "크기 " << n << "인 벡터 4개 생성\n";
    std::cout << "a: ";
    a.print();
    std::cout << "\nb: ";
    b.print();
    std::cout << "\nc: ";
    c.print();
    std::cout << "\nd: ";
    d.print();
    std::cout << "\n\n";

    // 1. 스칼라 곱셈
    std::cout << "[표현식 1: a * 2.0]\n";
    Vector result1(n);
    assign_expr(result1, a * 2.0);
    std::cout << "첫 5개: ";
    result1.print();
    std::cout << "\n\n";

    // 2. 복합 연산: (a + b) * c
    std::cout << "[표현식 2: (a + b) * c]\n";
    Vector result2(n);
    assign_expr(result2, (a + b) * c);
    std::cout << "첫 5개: ";
    result2.print();
    std::cout << "\n\n";

    // 3. 더 복합한 연산: (a * 2.0) + (b * 3.0)
    std::cout << "[표현식 3: (a * 2.0) + (b * 3.0)]\n";
    Vector result3(n);
    assign_expr(result3, (a * 2.0) + (b * 3.0));
    std::cout << "첫 5개: ";
    result3.print();
    std::cout << "\n\n";

    // 성능 비교
    std::cout << "=== 성능 비교 ===\n\n";

    // 표현식 템플릿 방식
    Vector et_result(n);
    auto et_start = std::chrono::high_resolution_clock::now();
    for (int iter = 0; iter < 1000; iter++) {
        assign_expr(et_result, (a * 2.0) + (b * 3.0) + (c * 1.5));
    }
    auto et_end = std::chrono::high_resolution_clock::now();
    auto et_time = std::chrono::duration_cast<std::chrono::microseconds>(et_end - et_start);

    std::cout << "표현식 템플릿: " << et_time.count() << " μs\n";

    // 일반 방식 (중간 벡터 생성)
    Vector normal_result(n);
    auto normal_start = std::chrono::high_resolution_clock::now();
    for (int iter = 0; iter < 1000; iter++) {
        Vector temp1(n), temp2(n), temp3(n), temp4(n);
        for (size_t i = 0; i < n; i++) {
            temp1[i] = a[i] * 2.0;
            temp2[i] = b[i] * 3.0;
            temp3[i] = temp1[i] + temp2[i];
            temp4[i] = c[i] * 1.5;
            normal_result[i] = temp3[i] + temp4[i];
        }
    }
    auto normal_end = std::chrono::high_resolution_clock::now();
    auto normal_time = std::chrono::duration_cast<std::chrono::microseconds>(normal_end - normal_start);

    std::cout << "일반 방식:     " << normal_time.count() << " μs\n";
    std::cout << "개선율:        " <<
        (normal_time.count() > et_time.count() ?
         (100.0 * (normal_time.count() - et_time.count()) / normal_time.count()) : 0.0)
        << "%\n\n";

    std::cout << "=== 결론 ===\n";
    std::cout << "- 표현식 템플릿은 컴파일 타임에 연산 구조를 최적화\n";
    std::cout << "- 중간 벡터 생성으로 인한 메모리 할당 제거\n";
    std::cout << "- 캐시 효율성 향상으로 인한 성능 개선\n";
    std::cout << "- 복합 연산에서 특히 효과적\n";

    return 0;
}
