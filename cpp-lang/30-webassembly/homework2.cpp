// WebAssembly 통계 라이브러리 — 배열 포인터 전달 & 선형 메모리 패턴
//
// JavaScript 사용 예:
//   const n   = 5;
//   const ptr = Module._malloc(n * 8);                    // double = 8바이트
//   const arr = new Float64Array(Module.HEAPF64.buffer, ptr, n);
//   arr.set([4.0, 7.0, 2.0, 9.0, 1.0]);
//   console.log(Module._stats_mean(ptr, n));              // → 4.6
//   console.log(Module._stats_median(ptr, n));            // → 4.0
//   Module._free(ptr);
//
// Emscripten 빌드:
//   emcc -std=c++23 homework2.cpp -o stats.html
//        -s EXPORTED_FUNCTIONS='["_stats_sum","_stats_mean","_stats_min","_stats_max",
//                                "_stats_variance","_stats_stddev","_stats_median",
//                                "_malloc","_free"]'
//        -s EXPORTED_RUNTIME_METHODS='["HEAPF64"]'
// 네이티브 테스트:
//   g++ -std=c++23 -Wall -Wextra -Wpedantic homework2.cpp -o homework2

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define WASM_EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define WASM_EXPORT
#endif

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>

// ─── 통계 함수 API ────────────────────────────────────────────────────────────
// 인터페이스: (const double* arr, int n)
//   arr = WebAssembly 선형 메모리 내 포인터 (JS: HEAPF64.buffer 기반 뷰)
//   n   = 원소 개수

extern "C" {

WASM_EXPORT double stats_sum(const double* arr, int n) {
    double s = 0.0;
    for (int i = 0; i < n; ++i)
        s += arr[i];
    return s;
}

WASM_EXPORT double stats_mean(const double* arr, int n) {
    if (n <= 0) return 0.0;
    return stats_sum(arr, n) / static_cast<double>(n);
}

WASM_EXPORT double stats_min(const double* arr, int n) {
    if (n <= 0) return 0.0;
    double m = arr[0];
    for (int i = 1; i < n; ++i)
        if (arr[i] < m) m = arr[i];
    return m;
}

WASM_EXPORT double stats_max(const double* arr, int n) {
    if (n <= 0) return 0.0;
    double m = arr[0];
    for (int i = 1; i < n; ++i)
        if (arr[i] > m) m = arr[i];
    return m;
}

WASM_EXPORT double stats_variance(const double* arr, int n) {
    if (n <= 1) return 0.0;
    const double mean = stats_mean(arr, n);
    double var = 0.0;
    for (int i = 0; i < n; ++i) {
        const double d = arr[i] - mean;
        var += d * d;
    }
    return var / static_cast<double>(n);
}

WASM_EXPORT double stats_stddev(const double* arr, int n) {
    return std::sqrt(stats_variance(arr, n));
}

// 내부 복사본을 정렬해 중앙값 계산 (원본 배열 불변)
// WASM 선형 메모리 내에서 malloc/free로 임시 버퍼를 관리하는 패턴
WASM_EXPORT double stats_median(const double* arr, int n) {
    if (n <= 0) return 0.0;
    double* copy = static_cast<double*>(
        malloc(static_cast<size_t>(n) * sizeof(double))
    );
    if (!copy) return 0.0;
    for (int i = 0; i < n; ++i)
        copy[i] = arr[i];
    std::sort(copy, copy + n);
    const double result = (n % 2 == 1)
        ? copy[n / 2]
        : (copy[n / 2 - 1] + copy[n / 2]) / 2.0;
    free(copy);
    return result;
}

} // extern "C"

// ─── 네이티브 테스트 드라이버 ─────────────────────────────────────────────────

#ifndef __EMSCRIPTEN__

static void print_stats(const double* arr, int n, const char* label) {
    std::cout << "\n[" << label << "]\n";
    std::cout << "  sum      = " << stats_sum(arr, n)      << "\n";
    std::cout << "  mean     = " << stats_mean(arr, n)     << "\n";
    std::cout << "  min      = " << stats_min(arr, n)      << "\n";
    std::cout << "  max      = " << stats_max(arr, n)      << "\n";
    std::cout << "  variance = " << stats_variance(arr, n) << "\n";
    std::cout << "  stddev   = " << stats_stddev(arr, n)   << "\n";
    std::cout << "  median   = " << stats_median(arr, n)   << "\n";
}

int main() {
    std::cout << "=== WebAssembly 통계 라이브러리 — 네이티브 테스트 ===\n";
    std::cout << "(브라우저: Float64Array + HEAPF64 패턴으로 동일 함수 호출)\n";

    const double sample1[] = {4.0, 7.0, 2.0, 9.0, 1.0, 5.0, 3.0, 8.0, 6.0, 10.0};
    print_stats(sample1, 10, "1~10 균일 샘플");

    const double sample2[] = {100.0, 50.0, 75.0, 25.0, 90.0};
    print_stats(sample2, 5, "100 이하 샘플");

    const double single[] = {42.0};
    print_stats(single, 1, "단일 원소");

    return 0;
}

#endif
