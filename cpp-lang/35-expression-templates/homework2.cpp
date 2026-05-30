#include <iostream>
#include <vector>
#include <cmath>

template <typename Expr>
class VectorExpr {
private:
    Expr expr;

public:
    explicit VectorExpr(const Expr& e) : expr(e) {}
    explicit VectorExpr(Expr&& e) : expr(std::move(e)) {}

    double operator[](std::size_t i) const {
        return expr[i];
    }

    std::size_t size() const {
        return expr.size();
    }
};

class Vector {
private:
    std::vector<double> data;

public:
    Vector(std::size_t n) : data(n) {}

    Vector(const Vector& v) : data(v.data) {}

    template <typename Expr>
    Vector(const VectorExpr<Expr>& expr) : data(expr.size()) {
        for (std::size_t i = 0; i < expr.size(); i++) {
            data[i] = expr[i];
        }
    }

    Vector& operator=(const Vector& v) {
        if (this != &v) {
            data = v.data;
        }
        return *this;
    }

    template <typename Expr>
    Vector& operator=(const VectorExpr<Expr>& expr) {
        data.resize(expr.size());
        for (std::size_t i = 0; i < expr.size(); i++) {
            data[i] = expr[i];
        }
        return *this;
    }

    double& operator[](std::size_t i) { return data[i]; }
    double operator[](std::size_t i) const { return data[i]; }
    std::size_t size() const { return data.size(); }

    void display() const {
        std::cout << "[ ";
        for (double v : data) {
            printf("%.2f ", v);
        }
        std::cout << "]\n";
    }
};

template <typename Lhs, typename Rhs>
class VectorAdd {
private:
    const Lhs& lhs;
    const Rhs& rhs;

public:
    VectorAdd(const Lhs& l, const Rhs& r) : lhs(l), rhs(r) {}

    double operator[](std::size_t i) const {
        return lhs[i] + rhs[i];
    }

    std::size_t size() const {
        return lhs.size();
    }
};

template <typename Expr>
class VectorScale {
private:
    const Expr& expr;
    double scalar;

public:
    VectorScale(const Expr& e, double s) : expr(e), scalar(s) {}

    double operator[](std::size_t i) const {
        return expr[i] * scalar;
    }

    std::size_t size() const {
        return expr.size();
    }
};

template <typename Lhs, typename Rhs>
class VectorDot {
private:
    const Lhs& lhs;
    const Rhs& rhs;

public:
    VectorDot(const Lhs& l, const Rhs& r) : lhs(l), rhs(r) {}

    double compute() const {
        double result = 0.0;
        for (std::size_t i = 0; i < lhs.size(); i++) {
            result += lhs[i] * rhs[i];
        }
        return result;
    }
};

template <typename Expr>
auto operator+(const Vector& lhs, const VectorExpr<Expr>& rhs) {
    return VectorExpr<VectorAdd<Vector, VectorExpr<Expr>>>(
        VectorAdd<Vector, VectorExpr<Expr>>(lhs, rhs));
}

template <typename Expr>
auto operator+(const VectorExpr<Expr>& lhs, const Vector& rhs) {
    return VectorExpr<VectorAdd<VectorExpr<Expr>, Vector>>(
        VectorAdd<VectorExpr<Expr>, Vector>(lhs, rhs));
}

auto operator+(const Vector& lhs, const Vector& rhs) {
    return VectorExpr<VectorAdd<Vector, Vector>>(
        VectorAdd<Vector, Vector>(lhs, rhs));
}

VectorExpr<VectorScale<Vector>> operator*(const Vector& v, double scalar) {
    return VectorExpr<VectorScale<Vector>>(VectorScale<Vector>(v, scalar));
}

VectorExpr<VectorScale<Vector>> operator*(double scalar, const Vector& v) {
    return VectorExpr<VectorScale<Vector>>(VectorScale<Vector>(v, scalar));
}

double dot(const Vector& lhs, const Vector& rhs) {
    VectorDot<Vector, Vector> dot_prod(lhs, rhs);
    return dot_prod.compute();
}

int main() {
    std::cout << "=== 확장 표현식 템플릿: 연산 다양화 ===\n\n";

    Vector v1(4);
    Vector v2(4);

    for (std::size_t i = 0; i < 4; i++) {
        v1[i] = static_cast<double>(i + 1);
        v2[i] = static_cast<double>((i + 1) * 3);
    }

    std::cout << "v1: ";
    v1.display();

    std::cout << "v2: ";
    v2.display();

    Vector v3 = v1 * 2.0;
    std::cout << "\nv3 = v1 * 2.0: ";
    v3.display();

    Vector v4 = 3.0 * v2;
    std::cout << "v4 = 3.0 * v2: ";
    v4.display();

    Vector v5 = v1 + v2 * 2.0;
    std::cout << "v5 = v1 + v2 * 2.0: ";
    v5.display();

    double result = dot(v1, v2);
    std::cout << "\ndot(v1, v2) = " << result << "\n";

    std::cout << "\n지원되는 표현식:\n";
    std::cout << "- 덧셈: v1 + v2\n";
    std::cout << "- 스칼라 곱셈: v * 2.0 또는 3.0 * v\n";
    std::cout << "- 내적: dot(v1, v2)\n";
    std::cout << "- 복합: v1 + v2 * 2.0\n";

    return 0;
}
