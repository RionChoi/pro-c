#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

template <typename Expr>
class MatrixExpr {
private:
    Expr expr;

public:
    explicit MatrixExpr(const Expr& e) : expr(e) {}
    explicit MatrixExpr(Expr&& e) : expr(std::move(e)) {}

    double operator()(std::size_t i, std::size_t j) const {
        return expr(i, j);
    }

    std::size_t rows() const { return expr.rows(); }
    std::size_t cols() const { return expr.cols(); }
};

class Matrix {
private:
    std::vector<std::vector<double>> data;
    std::size_t m, n;

public:
    Matrix(std::size_t rows, std::size_t cols) : m(rows), n(cols) {
        data.resize(rows, std::vector<double>(cols, 0.0));
    }

    double& operator()(std::size_t i, std::size_t j) {
        return data[i][j];
    }

    double operator()(std::size_t i, std::size_t j) const {
        return data[i][j];
    }

    std::size_t rows() const { return m; }
    std::size_t cols() const { return n; }

    void display() const {
        for (std::size_t i = 0; i < m; i++) {
            std::cout << "[ ";
            for (std::size_t j = 0; j < n; j++) {
                printf("%.1f ", data[i][j]);
            }
            std::cout << "]\n";
        }
    }
};

template <typename Lhs, typename Rhs>
class MatrixAdd {
private:
    const Lhs& lhs;
    const Rhs& rhs;

public:
    MatrixAdd(const Lhs& l, const Rhs& r) : lhs(l), rhs(r) {}

    double operator()(std::size_t i, std::size_t j) const {
        return lhs(i, j) + rhs(i, j);
    }

    std::size_t rows() const { return lhs.rows(); }
    std::size_t cols() const { return lhs.cols(); }
};

auto operator+(const Matrix& lhs, const Matrix& rhs) {
    return MatrixExpr<MatrixAdd<Matrix, Matrix>>(
        MatrixAdd<Matrix, Matrix>(lhs, rhs));
}

int main() {
    std::cout << "=== 수학 문제: 행렬식 계산 게임 ===\n\n";

    srand(static_cast<unsigned>(time(nullptr)));

    int score = 0;
    int questions = 0;

    for (int q = 0; q < 3; q++) {
        questions++;

        int a = 1 + rand() % 5;
        int b = 1 + rand() % 5;
        int c = 1 + rand() % 5;
        int d = 1 + rand() % 5;

        Matrix m1(2, 2);
        Matrix m2(2, 2);

        m1(0, 0) = a;
        m1(0, 1) = b;
        m1(1, 0) = c;
        m1(1, 1) = d;

        int e = 1 + rand() % 3;
        int f = 1 + rand() % 3;
        int g = 1 + rand() % 3;
        int h = 1 + rand() % 3;

        m2(0, 0) = e;
        m2(0, 1) = f;
        m2(1, 0) = g;
        m2(1, 1) = h;

        std::cout << "문제 " << q + 1 << ": 다음 행렬의 합을 구하시오.\n";
        std::cout << "행렬 A:\n";
        m1.display();

        std::cout << "행렬 B:\n";
        m2.display();

        std::cout << "A + B의 (0,0) 원소는?\n";
        std::cout << "1) " << (a + e) << "\n";
        std::cout << "2) " << (a + f) << "\n";
        std::cout << "3) " << (b + e) << "\n";

        int choice;
        std::cout << "선택 (1/2/3): ";
        std::cin >> choice;

        if (choice == 1) {
            std::cout << "✓ 정답!\n\n";
            score++;
        } else {
            std::cout << "✗ 오답. 정답은 " << (a + e) << "입니다.\n\n";
        }
    }

    std::cout << "=== 게임 종료 ===\n";
    std::cout << "점수: " << score << " / " << questions << "\n";

    if (score == questions) {
        std::cout << "완벽한 점수!\n";
    } else if (score >= questions / 2) {
        std::cout << "합격!\n";
    } else {
        std::cout << "더 열심히 공부하세요.\n";
    }

    return 0;
}
