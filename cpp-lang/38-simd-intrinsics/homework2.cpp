#include <iostream>
#include <immintrin.h>
#include <cstring>

void printMatrix(const float* mat, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::cout << mat[i * cols + j] << " ";
        }
        std::cout << std::endl;
    }
}

void matmul2x2Naive(const float* A, const float* B, float* C) {
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            C[i * 2 + j] = 0.0f;
            for (int k = 0; k < 2; ++k) {
                C[i * 2 + j] += A[i * 2 + k] * B[k * 2 + j];
            }
        }
    }
}

void matmul2x2SIMD(const float* A, const float* B, float* C) {
    __m128 row1_a = _mm_setr_ps(A[0], A[1], A[0], A[1]);
    __m128 row2_a = _mm_setr_ps(A[2], A[3], A[2], A[3]);

    __m128 col12_b = _mm_setr_ps(B[0], B[2], B[1], B[3]);

    __m128 mul1 = _mm_mul_ps(row1_a, col12_b);
    __m128 mul2 = _mm_mul_ps(row2_a, col12_b);

    __m128 sum1 = _mm_hadd_ps(mul1, mul2);

    _mm_storeu_ps(C, sum1);
}

int main() {
    std::cout << "=== 2x2 행렬 곱셈 (SIMD) ===" << std::endl;

    float A[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float B[4] = {5.0f, 6.0f, 7.0f, 8.0f};
    float C_naive[4], C_simd[4];

    std::cout << "행렬 A:" << std::endl;
    printMatrix(A, 2, 2);

    std::cout << "행렬 B:" << std::endl;
    printMatrix(B, 2, 2);

    matmul2x2Naive(A, B, C_naive);
    matmul2x2SIMD(A, B, C_simd);

    std::cout << "결과 (일반):" << std::endl;
    printMatrix(C_naive, 2, 2);

    std::cout << "결과 (SIMD):" << std::endl;
    printMatrix(C_simd, 2, 2);

    bool match = true;
    for (int i = 0; i < 4; ++i) {
        if (std::abs(C_naive[i] - C_simd[i]) > 0.001f) {
            match = false;
            break;
        }
    }

    if (match) {
        std::cout << "결과 일치! ✓" << std::endl;
    } else {
        std::cout << "결과 불일치!" << std::endl;
    }

    return 0;
}
