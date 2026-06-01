#include <iostream>
#include <immintrin.h>
#include <iomanip>

using namespace std;

int main() {
    cout << "=== SIMD 기본 연산 ===" << endl << endl;

    cout << "1. SSE (128-bit) 정수 덧셈" << endl;
    __m128i a = _mm_set_epi32(10, 20, 30, 40);
    __m128i b = _mm_set_epi32(5, 10, 15, 20);
    __m128i sum = _mm_add_epi32(a, b);

    cout << "   a = [40, 30, 20, 10]" << endl;
    cout << "   b = [20, 15, 10, 5]" << endl;
    cout << "   a + b = ";
    int r1[4];
    _mm_storeu_si128((__m128i*)r1, sum);
    for (int i = 3; i >= 0; --i) {
        cout << r1[i];
        if (i > 0) cout << ", ";
    }
    cout << endl << endl;

    cout << "2. SSE (128-bit) 실수 연산" << endl;
    __m128 x = _mm_set_ps(1.5f, 2.0f, 2.5f, 3.0f);
    __m128 y = _mm_set_ps(0.5f, 1.0f, 1.5f, 2.0f);

    __m128 add_result = _mm_add_ps(x, y);
    __m128 mul_result = _mm_mul_ps(x, y);
    __m128 div_result = _mm_div_ps(x, y);

    cout << "   x = [3.0, 2.5, 2.0, 1.5]" << endl;
    cout << "   y = [2.0, 1.5, 1.0, 0.5]" << endl;

    float* f_result = (float*)&add_result;
    cout << "   x + y = [";
    for (int i = 3; i >= 0; --i) {
        cout << fixed << setprecision(1) << f_result[i];
        if (i > 0) cout << ", ";
    }
    cout << "]" << endl;

    f_result = (float*)&mul_result;
    cout << "   x * y = [";
    for (int i = 3; i >= 0; --i) {
        cout << fixed << setprecision(1) << f_result[i];
        if (i > 0) cout << ", ";
    }
    cout << "]" << endl;

    f_result = (float*)&div_result;
    cout << "   x / y = [";
    for (int i = 3; i >= 0; --i) {
        cout << fixed << setprecision(2) << f_result[i];
        if (i > 0) cout << ", ";
    }
    cout << "]" << endl << endl;

    cout << "3. AVX (256-bit) 실수 덧셈" << endl;
    __m256 u = _mm256_set_ps(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
    __m256 v = _mm256_set_ps(0.5f, 1.0f, 1.5f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f);
    __m256 avx_sum = _mm256_add_ps(u, v);

    cout << "   u = [8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0]" << endl;
    cout << "   v = [4.0, 3.5, 3.0, 2.5, 2.0, 1.5, 1.0, 0.5]" << endl;
    cout << "   u + v = [";

    float* avx_result = (float*)&avx_sum;
    for (int i = 7; i >= 0; --i) {
        cout << fixed << setprecision(1) << avx_result[i];
        if (i > 0) cout << ", ";
    }
    cout << "]" << endl << endl;

    cout << "4. 배열 합계 (순차 vs 벡터화)" << endl;
    int arr[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

    // 순차 합계
    int sequential_sum = 0;
    for (int i = 0; i < 16; ++i) {
        sequential_sum += arr[i];
    }

    // SIMD 합계 (SSE 사용)
    int simd_sum = 0;
    __m128i vec_sum = _mm_setzero_si128();
    for (int i = 0; i < 16; i += 4) {
        __m128i vec = _mm_loadu_si128((__m128i*)(arr + i));
        vec_sum = _mm_add_epi32(vec_sum, vec);
    }

    // 최종 합계 계산 (horizontal sum)
    int tmp[4];
    _mm_storeu_si128((__m128i*)tmp, vec_sum);
    for (int i = 0; i < 4; ++i) {
        simd_sum += tmp[i];
    }

    cout << "   배열: [1..16]" << endl;
    cout << "   순차 합: " << sequential_sum << endl;
    cout << "   SIMD 합: " << simd_sum << endl << endl;

    cout << "5. 절댓값 (SSE)" << endl;
    __m128i neg = _mm_set_epi32(-40, -30, -20, -10);
    __m128i abs_result = _mm_abs_epi32(neg);

    cout << "   neg = [-10, -20, -30, -40]" << endl;
    cout << "   abs(neg) = ";
    int r5[4];
    _mm_storeu_si128((__m128i*)r5, abs_result);
    for (int i = 3; i >= 0; --i) {
        cout << r5[i];
        if (i > 0) cout << ", ";
    }
    cout << endl;

    return 0;
}
