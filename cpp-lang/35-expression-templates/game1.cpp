#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>

class Vector {
private:
    std::vector<double> data;

public:
    Vector() = default;
    explicit Vector(size_t size) : data(size, 0.0) {}

    size_t size() const { return data.size(); }

    double operator[](size_t i) const { return data[i]; }
    double& operator[](size_t i) { return data[i]; }

    void print(int max_elements = 5) const {
        std::cout << "[";
        size_t limit = std::min(static_cast<size_t>(max_elements), data.size());
        for (size_t i = 0; i < limit; i++) {
            if (i > 0) std::cout << ", ";
            std::cout << data[i];
        }
        if (data.size() > limit) std::cout << ", ...";
        std::cout << "]";
    }
};

struct Add { static double apply(double a, double b) { return a + b; } };
struct Sub { static double apply(double a, double b) { return a - b; } };
struct Mul { static double apply(double a, double b) { return a * b; } };
struct Div { static double apply(double a, double b) { return b != 0 ? a / b : 0; } };

template <typename L, typename R, typename Op>
class BinOp {
private:
    const L& lhs;
    const R& rhs;

public:
    BinOp(const L& l, const R& r) : lhs(l), rhs(r) {}
    double operator[](size_t i) const { return Op::apply(lhs[i], rhs[i]); }
    size_t size() const { return lhs.size(); }
};

class Scalar {
private:
    double value;

public:
    Scalar(double v) : value(v) {}
    double operator[](size_t) const { return value; }
};

BinOp<Vector, Vector, Add> operator+(const Vector& a, const Vector& b) {
    return BinOp<Vector, Vector, Add>(a, b);
}

BinOp<Vector, Vector, Sub> operator-(const Vector& a, const Vector& b) {
    return BinOp<Vector, Vector, Sub>(a, b);
}

BinOp<Vector, Vector, Mul> operator*(const Vector& a, const Vector& b) {
    return BinOp<Vector, Vector, Mul>(a, b);
}

BinOp<Vector, Scalar, Mul> operator*(const Vector& a, double s) {
    return BinOp<Vector, Scalar, Mul>(a, Scalar(s));
}

BinOp<Scalar, Vector, Mul> operator*(double s, const Vector& a) {
    return BinOp<Scalar, Vector, Mul>(Scalar(s), a);
}

template <typename L, typename R, typename Op>
BinOp<BinOp<L, R, Op>, Vector, Add> operator+(const BinOp<L, R, Op>& a, const Vector& b) {
    return BinOp<BinOp<L, R, Op>, Vector, Add>(a, b);
}

template <typename L1, typename R1, typename Op1, typename L2, typename R2, typename Op2>
BinOp<BinOp<L1, R1, Op1>, BinOp<L2, R2, Op2>, Add> operator+(const BinOp<L1, R1, Op1>& a, const BinOp<L2, R2, Op2>& b) {
    return BinOp<BinOp<L1, R1, Op1>, BinOp<L2, R2, Op2>, Add>(a, b);
}

template <typename L, typename R, typename Op>
BinOp<BinOp<L, R, Op>, Vector, Mul> operator*(const BinOp<L, R, Op>& a, const Vector& b) {
    return BinOp<BinOp<L, R, Op>, Vector, Mul>(a, b);
}

template <typename E>
Vector& assign_expr(Vector& result, const E& expr) {
    for (size_t i = 0; i < result.size(); i++) {
        result[i] = expr[i];
    }
    return result;
}

class ExpressionTemplateGame {
private:
    int player_score = 0;
    int cpu_score = 0;
    static constexpr int NUM_ROUNDS = 3;

public:
    void display_menu() const {
        std::cout << "\n=== 표현식 템플릿 성능 게임 ===\n";
        std::cout << "복잡한 표현식을 얼마나 빨리 처리할 수 있는지 겨루세요!\n\n";
    }

    int choose_complexity() {
        std::cout << "표현식 복잡도를 선택하세요:\n";
        std::cout << "1. 간단 (a + b, 1000번)\n";
        std::cout << "2. 중간 ((a + b) * c, 500번)\n";
        std::cout << "3. 복잡 ((a * 2.0) + (b * 3.0) + (c * 1.5), 300번)\n";
        std::cout << "선택 (1-3): ";

        int choice;
        if (std::cin >> choice && choice >= 1 && choice <= 3) {
            return choice;
        }
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return 1;
    }

    long long run_expression_test(int complexity, const std::string& name) {
        std::cout << "\n[테스트: " << name << "]\n";

        const size_t n = 10000;
        Vector a(n), b(n), c(n);

        for (size_t i = 0; i < n; i++) {
            a[i] = i + 1;
            b[i] = std::sin(i * 0.01);
            c[i] = std::cos(i * 0.01);
        }

        Vector result(n);
        auto start = std::chrono::high_resolution_clock::now();

        if (complexity == 1) {
            for (int iter = 0; iter < 1000; iter++) {
                assign_expr(result, a + b);
            }
        } else if (complexity == 2) {
            for (int iter = 0; iter < 500; iter++) {
                assign_expr(result, (a + b) * c);
            }
        } else {
            for (int iter = 0; iter < 300; iter++) {
                assign_expr(result, (a * 2.0) + (b * 3.0) + (c * 1.5));
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        std::cout << "소요 시간: " << duration.count() << " μs\n";
        return duration.count();
    }

    void play_round(int round_num) {
        std::cout << "\n========== 라운드 " << round_num << " ==========\n";

        int player_choice = choose_complexity();
        std::string complexity_names[] = {"", "간단", "중간", "복잡"};

        auto player_time = run_expression_test(player_choice, "당신의 선택");

        int cpu_choice = (std::rand() % 3) + 1;
        auto cpu_time = run_expression_test(cpu_choice, "CPU 선택 (" + complexity_names[cpu_choice] + ")");

        std::cout << "\n[라운드 결과]\n";
        std::cout << "당신: " << player_time << " μs\n";
        std::cout << "CPU: " << cpu_time << " μs\n";

        if (player_time < cpu_time) {
            std::cout << "축하합니다! 당신이 이겼습니다! (+10점)\n";
            player_score += 10;
        } else if (player_time > cpu_time) {
            std::cout << "CPU가 이겼습니다! (CPU +10점)\n";
            cpu_score += 10;
        } else {
            std::cout << "무승부입니다!\n";
        }

        std::cout << "현재 점수: 당신 " << player_score << " vs CPU " << cpu_score << "\n";
    }

    void display_final_results() {
        std::cout << "\n========== 최종 결과 ==========\n";
        std::cout << "당신의 점수: " << player_score << "\n";
        std::cout << "CPU의 점수: " << cpu_score << "\n";

        if (player_score > cpu_score) {
            std::cout << "\n축하합니다! 당신이 승리했습니다!\n";
        } else if (player_score < cpu_score) {
            std::cout << "\nCPU가 승리했습니다. 다시 도전하세요!\n";
        } else {
            std::cout << "\n동점입니다!\n";
        }
    }

    void play() {
        display_menu();

        for (int i = 1; i <= NUM_ROUNDS; i++) {
            play_round(i);
        }

        display_final_results();
    }
};

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    ExpressionTemplateGame game;
    game.play();

    std::cout << "\n게임 종료. 감사합니다!\n";
    return 0;
}
