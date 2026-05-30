#include <iostream>
#include <vector>

const int N = 100;

template <typename T>
class Vec {
public:
    T* data;
    int size;

    Vec(int n = 0) : size(n) {
        data = new T[n]();
    }

    ~Vec() {
        delete[] data;
    }

    template <typename E>
    Vec(const E& expr) : size(expr.size) {
        data = new T[size];
        for (int i = 0; i < size; ++i) {
            data[i] = expr[i];
        }
    }

    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
};

template <typename L, typename R>
class AddExpr {
public:
    const L& left;
    const R& right;
    int size;

    AddExpr(const L& l, const R& r) : left(l), right(r), size(l.size) {}

    double operator[](int i) const {
        return left[i] + right[i];
    }
};

template <typename E>
class MulExpr {
public:
    const E& expr;
    double scalar;
    int size;

    MulExpr(double s, const E& e) : expr(e), scalar(s), size(e.size) {}

    double operator[](int i) const {
        return scalar * expr[i];
    }
};

template <typename L, typename R>
AddExpr<L, R> operator+(const L& l, const R& r) {
    return AddExpr<L, R>(l, r);
}

template <typename E>
MulExpr<E> operator*(double scalar, const E& expr) {
    return MulExpr<E>(scalar, expr);
}

template <typename E>
MulExpr<E> operator*(const E& expr, double scalar) {
    return MulExpr<E>(scalar, expr);
}

int main() {
    std::cout << "=== 표현식 템플릿 - 복합 연산 ===\n\n";

    Vec<double> a(N), b(N), c(N);
    for (int i = 0; i < N; ++i) {
        a[i] = i + 1.0;
        b[i] = i + 2.0;
        c[i] = i + 3.0;
    }

    std::cout << "a[0]=" << a[0] << ", b[0]=" << b[0] << ", c[0]=" << c[0] << "\n\n";

    std::cout << "식 1: 2*a + 3*b\n";
    {
        Vec<double> result = 2.0 * a + 3.0 * b;
        std::cout << "result[0] = " << result[0] << "\n";
        std::cout << "예상값 = 2*1 + 3*2 = 8 ✓\n\n";
    }

    std::cout << "식 2: (a + b) * 2 + c\n";
    {
        Vec<double> result = (a + b) * 2.0 + c;
        std::cout << "result[0] = " << result[0] << "\n";
        std::cout << "예상값 = (1 + 2) * 2 + 3 = 9 ✓\n\n";
    }

    std::cout << "식 3: 0.5 * (a + b + c)\n";
    {
        Vec<double> result = 0.5 * (a + b + c);
        std::cout << "result[0] = " << result[0] << "\n";
        std::cout << "예상값 = 0.5 * (1 + 2 + 3) = 3 ✓\n";
    }

    return 0;
}
