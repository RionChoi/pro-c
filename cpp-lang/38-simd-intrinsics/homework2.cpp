#include <iostream>
#include <immintrin.h>
#include <iomanip>
#include <cmath>

using namespace std;

// 벡터 내적 (16개 float)
float dot_product_scalar(const float* a, const float* b, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

// SIMD를 사용한 벡터 내적 (16개 float, SSE)
float dot_product_sse(const float* a, const float* b) {
    __m128 sum1 = _mm_mul_ps(_mm_loadu_ps(a),      _mm_loadu_ps(b));
    __m128 sum2 = _mm_mul_ps(_mm_loadu_ps(a + 4),  _mm_loadu_ps(b + 4));
    __m128 sum3 = _mm_mul_ps(_mm_loadu_ps(a + 8),  _mm_loadu_ps(b + 8));
    __m128 sum4 = _mm_mul_ps(_mm_loadu_ps(a + 12), _mm_loadu_ps(b + 12));

    // Horizontal sum
    __m128 sum = _mm_add_ps(_mm_add_ps(sum1, sum2), _mm_add_ps(sum3, sum4));
    float* result = (float*)&sum;
    return result[0] + result[1] + result[2] + result[3];
}

// SIMD를 사용한 벡터 내적 (16개 float, AVX)
float dot_product_avx(const float* a, const float* b) {
    __m256 sum1 = _mm256_mul_ps(_mm256_loadu_ps(a),     _mm256_loadu_ps(b));
    __m256 sum2 = _mm256_mul_ps(_mm256_loadu_ps(a + 8), _mm256_loadu_ps(b + 8));
    __m256 sum = _mm256_add_ps(sum1, sum2);

    // Horizontal sum
    float* result = (float*)&sum;
    return result[0] + result[1] + result[2] + result[3] +
           result[4] + result[5] + result[6] + result[7];
}

// 벡터 정규화
void normalize_scalar(float* v, int n) {
    float sum_sq = 0.0f;
    for (int i = 0; i < n; ++i) {
        sum_sq += v[i] * v[i];
    }
    float mag = sqrt(sum_sq);
    if (mag > 1e-6f) {
        for (int i = 0; i < n; ++i) {
            v[i] /= mag;
        }
    }
}

// SIMD를 사용한 벡터 정규화
void normalize_simd(float* v) {
    // 내적으로 norm 계산
    __m256 sum = _mm256_mul_ps(_mm256_loadu_ps(v), _mm256_loadu_ps(v));
    float* s = (float*)&sum;
    float sum_sq = s[0] + s[1] + s[2] + s[3] + s[4] + s[5] + s[6] + s[7];
    float mag = sqrt(sum_sq);

    if (mag > 1e-6f) {
        __m256 divisor = _mm256_set1_ps(mag);
        __m256 v1 = _mm256_loadu_ps(v);
        __m256 v2 = _mm256_loadu_ps(v + 8);
        _mm256_storeu_ps(v, _mm256_div_ps(v1, divisor));
        _mm256_storeu_ps(v + 8, _mm256_div_ps(v2, divisor));
    }
}

// 행렬 벡터 곱 (4x4 * 4)
void mat_vec_multiply_scalar(const float mat[4][4], const float vec[4], float result[4]) {
    for (int i = 0; i < 4; ++i) {
        result[i] = 0.0f;
        for (int j = 0; j < 4; ++j) {
            result[i] += mat[i][j] * vec[j];
        }
    }
}

// SIMD 행렬 벡터 곱 (4x4 * 4)
void mat_vec_multiply_simd(const float mat[4][4], const float vec[4], float result[4]) {
    __m128 v = _mm_loadu_ps(vec);

    for (int i = 0; i < 4; ++i) {
        __m128 row = _mm_loadu_ps(mat[i]);
        __m128 mul = _mm_mul_ps(row, v);

        // Horizontal sum
        float* m = (float*)&mul;
        result[i] = m[0] + m[1] + m[2] + m[3];
    }
}

int main() {
    cout << "=== 고급 SIMD 연산 ===" << endl << endl;

    cout << "1. 벡터 내적 비교 (16개 float)" << endl;
    float a[16] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f,
                   9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f};
    float b[16] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                   1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

    float scalar_dot = dot_product_scalar(a, b, 16);
    float sse_dot = dot_product_sse(a, b);
    float avx_dot = dot_product_avx(a, b);

    cout << "   a = [1..16]" << endl;
    cout << "   b = [1..16]" << endl;
    cout << "   내적 (순차):   " << fixed << setprecision(1) << scalar_dot << endl;
    cout << "   내적 (SSE):    " << sse_dot << endl;
    cout << "   내적 (AVX):    " << avx_dot << endl << endl;

    cout << "2. 벡터 정규화 비교" << endl;
    float v1[16] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f,
                    1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float v2[16];
    for (int i = 0; i < 16; ++i) v2[i] = v1[i];

    normalize_scalar(v1, 16);
    normalize_simd(v2);

    cout << "   정규화 후 벡터의 첫 4개 원소:" << endl;
    cout << "   순차: [";
    for (int i = 0; i < 4; ++i) {
        cout << fixed << setprecision(4) << v1[i];
        if (i < 3) cout << ", ";
    }
    cout << "]" << endl;

    cout << "   SIMD: [";
    for (int i = 0; i < 4; ++i) {
        cout << fixed << setprecision(4) << v2[i];
        if (i < 3) cout << ", ";
    }
    cout << "]" << endl << endl;

    cout << "3. 행렬-벡터 곱셈 (4x4 * 4)" << endl;
    float mat[4][4] = {{1, 2, 3, 4},
                       {5, 6, 7, 8},
                       {9, 10, 11, 12},
                       {13, 14, 15, 16}};
    float vec[4] = {1, 2, 3, 4};
    float res_scalar[4], res_simd[4];

    mat_vec_multiply_scalar(mat, vec, res_scalar);
    mat_vec_multiply_simd(mat, vec, res_simd);

    cout << "   행렬: " << endl;
    for (int i = 0; i < 4; ++i) {
        cout << "   [";
        for (int j = 0; j < 4; ++j) {
            cout << fixed << setw(2) << (int)mat[i][j];
            if (j < 3) cout << " ";
        }
        cout << "]" << endl;
    }
    cout << "   벡터: [1, 2, 3, 4]" << endl << endl;

    cout << "   결과 (순차): [";
    for (int i = 0; i < 4; ++i) {
        cout << fixed << setprecision(0) << res_scalar[i];
        if (i < 3) cout << ", ";
    }
    cout << "]" << endl;

    cout << "   결과 (SIMD): [";
    for (int i = 0; i < 4; ++i) {
        cout << fixed << setprecision(0) << res_simd[i];
        if (i < 3) cout << ", ";
    }
    cout << "]" << endl;

    return 0;
}
