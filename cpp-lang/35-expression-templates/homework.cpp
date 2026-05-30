#include <iostream>
#include <vector>
#include <array>

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
            std::cout << v << " ";
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

template <typename Lhs, typename Rhs>
VectorExpr<VectorAdd<Lhs, Rhs>> operator+(const VectorExpr<Lhs>& lhs,
                                          const VectorExpr<Rhs>& rhs) {
    return VectorExpr<VectorAdd<Lhs, Rhs>>(
        VectorAdd<Lhs, Rhs>(lhs, rhs));
}

auto operator+(const Vector& lhs, const Vector& rhs) {
    return VectorExpr<VectorAdd<Vector, Vector>>(
        VectorAdd<Vector, Vector>(lhs, rhs));
}

int main() {
    std::cout << "=== 표현식 템플릿 (Expression Templates) ===\n\n";

    Vector v1(5);
    Vector v2(5);

    for (std::size_t i = 0; i < 5; i++) {
        v1[i] = static_cast<double>(i + 1);
        v2[i] = static_cast<double>((i + 1) * 2);
    }

    std::cout << "v1: ";
    v1.display();

    std::cout << "v2: ";
    v2.display();

    Vector v3 = v1 + v2;

    std::cout << "\nv3 = v1 + v2: ";
    v3.display();

    Vector v4 = v1 + v2 + v3;

    std::cout << "v4 = v1 + v2 + v3: ";
    v4.display();

    std::cout << "\n표현식 템플릿 장점:\n";
    std::cout << "- 중간 벡터 생성 없음\n";
    std::cout << "- 메모리 할당 최소화\n";
    std::cout << "- 컴파일러 최적화 향상\n";

    return 0;
}
