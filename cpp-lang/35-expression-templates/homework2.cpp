#include <iostream>
#include <vector>
#include <iomanip>

// 행렬 표현식 베이스 (CRTP)
template<typename E>
class MatExpr {
public:
    double operator()(int r, int c) const {
        return static_cast<const E &>(*this)(r, c);
    }

    int rows() const { return static_cast<const E &>(*this).rows(); }
    int cols() const { return static_cast<const E &>(*this).cols(); }
};

// 실제 행렬
class Matrix : public MatExpr<Matrix> {
private:
    std::vector<double> data;
    int r, c;

public:
    Matrix(int rows, int cols) : data(rows * cols, 0), r(rows), c(cols) {}

    template<typename E>
    Matrix(const MatExpr<E> &expr)
        : data(expr.rows() * expr.cols()), r(expr.rows()), c(expr.cols()) {
        for (int i = 0; i < r; ++i) {
            for (int j = 0; j < c; ++j) {
                (*this)(i, j) = expr(i, j);
            }
        }
    }

    double operator()(int i, int j) const { return data[i * c + j]; }
    double &operator()(int i, int j) { return data[i * c + j]; }

    int rows() const { return r; }
    int cols() const { return c; }

    void print() const {
        std::cout << "[\n";
        for (int i = 0; i < r; ++i) {
            std::cout << " ";
            for (int j = 0; j < c; ++j) {
                std::cout << std::fixed << std::setprecision(1) << (*this)(i, j) << " ";
            }
            std::cout << "\n";
        }
        std::cout << "]\n";
    }
};

// 행렬 덧셈 표현식
template<typename E1, typename E2>
class MatAdd : public MatExpr<MatAdd<E1, E2>> {
private:
    const E1 &a;
    const E2 &b;

public:
    MatAdd(const E1 &x, const E2 &y) : a(x), b(y) {}

    double operator()(int i, int j) const { return a(i, j) + b(i, j); }
    int rows() const { return a.rows(); }
    int cols() const { return a.cols(); }
};

// 행렬 스칼라 곱셈
template<typename E>
class MatScale : public MatExpr<MatScale<E>> {
private:
    const E &m;
    double s;

public:
    MatScale(const E &matrix, double scalar) : m(matrix), s(scalar) {}

    double operator()(int i, int j) const { return m(i, j) * s; }
    int rows() const { return m.rows(); }
    int cols() const { return m.cols(); }
};

// 행렬 곱셈 (행렬 크기가 다를 수 있음)
template<typename E1, typename E2>
class MatMul : public MatExpr<MatMul<E1, E2>> {
private:
    const E1 &a;
    const E2 &b;
    mutable std::vector<double> cache;
    mutable bool cached = false;

public:
    MatMul(const E1 &x, const E2 &y) : a(x), b(y) {}

    double operator()(int i, int j) const {
        if (!cached) {
            compute_cache();
            cached = true;
        }
        return cache[i * b.cols() + j];
    }

    int rows() const { return a.rows(); }
    int cols() const { return b.cols(); }

private:
    void compute_cache() const {
        cache.resize(a.rows() * b.cols(), 0);
        for (int i = 0; i < a.rows(); ++i) {
            for (int j = 0; j < b.cols(); ++j) {
                for (int k = 0; k < a.cols(); ++k) {
                    cache[i * b.cols() + j] += a(i, k) * b(k, j);
                }
            }
        }
    }
};

// 연산자
template<typename E1, typename E2>
MatAdd<E1, E2> operator+(const MatExpr<E1> &a, const MatExpr<E2> &b) {
    return MatAdd<E1, E2>(static_cast<const E1 &>(a), static_cast<const E2 &>(b));
}

template<typename E>
MatScale<E> operator*(const MatExpr<E> &m, double s) {
    return MatScale<E>(static_cast<const E &>(m), s);
}

template<typename E>
MatScale<E> operator*(double s, const MatExpr<E> &m) {
    return MatScale<E>(static_cast<const E &>(m), s);
}

template<typename E1, typename E2>
MatMul<E1, E2> operator*(const MatExpr<E1> &a, const MatExpr<E2> &b) {
    return MatMul<E1, E2>(static_cast<const E1 &>(a), static_cast<const E2 &>(b));
}

int main() {
    std::cout << "=== 표현식 템플릿 (행렬 연산) ===\n\n";

    std::cout << "1️⃣  행렬 초기화:\n";
    Matrix A(2, 3);
    A(0, 0) = 1; A(0, 1) = 2; A(0, 2) = 3;
    A(1, 0) = 4; A(1, 1) = 5; A(1, 2) = 6;

    Matrix B(2, 3);
    B(0, 0) = 7; B(0, 1) = 8; B(0, 2) = 9;
    B(1, 0) = 1; B(1, 1) = 2; B(1, 2) = 3;

    std::cout << "A =\n"; A.print();
    std::cout << "B =\n"; B.print();

    std::cout << "\n2️⃣  덧셈 (A + B):\n";
    Matrix C = A + B;
    std::cout << "C =\n"; C.print();

    std::cout << "\n3️⃣  스칼라 곱셈 (A * 2.0):\n";
    Matrix D = A * 2.0;
    std::cout << "D =\n"; D.print();

    std::cout << "\n4️⃣  복합 식 ((A + B) * 0.5):\n";
    Matrix E = (A + B) * 0.5;
    std::cout << "E =\n"; E.print();

    std::cout << "\n5️⃣  행렬 곱셈 (A × B^T):\n";
    Matrix B_T(3, 2);
    B_T(0, 0) = 7; B_T(0, 1) = 1;
    B_T(1, 0) = 8; B_T(1, 1) = 2;
    B_T(2, 0) = 9; B_T(2, 1) = 3;

    Matrix F = A * B_T;
    std::cout << "F = A × B_T =\n"; F.print();

    return 0;
}
