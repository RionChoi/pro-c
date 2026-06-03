#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <iomanip>
#include <algorithm>

template <size_t N>
class Vector {
private:
    std::array<double, N> data;

public:
    Vector() : data{} {}

    explicit Vector(const std::array<double, N>& values) : data(values) {}

    Vector(std::initializer_list<double> init) {
        auto it = init.begin();
        for (size_t i = 0; i < N && it != init.end(); ++i, ++it) {
            data[i] = *it;
        }
    }

    double& operator[](size_t i) { return data[i]; }
    double operator[](size_t i) const { return data[i]; }

    Vector operator+(const Vector& other) const {
        Vector result;
        for (size_t i = 0; i < N; ++i) {
            result.data[i] = data[i] + other.data[i];
        }
        return result;
    }

    Vector operator*(double scalar) const {
        Vector result;
        for (size_t i = 0; i < N; ++i) {
            result.data[i] = data[i] * scalar;
        }
        return result;
    }

    double dot(const Vector& other) const {
        double sum = 0.0;
        for (size_t i = 0; i < N; ++i) {
            sum += data[i] * other.data[i];
        }
        return sum;
    }

    double magnitude() const {
        return std::sqrt(dot(*this));
    }

    Vector normalize() const {
        double mag = magnitude();
        if (mag < 1e-10) return Vector();
        return *this * (1.0 / mag);
    }

    void print(const std::string& name = "Vector") const {
        std::cout << name << " [" << N << "D]: ";
        for (size_t i = 0; i < N; ++i) {
            std::cout << std::fixed << std::setprecision(2) << data[i];
            if (i < N - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }

    static constexpr size_t dimension() { return N; }
};

template <size_t M, size_t N>
class Matrix {
private:
    std::vector<std::vector<double>> data;

public:
    Matrix() : data(M, std::vector<double>(N, 0.0)) {}

    double& at(size_t i, size_t j) { return data[i][j]; }
    double at(size_t i, size_t j) const { return data[i][j]; }

    Matrix operator+(const Matrix& other) const {
        Matrix result;
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                result.data[i][j] = data[i][j] + other.data[i][j];
            }
        }
        return result;
    }

    Matrix operator*(double scalar) const {
        Matrix result;
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                result.data[i][j] = data[i][j] * scalar;
            }
        }
        return result;
    }

    template <size_t P>
    Matrix<M, P> multiply(const Matrix<N, P>& other) const {
        Matrix<M, P> result;
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < P; ++j) {
                double sum = 0.0;
                for (size_t k = 0; k < N; ++k) {
                    sum += data[i][k] * other.at(k, j);
                }
                result.at(i, j) = sum;
            }
        }
        return result;
    }

    void print(const std::string& name = "Matrix") const {
        std::cout << name << " [" << M << "x" << N << "]:" << std::endl;
        for (size_t i = 0; i < M; ++i) {
            std::cout << "  ";
            for (size_t j = 0; j < N; ++j) {
                std::cout << std::fixed << std::setprecision(2) << std::setw(8) << data[i][j];
            }
            std::cout << std::endl;
        }
    }

    static constexpr size_t rows() { return M; }
    static constexpr size_t cols() { return N; }
};

template <typename T>
class ExpressionBase {
public:
    double evaluate(size_t index) const {
        return static_cast<const T&>(*this).evaluate(index);
    }

    size_t size() const {
        return static_cast<const T&>(*this).size();
    }
};

template <size_t N>
class VectorExpr : public ExpressionBase<VectorExpr<N>> {
private:
    const Vector<N>& vec;

public:
    explicit VectorExpr(const Vector<N>& v) : vec(v) {}

    double evaluate(size_t index) const {
        return vec[index];
    }

    size_t size() const { return N; }
};

template <typename E1, typename E2>
class AddExpr : public ExpressionBase<AddExpr<E1, E2>> {
private:
    const E1& expr1;
    const E2& expr2;

public:
    AddExpr(const E1& e1, const E2& e2) : expr1(e1), expr2(e2) {}

    double evaluate(size_t index) const {
        return expr1.evaluate(index) + expr2.evaluate(index);
    }

    size_t size() const { return expr1.size(); }
};

template <typename E>
class ScaleExpr : public ExpressionBase<ScaleExpr<E>> {
private:
    const E& expr;
    double scalar;

public:
    ScaleExpr(const E& e, double s) : expr(e), scalar(s) {}

    double evaluate(size_t index) const {
        return expr.evaluate(index) * scalar;
    }

    size_t size() const { return expr.size(); }
};

template <size_t N, typename E>
Vector<N> evaluate_expression(const ExpressionBase<E>& expr) {
    Vector<N> result;
    const E& e = static_cast<const E&>(expr);
    for (size_t i = 0; i < N; ++i) {
        result[i] = e.evaluate(i);
    }
    return result;
}

int main() {
    std::cout << "=== Advanced Numerical Engine ===" << std::endl << std::endl;

    std::cout << "1. Vector Operations (4D vectors):" << std::endl;
    Vector<4> v1{1.0, 2.0, 3.0, 4.0};
    Vector<4> v2{5.0, 6.0, 7.0, 8.0};

    v1.print("v1");
    v2.print("v2");

    Vector<4> v_sum = v1 + v2;
    v_sum.print("v1 + v2");

    Vector<4> v_scaled = v1 * 2.5;
    v_scaled.print("v1 * 2.5");

    double dot_product = v1.dot(v2);
    std::cout << "v1 dot v2 = " << std::fixed << std::setprecision(2) << dot_product << std::endl;

    double mag1 = v1.magnitude();
    std::cout << "|v1| = " << std::fixed << std::setprecision(2) << mag1 << std::endl;

    Vector<4> v1_norm = v1.normalize();
    v1_norm.print("normalized v1");
    std::cout << std::endl;

    std::cout << "2. 3D Vector Cross-product simulation:" << std::endl;
    Vector<3> u{1.0, 0.0, 0.0};
    Vector<3> v{0.0, 1.0, 0.0};
    u.print("u");
    v.print("v");
    Vector<3> u_cross_v{
        u[1] * v[2] - u[2] * v[1],
        u[2] * v[0] - u[0] * v[2],
        u[0] * v[1] - u[1] * v[0]
    };
    u_cross_v.print("u x v");
    std::cout << std::endl;

    std::cout << "3. Matrix Operations (3x3):" << std::endl;
    Matrix<3, 3> A;
    A.at(0, 0) = 1.0; A.at(0, 1) = 2.0; A.at(0, 2) = 3.0;
    A.at(1, 0) = 4.0; A.at(1, 1) = 5.0; A.at(1, 2) = 6.0;
    A.at(2, 0) = 7.0; A.at(2, 1) = 8.0; A.at(2, 2) = 9.0;

    Matrix<3, 3> B;
    B.at(0, 0) = 9.0; B.at(0, 1) = 8.0; B.at(0, 2) = 7.0;
    B.at(1, 0) = 6.0; B.at(1, 1) = 5.0; B.at(1, 2) = 4.0;
    B.at(2, 0) = 3.0; B.at(2, 1) = 2.0; B.at(2, 2) = 1.0;

    A.print("Matrix A");
    B.print("Matrix B");

    Matrix<3, 3> C = A + B;
    C.print("A + B");

    Matrix<3, 3> D = A * 0.5;
    D.print("A * 0.5");

    Matrix<3, 3> E = A.multiply(B);
    E.print("A x B");
    std::cout << std::endl;

    std::cout << "4. Expression Templates (Lazy Evaluation):" << std::endl;
    Vector<4> x{1.0, 2.0, 3.0, 4.0};
    Vector<4> y{2.0, 3.0, 4.0, 5.0};

    x.print("x");
    y.print("y");

    VectorExpr<4> expr_x(x);
    VectorExpr<4> expr_y(y);
    AddExpr<VectorExpr<4>, VectorExpr<4>> add_expr(expr_x, expr_y);
    ScaleExpr<AddExpr<VectorExpr<4>, VectorExpr<4>>> scale_expr(add_expr, 0.5);

    Vector<4> result = evaluate_expression<4>(scale_expr);
    result.print("(x + y) * 0.5");
    std::cout << std::endl;

    std::cout << "5. Dimension Validation (constexpr):" << std::endl;
    std::cout << "Vector<4> dimension: " << Vector<4>::dimension() << std::endl;
    std::cout << "Vector<3> dimension: " << Vector<3>::dimension() << std::endl;
    std::cout << "Matrix<3,3> dimensions: " << Matrix<3, 3>::rows() << "x" << Matrix<3, 3>::cols() << std::endl;
    std::cout << "Matrix<2,4> dimensions: " << Matrix<2, 4>::rows() << "x" << Matrix<2, 4>::cols() << std::endl;

    return 0;
}
