#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

class Vector {
    std::vector<double> data;
public:
    explicit Vector(size_t n, double val = 0.0) : data(n, val) {}

    Vector(const std::vector<double>& v) : data(v) {}

    size_t size() const { return data.size(); }

    double operator[](size_t i) const { return data[i]; }
    double& operator[](size_t i) { return data[i]; }

    Vector& operator=(const Vector& v) {
        data = v.data;
        return *this;
    }

    template<typename E>
    Vector& operator=(const E& expr) {
        for (size_t i = 0; i < data.size(); ++i) {
            data[i] = expr[i];
        }
        return *this;
    }

    void print(const std::string& name = "Vector") const {
        std::cout << name << " = [";
        for (size_t i = 0; i < data.size(); ++i) {
            std::cout << std::fixed << std::setprecision(2) << data[i];
            if (i < data.size() - 1) std::cout << ", ";
        }
        std::cout << "]\n";
    }
};

struct Add {
    double operator()(double a, double b) const { return a + b; }
};

struct Subtract {
    double operator()(double a, double b) const { return a - b; }
};

struct Multiply {
    double operator()(double a, double b) const { return a * b; }
};

struct Divide {
    double operator()(double a, double b) const { return a / b; }
};

template<typename Left, typename Right, typename Op>
class BinaryExpr {
    const Left& left;
    const Right& right;
    Op op;
public:
    BinaryExpr(const Left& l, const Right& r) : left(l), right(r) {}

    double operator[](size_t i) const {
        return op(left[i], right[i]);
    }

    size_t size() const { return left.size(); }
};

template<typename V, typename Op>
class ScalarExpr {
    const V& vec;
    double scalar;
    Op op;
public:
    ScalarExpr(const V& v, double s) : vec(v), scalar(s) {}

    double operator[](size_t i) const {
        return op(vec[i], scalar);
    }

    size_t size() const { return vec.size(); }
};

template<typename L, typename R>
auto operator+(const L& l, const R& r) {
    return BinaryExpr<L, R, Add>(l, r);
}

template<typename L, typename R>
auto operator-(const L& l, const R& r) {
    return BinaryExpr<L, R, Subtract>(l, r);
}

template<typename L, typename R>
auto operator*(const L& l, const R& r) {
    return BinaryExpr<L, R, Multiply>(l, r);
}

template<typename L, typename R>
auto operator/(const L& l, const R& r) {
    return BinaryExpr<L, R, Divide>(l, r);
}

template<typename V>
auto operator*(const V& v, double s) {
    return ScalarExpr<V, Multiply>(v, s);
}

template<typename V>
auto operator*(double s, const V& v) {
    return ScalarExpr<V, Multiply>(v, s);
}

template<typename V>
auto operator+(const V& v, double s) {
    return ScalarExpr<V, Add>(v, s);
}

template<typename V>
auto operator+(double s, const V& v) {
    return ScalarExpr<V, Add>(v, s);
}

int main() {
    std::cout << "=== Expression Templates 기초 ===\n\n";

    // 벡터 생성
    Vector v1({1.0, 2.0, 3.0});
    Vector v2({4.0, 5.0, 6.0});
    Vector v3({7.0, 8.0, 9.0});

    v1.print("v1");
    v2.print("v2");
    v3.print("v3");
    std::cout << "\n";

    // 1. 단순 덧셈 (표현식: v1 + v2)
    Vector result1(3);
    result1 = v1 + v2;
    result1.print("v1 + v2");
    std::cout << "\n";

    // 2. 뺄셈
    Vector result2(3);
    result2 = v2 - v1;
    result2.print("v2 - v1");
    std::cout << "\n";

    // 3. 스칼라 곱셈
    Vector result3(3);
    result3 = v1 * 2.5;
    result3.print("v1 * 2.5");
    std::cout << "\n";

    // 4. 복합 표현식 (v1 + v2 - v3)
    // 이 표현식은 런타임에 계산되지 않고, operator[]가 호출될 때만 계산됨
    Vector result4(3);
    result4 = v1 + v2 - v3;
    result4.print("v1 + v2 - v3");
    std::cout << "\n";

    // 5. 더 복잡한 표현식 (2.0 * v1 + v2)
    Vector result5(3);
    result5 = 2.0 * v1 + v2;
    result5.print("2.0 * v1 + v2");
    std::cout << "\n";

    // 6. 곱셈과 덧셈의 조합 (v1 * 2.0 + v2 * 3.0)
    Vector result6(3);
    result6 = v1 * 2.0 + v2 * 3.0;
    result6.print("v1 * 2.0 + v2 * 3.0");
    std::cout << "\n";

    // 7. 나눗셈
    Vector result7(3);
    result7 = v2 / v1;
    result7.print("v2 / v1");
    std::cout << "\n";

    std::cout << "모든 표현식이 지연 평가(lazy evaluation)로 계산되었습니다!\n";
    std::cout << "임시 벡터 객체가 생성되지 않으므로 메모리 효율성이 높습니다.\n";

    return 0;
}
