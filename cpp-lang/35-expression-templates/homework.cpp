#include <iostream>
#include <vector>
#include <cmath>

template<typename E>
class VecExpr {
public:
    double operator[](std::size_t i) const {
        return static_cast<const E&>(*this)[i];
    }

    std::size_t size() const {
        return static_cast<const E&>(*this).size();
    }
};

class Vec : public VecExpr<Vec> {
private:
    std::vector<double> data_;

public:
    Vec(std::size_t n) : data_(n) {}

    Vec(const Vec& other) : data_(other.data_) {}

    template<typename E>
    Vec(const VecExpr<E>& expr) {
        const E& e = static_cast<const E&>(expr);
        data_.resize(e.size());
        for (std::size_t i = 0; i < e.size(); ++i) {
            data_[i] = e[i];
        }
    }

    template<typename E>
    Vec& operator=(const VecExpr<E>& expr) {
        const E& e = static_cast<const E&>(expr);
        if (data_.size() != e.size()) {
            data_.resize(e.size());
        }
        for (std::size_t i = 0; i < e.size(); ++i) {
            data_[i] = e[i];
        }
        return *this;
    }

    double& operator[](std::size_t i) { return data_[i]; }
    double operator[](std::size_t i) const { return data_[i]; }
    std::size_t size() const { return data_.size(); }
};

template<typename L, typename R>
class VecAdd : public VecExpr<VecAdd<L, R>> {
private:
    const L& l_;
    const R& r_;

public:
    VecAdd(const L& l, const R& r) : l_(l), r_(r) {}

    double operator[](std::size_t i) const {
        return l_[i] + r_[i];
    }

    std::size_t size() const { return l_.size(); }
};

template<typename L, typename R>
class VecSub : public VecExpr<VecSub<L, R>> {
private:
    const L& l_;
    const R& r_;

public:
    VecSub(const L& l, const R& r) : l_(l), r_(r) {}

    double operator[](std::size_t i) const {
        return l_[i] - r_[i];
    }

    std::size_t size() const { return l_.size(); }
};

template<typename E>
class VecScale : public VecExpr<VecScale<E>> {
private:
    const E& e_;
    double scalar_;

public:
    VecScale(const E& e, double s) : e_(e), scalar_(s) {}

    double operator[](std::size_t i) const {
        return e_[i] * scalar_;
    }

    std::size_t size() const { return e_.size(); }
};

template<typename L, typename R>
VecAdd<L, R> operator+(const VecExpr<L>& l, const VecExpr<R>& r) {
    return VecAdd<L, R>(static_cast<const L&>(l), static_cast<const R&>(r));
}

template<typename L, typename R>
VecSub<L, R> operator-(const VecExpr<L>& l, const VecExpr<R>& r) {
    return VecSub<L, R>(static_cast<const L&>(l), static_cast<const R&>(r));
}

template<typename E>
VecScale<E> operator*(const VecExpr<E>& e, double s) {
    return VecScale<E>(static_cast<const E&>(e), s);
}

template<typename E>
VecScale<E> operator*(double s, const VecExpr<E>& e) {
    return VecScale<E>(static_cast<const E&>(e), s);
}

double dot_product(const Vec& a, const Vec& b) {
    double result = 0.0;
    for (std::size_t i = 0; i < a.size(); ++i) {
        result += a[i] * b[i];
    }
    return result;
}

void print_vec(const Vec& v, const std::string& name = "") {
    if (!name.empty()) {
        std::cout << name << " = [";
    } else {
        std::cout << "[";
    }
    for (std::size_t i = 0; i < v.size(); ++i) {
        std::cout << v[i];
        if (i < v.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n";
}

int main() {
    std::cout << "=== 표현식 템플릿 (Expression Templates) ===\n\n";

    // 기본 벡터 생성
    std::cout << "1. 벡터 생성 및 초기화:\n";
    Vec a(5);
    Vec b(5);
    for (std::size_t i = 0; i < 5; ++i) {
        a[i] = i + 1;
        b[i] = (i + 1) * 2;
    }
    print_vec(a, "a");
    print_vec(b, "b");

    std::cout << "\n2. 벡터 덧셈 (표현식 템플릿):\n";
    Vec c = a + b;
    print_vec(c, "c = a + b");

    std::cout << "\n3. 벡터 뺄셈:\n";
    Vec d = a - b;
    print_vec(d, "d = a - b");

    std::cout << "\n4. 스칼라 곱셈:\n";
    Vec e = a * 2.0;
    print_vec(e, "e = a * 2.0");

    std::cout << "\n5. 복합 연산 (한 번에 계산, 임시 벡터 없음):\n";
    Vec result = a * 2.0 + b - a;
    print_vec(result, "result = a*2.0 + b - a");

    std::cout << "\n6. 내적 계산:\n";
    Vec v1(3);
    Vec v2(3);
    v1[0] = 1; v1[1] = 2; v1[2] = 3;
    v2[0] = 4; v2[1] = 5; v2[2] = 6;
    double dp = dot_product(v1, v2);
    std::cout << "dot_product([1,2,3], [4,5,6]) = " << dp << "\n";

    std::cout << "\n7. 표현식 템플릿의 장점:\n";
    std::cout << "- 임시 벡터 생성 없음 (메모리 효율)\n";
    std::cout << "- 컴파일러 최적화 기회 증가\n";
    std::cout << "- 지연 평가(Lazy Evaluation)\n";

    return 0;
}
