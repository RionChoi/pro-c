#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

class Vector {
    std::vector<double> data;
public:
    explicit Vector(size_t n = 0, double val = 0.0) : data(n, val) {}

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

    double magnitude() const {
        double sum = 0.0;
        for (const auto& val : data) {
            sum += val * val;
        }
        return std::sqrt(sum);
    }

    double dot(const Vector& v) const {
        double sum = 0.0;
        for (size_t i = 0; i < data.size(); ++i) {
            sum += data[i] * v.data[i];
        }
        return sum;
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

template<typename V>
auto operator*(const V& v, double s) {
    return ScalarExpr<V, Multiply>(v, s);
}

template<typename V>
auto operator*(double s, const V& v) {
    return ScalarExpr<V, Multiply>(v, s);
}

int main() {
    std::cout << "=== 선형 조합 계산기 (Expression Templates 게임) ===\n";
    std::cout << "벡터 v1, v2, v3의 선형 조합을 계산합니다.\n";
    std::cout << "결과: a*v1 + b*v2 + c*v3\n\n";

    while (true) {
        std::cout << "명령을 선택하세요:\n";
        std::cout << "  1. 벡터 입력\n";
        std::cout << "  2. 선형 조합 계산\n";
        std::cout << "  3. 벡터 정보 출력\n";
        std::cout << "  0. 종료\n";
        std::cout << "선택: ";

        int cmd;
        if (!(std::cin >> cmd)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "잘못된 입력입니다.\n\n";
            continue;
        }
        std::cin.ignore(10000, '\n');

        if (cmd == 0) {
            std::cout << "게임을 종료합니다.\n";
            break;
        } else if (cmd == 1) {
            // 벡터 입력
            std::cout << "\n벡터의 차원을 입력하세요 (2-5): ";
            int dim;
            if (!(std::cin >> dim) || dim < 2 || dim > 5) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "잘못된 차원입니다.\n\n";
                continue;
            }
            std::cin.ignore(10000, '\n');

            Vector v1(dim), v2(dim), v3(dim);

            std::cout << "\nv1의 요소를 입력하세요:\n";
            for (int i = 0; i < dim; ++i) {
                std::cout << "  [" << i << "]: ";
                if (!(std::cin >> v1[i])) {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << "입력 오류\n";
                    break;
                }
            }
            std::cin.ignore(10000, '\n');

            std::cout << "\nv2의 요소를 입력하세요:\n";
            for (int i = 0; i < dim; ++i) {
                std::cout << "  [" << i << "]: ";
                if (!(std::cin >> v2[i])) {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << "입력 오류\n";
                    break;
                }
            }
            std::cin.ignore(10000, '\n');

            std::cout << "\nv3의 요소를 입력하세요:\n";
            for (int i = 0; i < dim; ++i) {
                std::cout << "  [" << i << "]: ";
                if (!(std::cin >> v3[i])) {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                    std::cout << "입력 오류\n";
                    break;
                }
            }
            std::cin.ignore(10000, '\n');

            std::cout << "\n벡터가 저장되었습니다.\n";
            v1.print("v1");
            v2.print("v2");
            v3.print("v3");
            std::cout << "\n";

        } else if (cmd == 2) {
            // 선형 조합 계산
            std::cout << "\n선형 조합을 계산합니다: result = a*v1 + b*v2 + c*v3\n";
            std::cout << "a의 값: ";
            double a;
            if (!(std::cin >> a)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "입력 오류\n\n";
                continue;
            }

            std::cout << "b의 값: ";
            double b;
            if (!(std::cin >> b)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "입력 오류\n\n";
                continue;
            }

            std::cout << "c의 값: ";
            double c;
            if (!(std::cin >> c)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                std::cout << "입력 오류\n\n";
                continue;
            }
            std::cin.ignore(10000, '\n');

            // 임시로 벡터들을 생성합니다 (실제 게임에서는 저장된 벡터 사용)
            Vector v1({1.0, 2.0, 3.0}), v2({4.0, 5.0, 6.0}), v3({7.0, 8.0, 9.0});

            // Expression Template를 사용한 계산
            Vector result(3);
            result = a * v1 + b * v2 + c * v3;

            std::cout << "\n계산 결과:\n";
            std::cout << std::fixed << std::setprecision(2);
            std::cout << a << "*v1 + " << b << "*v2 + " << c << "*v3\n";
            result.print("result");

            std::cout << "\n벡터 크기: " << result.magnitude() << "\n";
            std::cout << "스칼라 곱 (v1·result): " << v1.dot(result) << "\n\n";

        } else if (cmd == 3) {
            // 벡터 정보 출력
            Vector v1({1.0, 2.0, 3.0}), v2({4.0, 5.0, 6.0}), v3({7.0, 8.0, 9.0});

            std::cout << "\n현재 저장된 벡터:\n";
            v1.print("v1");
            v2.print("v2");
            v3.print("v3");

            std::cout << "크기:\n";
            std::cout << "  ||v1|| = " << v1.magnitude() << "\n";
            std::cout << "  ||v2|| = " << v2.magnitude() << "\n";
            std::cout << "  ||v3|| = " << v3.magnitude() << "\n";

            std::cout << "\n스칼라 곱:\n";
            std::cout << "  v1·v2 = " << v1.dot(v2) << "\n";
            std::cout << "  v1·v3 = " << v1.dot(v3) << "\n";
            std::cout << "  v2·v3 = " << v2.dot(v3) << "\n\n";

        } else {
            std::cout << "잘못된 명령입니다.\n\n";
        }
    }

    return 0;
}
