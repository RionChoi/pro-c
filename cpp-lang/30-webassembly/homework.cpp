// WebAssembly 계산기 모듈 — Emscripten extern "C" / keepalive 패턴
//
// Emscripten 빌드:
//   emcc -std=c++23 homework.cpp -o calculator.html
//        -s EXPORTED_FUNCTIONS='["_wasm_add","_wasm_sub","_wasm_mul","_wasm_div",
//                                "_wasm_factorial","_wasm_fibonacci",
//                                "_wasm_greet","_wasm_alloc","_wasm_free"]'
//        -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","UTF8ToString","allocateUTF8"]'
// 네이티브 테스트:
//   g++ -std=c++23 -Wall -Wextra -Wpedantic homework.cpp -o homework

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define WASM_EXPORT EMSCRIPTEN_KEEPALIVE
#else
// 네이티브 빌드에서는 WASM_EXPORT를 빈 매크로로 정의
#define WASM_EXPORT
#endif

#include <cstdio>
#include <cstdlib>
#include <iostream>

// ─── JavaScript에 노출될 API ──────────────────────────────────────────────────
// extern "C": C 연결(linkage)로 이름 맹글링 방지
//   C++: _Z8wasm_addii  →  C: _wasm_add  (JS에서 Module._wasm_add 로 호출)

extern "C" {

WASM_EXPORT int wasm_add(int a, int b) { return a + b; }
WASM_EXPORT int wasm_sub(int a, int b) { return a - b; }
WASM_EXPORT int wasm_mul(int a, int b) { return a * b; }

WASM_EXPORT double wasm_div(double a, double b) {
    return (b != 0.0) ? a / b : 0.0;
}

WASM_EXPORT long long wasm_factorial(int n) {
    if (n <= 1) return 1LL;
    long long r = 1LL;
    for (int i = 2; i <= n; ++i)
        r *= static_cast<long long>(i);
    return r;
}

WASM_EXPORT int wasm_fibonacci(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    int a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        int c = a + b;
        a = b;
        b = c;
    }
    return b;
}

// 정적 버퍼 반환 — JS에서 UTF8ToString(ptr)으로 읽는다
WASM_EXPORT const char* wasm_greet(const char* name) {
    static char buf[256];
    snprintf(buf, sizeof(buf), "안녕하세요, %s! WebAssembly에서 보냅니다.", name);
    return buf;
}

// 선형 메모리 수동 관리
// JS: const ptr = Module._wasm_alloc(n);
//     new Uint8Array(Module.HEAPU8.buffer, ptr, n) 로 뷰 생성
WASM_EXPORT char* wasm_alloc(int size) {
    return static_cast<char*>(malloc(static_cast<size_t>(size)));
}

WASM_EXPORT void wasm_free(char* ptr) {
    free(ptr);
}

} // extern "C"

// ─── 네이티브 테스트 드라이버 ─────────────────────────────────────────────────
// Emscripten 빌드 시 main()은 JS가 담당하므로 조건부로 제외

#ifndef __EMSCRIPTEN__
int main() {
    std::cout << "=== WebAssembly 계산기 모듈 — 네이티브 테스트 ===\n\n";

    std::cout << "기본 연산 (JavaScript 호출 시뮬레이션):\n";
    std::cout << "  wasm_add(10, 3)      = " << wasm_add(10, 3)     << "\n";
    std::cout << "  wasm_sub(10, 3)      = " << wasm_sub(10, 3)     << "\n";
    std::cout << "  wasm_mul(10, 3)      = " << wasm_mul(10, 3)     << "\n";
    std::cout << "  wasm_div(10.0, 3.0)  = " << wasm_div(10.0, 3.0) << "\n";
    std::cout << "  wasm_div(10.0, 0.0)  = " << wasm_div(10.0, 0.0)
              << "  (0 나누기 방어)\n\n";

    std::cout << "팩토리얼:\n";
    for (int i = 0; i <= 10; ++i)
        std::cout << "  " << i << "! = " << wasm_factorial(i) << "\n";

    std::cout << "\n피보나치 F(0..10): ";
    for (int i = 0; i <= 10; ++i)
        std::cout << wasm_fibonacci(i) << " ";
    std::cout << "\n";

    std::cout << "\n문자열 반환:\n";
    std::cout << "  wasm_greet(\"RionChoi\") = " << wasm_greet("RionChoi") << "\n";

    std::cout << "\n선형 메모리 관리:\n";
    char* buf = wasm_alloc(64);
    if (buf) {
        snprintf(buf, 64, "WASM 선형 메모리 테스트 버퍼");
        std::cout << "  wasm_alloc(64) 후 쓰기: " << buf << "\n";
        wasm_free(buf);
        std::cout << "  wasm_free 완료\n";
    }

    std::cout << "\n※ emcc로 빌드하면 브라우저 JS에서 동일 API를 호출할 수 있습니다.\n";
    return 0;
}
#endif
