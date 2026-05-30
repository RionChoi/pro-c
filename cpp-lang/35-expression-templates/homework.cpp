#include <iostream>
#include <vector>
#include <cmath>

// Vector 클래스
class Vector {
private:
    std::vector<double> data;

public:
    Vector() = default;
    explicit Vector(size_t size) : data(size, 0.0) {}
    Vector(const std::vector<double>& v) : data(v) {}

    size_t size() const { return data.size(); }

    double operator[](size_t i) const { return data[i]; }
    double& operator[](size_t i) { return data[i]; }

    // 출력
    void print() const {
        std::cout << "[";
        for (size_t i = 0; i < data.size(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << data[i];
        }
        std::cout << "]\n";
    }
};

// 표현식 템플릿 기본 구조

// Add 연산자
struct Add {
    static double apply(double a, double b) { return a + b; }
};

// Mul 연산자
struct Mul {
    static double apply(double a, double b) { return a * b; }
};

// 이진 연산 표현식 노드
template <typename L, typename R, typename Op>
class BinOp {
private:
    const L& lhs;
    const R& rhs;

public:
    BinOp(const L& l, const R& r) : lhs(l), rhs(r) {}

    double operator[](size_t i) const {
        return Op::apply(lhs[i], rhs[i]);
    }

    size_t size() const { return lhs.size(); }
};

// 벡터 + 벡터
BinOp<Vector, Vector, Add> operator+(const Vector& a, const Vector& b) {
    return BinOp<Vector, Vector, Add>(a, b);
}

// BinOp + Vector
template <typename L, typename R>
BinOp<BinOp<L, R, Add>, Vector, Add> operator+(const BinOp<L, R, Add>& a, const Vector& b) {
    return BinOp<BinOp<L, R, Add>, Vector, Add>(a, b);
}

// 벡터 * 벡터
BinOp<Vector, Vector, Mul> operator*(const Vector& a, const Vector& b) {
    return BinOp<Vector, Vector, Mul>(a, b);
}

// BinOp * Vector
template <typename L, typename R>
BinOp<BinOp<L, R, Add>, Vector, Mul> operator*(const BinOp<L, R, Add>& a, const Vector& b) {
    return BinOp<BinOp<L, R, Add>, Vector, Mul>(a, b);
}

// 할당 연산 (실제 계산 수행)
template <typename E>
Vector& assign_expr(Vector& result, const E& expr) {
    for (size_t i = 0; i < result.size(); i++) {
        result[i] = expr[i];
    }
    return result;
}

int main() {
    std::cout << "=== 표현식 템플릿 기본 예제 ===\n\n";

    // 벡터 생성
    Vector a(5);
    Vector b(5);
    Vector c(5);

    for (int i = 0; i < 5; i++) {
        a[i] = i + 1;      // [1, 2, 3, 4, 5]
        b[i] = (i + 1) * 2; // [2, 4, 6, 8, 10]
        c[i] = i;          // [0, 1, 2, 3, 4]
    }

    std::cout << "벡터 a: ";
    a.print();
    std::cout << "벡터 b: ";
    b.print();
    std::cout << "벡터 c: ";
    c.print();

    // 1. 간단한 더하기
    std::cout << "\n[표현식 1: a + b]\n";
    Vector result1(5);
    assign_expr(result1, a + b);
    std::cout << "결과: ";
    result1.print();

    // 2. 벡터 곱셈
    std::cout << "[표현식 2: a * b]\n";
    Vector result2(5);
    assign_expr(result2, a * b);
    std::cout << "결과: ";
    result2.print();

    // 3. 복합 연산: (a + b) + c
    std::cout << "[표현식 3: (a + b) + c]\n";
    Vector result3(5);
    assign_expr(result3, (a + b) + c);
    std::cout << "결과: ";
    result3.print();

    // 표현식 템플릿의 장점 설명
    std::cout << "\n=== 표현식 템플릿의 장점 ===\n";
    std::cout << "1. 중간 벡터 객체 생성 제거\n";
    std::cout << "2. 메모리 할당 횟수 감소\n";
    std::cout << "3. 캐시 지역성 향상\n";
    std::cout << "4. 컴파일러 최적화 용이\n\n";

    std::cout << "표현식: (a + b) + c\n";
    std::cout << "- 일반 방법: 중간 벡터 2개 생성 필요\n";
    std::cout << "- 표현식 템플릿: 중간 벡터 0개 생성\n";
    std::cout << "- 컴파일 타임에 표현식 트리 구성\n";
    std::cout << "- 런타임에만 최종 계산 수행\n";

    return 0;
}
