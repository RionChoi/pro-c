#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <cstdlib>
#include <ctime>

// 벡터 표현식 (게임용)
template<typename E>
class Vec {
public:
    double operator[](int i) const {
        return static_cast<const E &>(*this)[i];
    }

    int size() const {
        return static_cast<const E &>(*this).size();
    }
};

class Vector : public Vec<Vector> {
private:
    std::vector<double> data;

public:
    Vector(int n) : data(n) {}

    template<typename E>
    Vector(const Vec<E> &v) : data(v.size()) {
        for (int i = 0; i < v.size(); ++i) {
            data[i] = v[i];
        }
    }

    double operator[](int i) const { return data[i]; }
    double &operator[](int i) { return data[i]; }
    int size() const { return data.size(); }

    double length() const {
        double sum = 0;
        for (int i = 0; i < (int)data.size(); ++i) {
            sum += data[i] * data[i];
        }
        return std::sqrt(sum);
    }

    double dot(const Vector &other) const {
        double sum = 0;
        for (int i = 0; i < (int)data.size(); ++i) {
            sum += data[i] * other[i];
        }
        return sum;
    }

    void print() const {
        for (int i = 0; i < (int)data.size(); ++i) {
            std::cout << std::fixed << std::setprecision(1) << data[i] << " ";
        }
    }
};

template<typename E1, typename E2>
class VecAdd : public Vec<VecAdd<E1, E2>> {
private:
    const E1 &a;
    const E2 &b;

public:
    VecAdd(const E1 &x, const E2 &y) : a(x), b(y) {}

    double operator[](int i) const { return a[i] + b[i]; }
    int size() const { return a.size(); }
};

template<typename E>
class VecScale : public Vec<VecScale<E>> {
private:
    const E &v;
    double s;

public:
    VecScale(const E &vec, double scalar) : v(vec), s(scalar) {}

    double operator[](int i) const { return v[i] * s; }
    int size() const { return v.size(); }
};

template<typename E1, typename E2>
VecAdd<E1, E2> operator+(const Vec<E1> &a, const Vec<E2> &b) {
    return VecAdd<E1, E2>(static_cast<const E1 &>(a), static_cast<const E2 &>(b));
}

template<typename E>
VecScale<E> operator*(const Vec<E> &v, double s) {
    return VecScale<E>(static_cast<const E &>(v), s);
}

int main() {
    std::cout << "🎮 벡터 표현식 게임: 물리 시뮬레이션\n";
    std::cout << "=== 힘의 합성과 이동 거리를 계산하세요 ===\n\n";

    std::srand(std::time(nullptr));
    int score = 0;

    for (int round = 1; round <= 3; ++round) {
        std::cout << "🔹 라운드 " << round << ":\n";

        Vector f1(2);
        Vector f2(2);

        f1[0] = 3.0 + (std::rand() % 5);
        f1[1] = 2.0 + (std::rand() % 3);
        f2[0] = 2.0 + (std::rand() % 4);
        f2[1] = 3.0 + (std::rand() % 3);

        std::cout << "힘1: ("; f1.print(); std::cout << ")\n";
        std::cout << "힘2: ("; f2.print(); std::cout << ")\n";

        std::cout << "합성 힘의 크기를 계산하세요 (F_total = F1 + F2): ";
        double guess_magnitude;
        std::cin >> guess_magnitude;

        // 표현식 템플릿 사용
        Vector result = f1 + f2;
        double actual = result.length();
        double error = std::abs(guess_magnitude - actual);

        std::cout << "계산 결과: ("; result.print(); std::cout << ")\n";
        std::cout << "합성 힘의 크기: " << std::fixed << std::setprecision(2) << actual << "\n";

        int round_score = std::max(0, 100 - (int)(error * 50));
        score += round_score;

        std::cout << "정확도: " << round_score << "/100\n";
        std::cout << "누적 점수: " << score << "\n\n";
    }

    std::cout << "🏁 게임 완료!\n";
    std::cout << "최종 점수: " << score << " / 300\n";

    if (score >= 250) {
        std::cout << "⭐ 탁월한 성과! 물리 엔진 개발자급!\n";
    } else if (score >= 150) {
        std::cout << "👍 좋은 결과! 계속 연습하면 더 나아집니다.\n";
    } else {
        std::cout << "📚 더 많은 연습이 필요합니다.\n";
    }

    return 0;
}
