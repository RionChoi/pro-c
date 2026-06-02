#include <iostream>
#include <immintrin.h>
#include <cstring>
#include <chrono>

alignas(16) float vecA[4] = {1.0f, 2.0f, 3.0f, 4.0f};
alignas(16) float vecB[4] = {5.0f, 6.0f, 7.0f, 8.0f};

float dotProductNaive() {
    float result = 0.0f;
    for (int i = 0; i < 4; ++i) {
        result += vecA[i] * vecB[i];
    }
    return result;
}

float dotProductSSE() {
    __m128 a = _mm_load_ps(vecA);
    __m128 b = _mm_load_ps(vecB);
    __m128 mul = _mm_mul_ps(a, b);

    __m128 shuf = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(1, 0, 3, 2));
    __m128 sum1 = _mm_add_ps(mul, shuf);

    shuf = _mm_shuffle_ps(sum1, sum1, _MM_SHUFFLE(2, 3, 0, 1));
    __m128 sum2 = _mm_add_ps(sum1, shuf);

    return _mm_cvtss_f32(sum2);
}

int main() {
    std::cout << "=== SIMD 내적 계산 ===" << std::endl;

    float naive = dotProductNaive();
    float simd = dotProductSSE();

    std::cout << "일반 방식: " << naive << std::endl;
    std::cout << "SSE 방식: " << simd << std::endl;

    if (std::abs(naive - simd) < 0.001f) {
        std::cout << "결과 일치! ✓" << std::endl;
    } else {
        std::cout << "결과 불일치!" << std::endl;
    }

    return 0;
}
