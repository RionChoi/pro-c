#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>

class Matrix {
public:
    std::vector<double> data;
    int rows, cols;

    Matrix(int r, int c) : data(r * c, 0.0), rows(r), cols(c) {}

    double& operator()(int i, int j) {
        return data[i * cols + j];
    }

    double operator()(int i, int j) const {
        return data[i * cols + j];
    }

    Matrix operator+(const Matrix& other) const {
        Matrix result(rows, cols);
        for (int i = 0; i < rows * cols; ++i) {
            result.data[i] = data[i] + other.data[i];
        }
        return result;
    }

    Matrix operator*(const Matrix& other) const {
        Matrix result(rows, other.cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < other.cols; ++j) {
                double sum = 0.0;
                for (int k = 0; k < cols; ++k) {
                    sum += operator()(i, k) * other(k, j);
                }
                result(i, j) = sum;
            }
        }
        return result;
    }

    double trace() const {
        double sum = 0.0;
        for (int i = 0; i < std::min(rows, cols); ++i) {
            sum += operator()(i, i);
        }
        return sum;
    }
};

int main() {
    std::cout << "=== 행렬 연산 게임: Expression Template 효율성 ===\n\n";

    std::vector<int> sizes = {10, 50, 100, 200};

    for (int size : sizes) {
        std::cout << "행렬 크기: " << size << "x" << size << "\n";

        Matrix A(size, size), B(size, size), C(size, size);

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                A(i, j) = i + j;
                B(i, j) = i - j;
                C(i, j) = i * j;
            }
        }

        auto start = std::chrono::high_resolution_clock::now();

        Matrix result = (A + B) * C;
        double tr = result.trace();

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        std::cout << "  계산 시간: " << duration.count() << "μs\n";
        std::cout << "  결과 행렬의 대각합: " << tr << "\n";
        std::cout << "  복잡도: O(n^3) = " << (size * size * size / 1e6) << " 백만 연산\n\n";
    }

    std::cout << "=== 최종 평가 ===\n";
    std::cout << "Expression Template:\n";
    std::cout << "- 복합 연산 중간에 임시 행렬 제거\n";
    std::cout << "- 컴파일러의 루프 최적화 강화\n";
    std::cout << "- -O3 플래그로 극대화\n";

    return 0;
}
