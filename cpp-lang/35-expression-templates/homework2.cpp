#include <iostream>
#include <vector>
#include <cmath>

// 간단한 행렬 클래스
template <typename T>
class Matrix {
private:
    std::vector<T> data;
    size_t rows, cols;

public:
    Matrix(size_t r, size_t c) : rows(r), cols(c), data(r * c, 0) {}

    size_t get_rows() const { return rows; }
    size_t get_cols() const { return cols; }

    T& at(size_t r, size_t c) { return data[r * cols + c]; }
    const T& at(size_t r, size_t c) const { return data[r * cols + c]; }

    void display() const {
        for (size_t r = 0; r < rows; ++r) {
            std::cout << "   [";
            for (size_t c = 0; c < cols; ++c) {
                std::cout << at(r, c);
                if (c < cols - 1) std::cout << ", ";
            }
            std::cout << "]\n";
        }
    }
};

// 행렬 표현식 기본
template <typename E>
class MatrixExpr {
public:
    const E& derived() const {
        return static_cast<const E&>(*this);
    }

    double at(size_t r, size_t c) const {
        return derived().at(r, c);
    }

    size_t get_rows() const { return derived().get_rows(); }
    size_t get_cols() const { return derived().get_cols(); }
};

// 행렬 리터럴
template <typename T>
class MatrixLiteral : public MatrixExpr<MatrixLiteral<T>> {
private:
    const Matrix<T>& mat;

public:
    MatrixLiteral(const Matrix<T>& m) : mat(m) {}

    double at(size_t r, size_t c) const { return mat.at(r, c); }
    size_t get_rows() const { return mat.get_rows(); }
    size_t get_cols() const { return mat.get_cols(); }
};

// 행렬 덧셈 표현식
template <typename E1, typename E2>
class MatrixAdd : public MatrixExpr<MatrixAdd<E1, E2>> {
private:
    const E1& left;
    const E2& right;

public:
    MatrixAdd(const E1& l, const E2& r) : left(l), right(r) {}

    double at(size_t r, size_t c) const {
        return left.at(r, c) + right.at(r, c);
    }

    size_t get_rows() const { return left.get_rows(); }
    size_t get_cols() const { return left.get_cols(); }
};

// 행렬 스칼라 곱셈 표현식
template <typename E>
class MatrixScale : public MatrixExpr<MatrixScale<E>> {
private:
    const E& expr;
    double scalar;

public:
    MatrixScale(const E& e, double s) : expr(e), scalar(s) {}

    double at(size_t r, size_t c) const {
        return expr.at(r, c) * scalar;
    }

    size_t get_rows() const { return expr.get_rows(); }
    size_t get_cols() const { return expr.get_cols(); }
};

// 연산자 오버로딩
template <typename E1, typename E2>
auto operator+(const MatrixExpr<E1>& lhs, const MatrixExpr<E2>& rhs) {
    return MatrixAdd<E1, E2>(lhs.derived(), rhs.derived());
}

template <typename E>
auto operator*(const MatrixExpr<E>& expr, double scalar) {
    return MatrixScale<E>(expr.derived(), scalar);
}

int main() {
    std::cout << "=== 행렬 표현식 템플릿 ===\n\n";

    Matrix<double> m1(2, 3);
    m1.at(0, 0) = 1.0; m1.at(0, 1) = 2.0; m1.at(0, 2) = 3.0;
    m1.at(1, 0) = 4.0; m1.at(1, 1) = 5.0; m1.at(1, 2) = 6.0;

    Matrix<double> m2(2, 3);
    m2.at(0, 0) = 1.0; m2.at(0, 1) = 1.0; m2.at(0, 2) = 1.0;
    m2.at(1, 0) = 2.0; m2.at(1, 1) = 2.0; m2.at(1, 2) = 2.0;

    std::cout << "1. 초기 행렬:\n";
    std::cout << "M1:\n";
    m1.display();
    std::cout << "M2:\n";
    m2.display();
    std::cout << "\n";

    std::cout << "2. 행렬 덧셈 (지연 평가):\n";
    auto m1_lit = MatrixLiteral(m1);
    auto m2_lit = MatrixLiteral(m2);
    auto sum_expr = m1_lit + m2_lit;

    std::cout << "M1 + M2:\n";
    for (size_t r = 0; r < 2; ++r) {
        std::cout << "   [";
        for (size_t c = 0; c < 3; ++c) {
            std::cout << sum_expr.at(r, c);
            if (c < 2) std::cout << ", ";
        }
        std::cout << "]\n";
    }
    std::cout << "\n";

    std::cout << "3. 스칼라 곱셈과 덧셈 조합:\n";
    auto scaled_m1 = m1_lit * 2.0;
    auto scaled_m2 = m2_lit * 3.0;
    auto combined = scaled_m1 + scaled_m2;

    std::cout << "2*M1 + 3*M2:\n";
    for (size_t r = 0; r < 2; ++r) {
        std::cout << "   [";
        for (size_t c = 0; c < 3; ++c) {
            std::cout << combined.at(r, c);
            if (c < 2) std::cout << ", ";
        }
        std::cout << "]\n";
    }

    std::cout << "\n표현식 템플릿으로 중간 행렬 생성 최소화!\n";

    return 0;
}
