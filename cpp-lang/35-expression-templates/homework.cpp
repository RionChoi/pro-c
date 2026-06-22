#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

// 표현식 템플릿을 위한 기본 클래스 (CRTP)
template<typename E>
class VecExpr {
public:
    double operator[](std::size_t i) const {
        return static_cast<const E &>(*this)[i];
    }

    std::size_t size() const {
        return static_cast<const E &>(*this).size();
    }
};

// 실제 벡터 (리프 노드)
class Vec : public VecExpr<Vec> {
private:
    std::vector<double> data;

public:
    Vec() = default;

    Vec(std::size_t n) : data(n) {}

    Vec(std::initializer_list<double> init) : data(init) {}

    template<typename E>
    Vec(const VecExpr<E> &expr) : data(expr.size()) {
        for (std::size_t i = 0; i < expr.size(); ++i) {
            data[i] = expr[i];
        }
    }

    template<typename E>
    Vec &operator=(const VecExpr<E> &expr) {
        data.resize(expr.size());
        for (std::size_t i = 0; i < expr.size(); ++i) {
            data[i] = expr[i];
        }
        return *this;
    }

    double operator[](std::size_t i) const { return data[i]; }
    double &operator[](std::size_t i) { return data[i]; }
    std::size_t size() const { return data.size(); }

    void print() const {
        std::cout << "[";
        for (std::size_t i = 0; i < data.size(); ++i) {
            std::cout << std::fixed << std::setprecision(2) << data[i];
            if (i + 1 < data.size()) std::cout << " ";
        }
        std::cout << "]\n";
    }
};

// 벡터 덧셈 표현식
template<typename E1, typename E2>
class VecAdd : public VecExpr<VecAdd<E1, E2>> {
private:
    const E1 &lhs;
    const E2 &rhs;

public:
    VecAdd(const E1 &a, const E2 &b) : lhs(a), rhs(b) {}

    double operator[](std::size_t i) const {
        return lhs[i] + rhs[i];
    }

    std::size_t size() const { return lhs.size(); }
};

// 벡터-스칼라 곱셈 표현식
template<typename E>
class VecScale : public VecExpr<VecScale<E>> {
private:
    const E &v;
    double scalar;

public:
    VecScale(const E &vec, double s) : v(vec), scalar(s) {}

    double operator[](std::size_t i) const {
        return v[i] * scalar;
    }

    std::size_t size() const { return v.size(); }
};

// 연산자 오버로딩
template<typename E1, typename E2>
VecAdd<E1, E2> operator+(const VecExpr<E1> &a, const VecExpr<E2> &b) {
    return VecAdd<E1, E2>(static_cast<const E1 &>(a), static_cast<const E2 &>(b));
}

template<typename E>
VecScale<E> operator*(const VecExpr<E> &v, double s) {
    return VecScale<E>(static_cast<const E &>(v), s);
}

template<typename E>
VecScale<E> operator*(double s, const VecExpr<E> &v) {
    return VecScale<E>(static_cast<const E &>(v), s);
}

int main() {
    std::cout << "=== 표현식 템플릿 (벡터 연산) ===\n\n";

    std::cout << "1️⃣  기본 벡터:\n";
    Vec a = {1.0, 2.0, 3.0};
    Vec b = {4.0, 5.0, 6.0};
    std::cout << "a = ";
    a.print();
    std::cout << "b = ";
    b.print();

    std::cout << "\n2️⃣  덧셈 (a + b):\n";
    Vec c = a + b;
    std::cout << "c = ";
    c.print();

    std::cout << "\n3️⃣  스칼라 곱셈 (a * 2.0):\n";
    Vec d = a * 2.0;
    std::cout << "d = ";
    d.print();

    std::cout << "\n4️⃣  복합 표현식 (a + b) * 2.0:\n";
    Vec e = (a + b) * 2.0;
    std::cout << "e = ";
    e.print();

    std::cout << "\n5️⃣  더 복잡한 식: a * 3.0 + b * 2.0:\n";
    Vec f = a * 3.0 + b * 2.0;
    std::cout << "f = ";
    f.print();

    std::cout << "\n✨ 임시 객체 생성 없이 표현식이 계산됩니다!\n";

    return 0;
}
