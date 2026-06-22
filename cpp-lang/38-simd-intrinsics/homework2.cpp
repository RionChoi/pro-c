#include <iostream>
#include <vector>
#include <chrono>
#include <immintrin.h>
#include <cmath>

// Scalar matrix-vector multiplication: y = A * x (4x4 matrix)
void matvec_scalar(const float A[4][4], const float x[4], float y[4]) {
    for (int i = 0; i < 4; ++i) {
        y[i] = 0.0f;
        for (int j = 0; j < 4; ++j) {
            y[i] += A[i][j] * x[j];
        }
    }
}

// AVX matrix-vector multiplication (process row as __m256)
void matvec_avx(const float A[4][4], const float x[4], float y[4]) {
    __m256 vx = _mm256_setr_ps(x[0], x[1], x[2], x[3], 0, 0, 0, 0);

    for (int i = 0; i < 4; ++i) {
        __m256 row = _mm256_setr_ps(A[i][0], A[i][1], A[i][2], A[i][3], 0, 0, 0, 0);
        __m256 prod = _mm256_mul_ps(row, vx);

        // Horizontal sum
        float temp[8];
        _mm256_storeu_ps(temp, prod);
        y[i] = temp[0] + temp[1] + temp[2] + temp[3];
    }
}

int main() {
    std::cout << "=== 행렬-벡터 곱셈 (Matrix-Vector Multiplication) ===" << std::endl << std::endl;

    // 4x4 행렬 초기화
    float A[4][4] = {
        {1.0f, 2.0f, 3.0f, 4.0f},
        {5.0f, 6.0f, 7.0f, 8.0f},
        {9.0f, 10.0f, 11.0f, 12.0f},
        {13.0f, 14.0f, 15.0f, 16.0f}
    };

    // 벡터 초기화
    float x[4] = {1.0f, 2.0f, 3.0f, 4.0f};

    float y_scalar[4], y_avx[4];

    // Scalar version
    auto start = std::chrono::high_resolution_clock::now();
    for (int iter = 0; iter < 100000; ++iter) {
        matvec_scalar(A, x, y_scalar);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto scalar_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // AVX version
    start = std::chrono::high_resolution_clock::now();
    for (int iter = 0; iter < 100000; ++iter) {
        matvec_avx(A, x, y_avx);
    }
    end = std::chrono::high_resolution_clock::now();
    auto avx_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    std::cout << "행렬 크기: 4x4" << std::endl;
    std::cout << "반복: 100,000회" << std::endl << std::endl;

    std::cout << "결과 벡터 (스칼라):" << std::endl;
    for (int i = 0; i < 4; ++i) {
        std::cout << "  y[" << i << "] = " << y_scalar[i] << std::endl;
    }
    std::cout << std::endl;

    std::cout << "결과 벡터 (AVX):" << std::endl;
    for (int i = 0; i < 4; ++i) {
        std::cout << "  y[" << i << "] = " << y_avx[i] << std::endl;
    }
    std::cout << std::endl;

    std::cout << "실행 시간:" << std::endl;
    std::cout << "  스칼라: " << scalar_time << " μs" << std::endl;
    std::cout << "  AVX:    " << avx_time << " μs" << std::endl << std::endl;

    double speedup = static_cast<double>(scalar_time) / avx_time;
    std::cout << "성능 개선: " << speedup << "x" << std::endl;

    // Verify correctness
    bool correct = true;
    for (int i = 0; i < 4; ++i) {
        if (std::abs(y_scalar[i] - y_avx[i]) > 1e-5) {
            correct = false;
            break;
        }
    }
    std::cout << "정확도: " << (correct ? "PASS" : "FAIL") << std::endl;

    return correct ? 0 : 1;
}
