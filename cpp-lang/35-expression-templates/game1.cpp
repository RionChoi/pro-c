#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <cstdlib>
#include <ctime>

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

double calculate_distance(const Vec& v) {
    double sum = 0.0;
    for (std::size_t i = 0; i < v.size(); ++i) {
        sum += v[i] * v[i];
    }
    return std::sqrt(sum);
}

void print_vector(const Vec& v) {
    std::cout << "[";
    for (std::size_t i = 0; i < v.size(); ++i) {
        std::cout << v[i];
        if (i < v.size() - 1) std::cout << ", ";
    }
    std::cout << "]";
}

class ExpressionGame {
private:
    Vec target_;
    Vec a_, b_, c_;
    int level_;
    int score_;

public:
    ExpressionGame() : target_(3), a_(3), b_(3), c_(3), level_(1), score_(0) {
        generate_round();
    }

    void generate_round() {
        for (std::size_t i = 0; i < 3; ++i) {
            a_[i] = 1.0 + (rand() % 5);
            b_[i] = 1.0 + (rand() % 5);
            c_[i] = 1.0 + (rand() % 5);
        }

        switch (level_) {
            case 1:
                for (std::size_t i = 0; i < 3; ++i) {
                    target_[i] = a_[i] + b_[i];
                }
                break;
            case 2:
                for (std::size_t i = 0; i < 3; ++i) {
                    target_[i] = a_[i] * 2.0 - b_[i];
                }
                break;
            case 3:
                for (std::size_t i = 0; i < 3; ++i) {
                    target_[i] = (a_[i] + b_[i]) * 2.0 - c_[i];
                }
                break;
            default:
                for (std::size_t i = 0; i < 3; ++i) {
                    target_[i] = a_[i] * 2.0 + b_[i] - c_[i] * 0.5;
                }
        }
    }

    void show_problem() const {
        std::cout << "\n=== 레벨 " << level_ << " ===\n";
        std::cout << "벡터 a = ";
        print_vector(a_);
        std::cout << "\n벡터 b = ";
        print_vector(b_);
        std::cout << "\n벡터 c = ";
        print_vector(c_);
        std::cout << "\n\n목표 벡터 = ";
        print_vector(target_);
        std::cout << "\n\n";
    }

    void show_instructions() const {
        std::cout << "옵션:\n";
        std::cout << "1. a + b\n";
        std::cout << "2. a - b\n";
        std::cout << "3. a * 2\n";
        std::cout << "4. a * 2 - b\n";
        std::cout << "5. a * 2 + b - c\n";
        std::cout << "6. (a + b) * 2 - c\n";
        std::cout << "7. 다음 레벨로\n";
        std::cout << "선택 (1-7): ";
    }

    bool check_answer(int choice) {
        Vec result(3);

        switch (choice) {
            case 1:
                result = a_ + b_;
                break;
            case 2:
                result = a_ - b_;
                break;
            case 3:
                result = a_ * 2.0;
                break;
            case 4:
                result = a_ * 2.0 - b_;
                break;
            case 5:
                result = a_ * 2.0 + b_ - c_;
                break;
            case 6:
                result = (a_ + b_) * 2.0 - c_;
                break;
            default:
                return false;
        }

        std::cout << "\n계산 결과 = ";
        print_vector(result);
        std::cout << "\n";

        double target_dist = calculate_distance(target_);
        double result_dist = calculate_distance(result);
        double error = std::abs(target_dist - result_dist);

        if (error < 0.01) {
            std::cout << "✓ 정답! 점수: 10점\n";
            score_ += 10;
            return true;
        } else {
            std::cout << "✗ 틀렸습니다. (오차: " << error << ")\n";
            return false;
        }
    }

    void next_level() {
        level_++;
        if (level_ > 4) {
            level_ = 4;
        }
        generate_round();
        std::cout << "\n다음 레벨로 진행합니다!\n";
    }

    int get_score() const { return score_; }
    int get_level() const { return level_; }
};

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::cout << "=== 표현식 템플릿 게임 ===\n";
    std::cout << "벡터 연산 식을 맞추는 게임입니다.\n";
    std::cout << "표현식 템플릿의 힘을 느껴보세요!\n\n";

    ExpressionGame game;

    while (true) {
        game.show_problem();
        game.show_instructions();

        int choice;
        if (std::cin >> choice) {
            std::cin.ignore(1000, '\n');

            if (choice == 7) {
                game.next_level();
            } else if (choice >= 1 && choice <= 6) {
                game.check_answer(choice);
                std::cout << "현재 점수: " << game.get_score() << "\n";
            } else {
                std::cout << "유효하지 않은 선택입니다.\n";
            }
        }
    }

    return 0;
}
