#include <iostream>
#include <immintrin.h>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <omp.h>

using namespace std;

const int ARRAY_SIZE = 1000000;
const int ITERATIONS = 100;

// 배열 합계 (순차)
long long sum_sequential(const int* arr, int size) {
    long long sum = 0;
    for (int i = 0; i < size; ++i) {
        sum += arr[i];
    }
    return sum;
}

// 배열 합계 (OpenMP)
long long sum_openmp(const int* arr, int size) {
    long long sum = 0;
#pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < size; ++i) {
        sum += arr[i];
    }
    return sum;
}

// 배열 합계 (SSE)
long long sum_sse(const int* arr, int size) {
    __m128i acc = _mm_set_epi32(0, 0, 0, 0);
    int i = 0;

    // SIMD 처리
    for (; i + 4 <= size; i += 4) {
        __m128i vec = _mm_loadu_si128((__m128i*)(arr + i));
        acc = _mm_add_epi32(acc, vec);
    }

    // 최종 합계
    long long sum = 0;
    int* result = (int*)&acc;
    for (int j = 0; j < 4; ++j) {
        sum += result[j];
    }

    // 나머지 처리
    for (; i < size; ++i) {
        sum += arr[i];
    }

    return sum;
}

// 배열 합계 (AVX)
long long sum_avx(const int* arr, int size) {
    __m256i acc = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0, 0);
    int i = 0;

    for (; i + 8 <= size; i += 8) {
        __m256i vec = _mm256_loadu_si256((__m256i*)(arr + i));
        acc = _mm256_add_epi32(acc, vec);
    }

    long long sum = 0;
    int* result = (int*)&acc;
    for (int j = 0; j < 8; ++j) {
        sum += result[j];
    }

    for (; i < size; ++i) {
        sum += arr[i];
    }

    return sum;
}

// 벡터 곱셈 (순차)
void multiply_sequential(const float* a, const float* b, float* c, int size) {
    for (int i = 0; i < size; ++i) {
        c[i] = a[i] * b[i];
    }
}

// 벡터 곱셈 (SSE)
void multiply_sse(const float* a, const float* b, float* c, int size) {
    int i = 0;
    for (; i + 4 <= size; i += 4) {
        __m128 va = _mm_loadu_ps(a + i);
        __m128 vb = _mm_loadu_ps(b + i);
        __m128 vc = _mm_mul_ps(va, vb);
        _mm_storeu_ps(c + i, vc);
    }

    for (; i < size; ++i) {
        c[i] = a[i] * b[i];
    }
}

// 벡터 곱셈 (AVX)
void multiply_avx(const float* a, const float* b, float* c, int size) {
    int i = 0;
    for (; i + 8 <= size; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vc = _mm256_mul_ps(va, vb);
        _mm256_storeu_ps(c + i, vc);
    }

    for (; i < size; ++i) {
        c[i] = a[i] * b[i];
    }
}

int main() {
    srand(time(0));
    cout << "=== SIMD 성능 비교 게임 ===" << endl << endl;

    // 배열 생성
    int* arr = new int[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        arr[i] = rand() % 100;
    }

    float* a = new float[ARRAY_SIZE];
    float* b = new float[ARRAY_SIZE];
    float* c = new float[ARRAY_SIZE];

    for (int i = 0; i < ARRAY_SIZE; ++i) {
        a[i] = static_cast<float>(rand() % 1000) / 100.0f;
        b[i] = static_cast<float>(rand() % 1000) / 100.0f;
    }

    cout << "배열 크기: " << ARRAY_SIZE << " 요소" << endl;
    cout << "반복 횟수: " << ITERATIONS << endl << endl;

    // 1. 배열 합계 성능 비교
    cout << "[Test 1] 배열 합계" << endl;

    auto start = chrono::high_resolution_clock::now();
    long long res1 = 0;
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        res1 = sum_sequential(arr, ARRAY_SIZE);
    }
    auto end = chrono::high_resolution_clock::now();
    auto seq_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
    cout << "순차: " << seq_time << " μs, 결과: " << res1 << endl;

    start = chrono::high_resolution_clock::now();
    long long res2 = 0;
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        res2 = sum_sse(arr, ARRAY_SIZE);
    }
    end = chrono::high_resolution_clock::now();
    auto sse_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
    cout << "SSE:  " << sse_time << " μs, 결과: " << res2 << endl;

    start = chrono::high_resolution_clock::now();
    long long res3 = 0;
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        res3 = sum_avx(arr, ARRAY_SIZE);
    }
    end = chrono::high_resolution_clock::now();
    auto avx_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
    cout << "AVX:  " << avx_time << " μs, 결과: " << res3 << endl;

    start = chrono::high_resolution_clock::now();
    long long res4 = 0;
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        res4 = sum_openmp(arr, ARRAY_SIZE);
    }
    end = chrono::high_resolution_clock::now();
    auto omp_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
    cout << "OpenMP: " << omp_time << " μs, 결과: " << res4 << endl << endl;

    // 속도 비교
    cout << "속도 향상:" << endl;
    cout << "SSE vs 순차: " << fixed << setprecision(2) << (float)seq_time / sse_time << "배" << endl;
    cout << "AVX vs 순차: " << (float)seq_time / avx_time << "배" << endl;
    cout << "OpenMP vs 순차: " << (float)seq_time / omp_time << "배" << endl << endl;

    // 2. 벡터 곱셈 성능 비교
    cout << "[Test 2] 벡터 원소별 곱셈" << endl;

    start = chrono::high_resolution_clock::now();
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        multiply_sequential(a, b, c, ARRAY_SIZE);
    }
    end = chrono::high_resolution_clock::now();
    seq_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
    cout << "순차: " << seq_time << " μs" << endl;

    start = chrono::high_resolution_clock::now();
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        multiply_sse(a, b, c, ARRAY_SIZE);
    }
    end = chrono::high_resolution_clock::now();
    sse_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
    cout << "SSE:  " << sse_time << " μs" << endl;

    start = chrono::high_resolution_clock::now();
    for (int iter = 0; iter < ITERATIONS; ++iter) {
        multiply_avx(a, b, c, ARRAY_SIZE);
    }
    end = chrono::high_resolution_clock::now();
    avx_time = chrono::duration_cast<chrono::microseconds>(end - start).count();
    cout << "AVX:  " << avx_time << " μs" << endl << endl;

    cout << "속도 향상:" << endl;
    cout << "SSE vs 순차: " << fixed << setprecision(2) << (float)seq_time / sse_time << "배" << endl;
    cout << "AVX vs 순차: " << (float)seq_time / avx_time << "배" << endl << endl;

    cout << "⭐ SIMD는 대규모 배열 연산에서 큰 성능 향상을 제공합니다!" << endl;

    delete[] arr;
    delete[] a;
    delete[] b;
    delete[] c;

    return 0;
}
