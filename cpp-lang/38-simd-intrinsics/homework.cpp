#include <iostream>
#include <vector>
#include <chrono>
#include <immintrin.h>

// Scalar dot product
float dot_scalar(const float* a, const float* b, int n) {
    float result = 0.0f;
    for (int i = 0; i < n; ++i) {
        result += a[i] * b[i];
    }
    return result;
}

// SSE dot product (process 4 floats at a time)
float dot_sse(const float* a, const float* b, int n) {
    __m128 sum = _mm_setzero_ps();

    int i = 0;
    for (; i + 3 < n; i += 4) {
        __m128 va = _mm_loadu_ps(&a[i]);
        __m128 vb = _mm_loadu_ps(&b[i]);
        __m128 prod = _mm_mul_ps(va, vb);
        sum = _mm_add_ps(sum, prod);
    }

    // Horizontal sum: sum all elements in __m128
    float temp[4];
    _mm_storeu_ps(temp, sum);
    float result = temp[0] + temp[1] + temp[2] + temp[3];

    // Remaining elements (if n is not divisible by 4)
    for (; i < n; ++i) {
        result += a[i] * b[i];
    }

    return result;
}

int main() {
    std::cout << "=== 벡터 내적 (Dot Product) ===" << std::endl << std::endl;

    const int n = 10000;
    std::vector<float> a(n), b(n);

    // Initialize with sample values
    for (int i = 0; i < n; ++i) {
        a[i] = 1.0f + 0.01f * i;
        b[i] = 2.0f - 0.01f * i;
    }

    // Warm-up
    volatile float dummy = dot_scalar(a.data(), b.data(), n);

    // Scalar version
    auto start = std::chrono::high_resolution_clock::now();
    for (int iter = 0; iter < 10000; ++iter) {
        volatile float res = dot_scalar(a.data(), b.data(), n);
        (void)res;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto scalar_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    float scalar_result = dot_scalar(a.data(), b.data(), n);

    // SSE version
    start = std::chrono::high_resolution_clock::now();
    for (int iter = 0; iter < 10000; ++iter) {
        volatile float res = dot_sse(a.data(), b.data(), n);
        (void)res;
    }
    end = std::chrono::high_resolution_clock::now();
    auto sse_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    float sse_result = dot_sse(a.data(), b.data(), n);

    std::cout << "벡터 크기: " << n << std::endl;
    std::cout << "반복: 10,000회" << std::endl << std::endl;

    std::cout << "결과:" << std::endl;
    std::cout << "  스칼라: " << scalar_result << std::endl;
    std::cout << "  SSE:    " << sse_result << std::endl << std::endl;

    std::cout << "실행 시간:" << std::endl;
    std::cout << "  스칼라: " << scalar_time << " μs" << std::endl;
    std::cout << "  SSE:    " << sse_time << " μs" << std::endl << std::endl;

    double speedup = static_cast<double>(scalar_time) / sse_time;
    std::cout << "성능 개선: " << speedup << "x" << std::endl;

    return 0;
}
