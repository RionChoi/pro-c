# Session 30: WebAssembly (Emscripten)

## 학습 목표

- WebAssembly(WASM)의 역할과 브라우저 실행 모델을 이해한다
- Emscripten으로 C++ 코드를 WASM 모듈로 빌드하는 과정을 익힌다
- `extern "C"` + `EMSCRIPTEN_KEEPALIVE`로 C++ 함수를 JavaScript에 노출한다
- WebAssembly 선형 메모리와 배열 포인터 전달 패턴을 구현한다
- `emscripten_set_main_loop`으로 브라우저 이벤트 루프에 게임을 통합하는 방법을 이해한다

## 핵심 개념

### WebAssembly란?
WebAssembly는 브라우저에서 네이티브 수준의 속도로 실행되는 바이너리 형식이다.
C/C++ → LLVM IR → WASM 바이트코드 → 브라우저 JIT 컴파일 순으로 실행된다.

```
C++ 소스 → emcc (LLVM 기반) → .wasm + .js (glue code) + .html
```

### extern "C" & EMSCRIPTEN_KEEPALIVE
C++ 함수를 JavaScript에서 호출하려면 두 가지가 필요하다.

1. `extern "C"` — 이름 맹글링(name mangling) 방지: `_Z8wasm_addii` 대신 `_wasm_add`로 노출
2. `EMSCRIPTEN_KEEPALIVE` — 링커의 데드 코드 제거(DCE) 방지

```cpp
#include <emscripten.h>

extern "C" {
    EMSCRIPTEN_KEEPALIVE int wasm_add(int a, int b) { return a + b; }
}
```

JavaScript에서 호출:
```js
// 직접 호출
const result = Module._wasm_add(10, 3);  // → 13

// ccall / cwrap 사용
const r = Module.ccall('wasm_add', 'number', ['number', 'number'], [10, 3]);
const add = Module.cwrap('wasm_add', 'number', ['number', 'number']);
console.log(add(5, 7));  // → 12
```

### 선형 메모리 모델
WebAssembly는 단일 연속 메모리 공간(선형 메모리)을 사용한다.
C++ `malloc`/`free`가 이 메모리를 관리하며, JavaScript는 `TypedArray`로 접근한다.

```js
// JS → C++ 배열 전달 (Float64Array, HEAPF64 사용)
const n   = 5;
const ptr = Module._malloc(n * 8);                     // double = 8바이트
const arr = new Float64Array(Module.HEAPF64.buffer, ptr, n);
arr.set([1.0, 2.0, 3.0, 4.0, 5.0]);
const mean = Module._stats_mean(ptr, n);               // → 3.0
Module._free(ptr);

// 문자열: UTF-8 인코딩, UTF8ToString으로 읽기
const namePtr = Module.allocateUTF8("RionChoi");
const msg = Module.UTF8ToString(Module._wasm_greet(namePtr));
Module._free(namePtr);
```

### emscripten_set_main_loop
브라우저는 메인 스레드를 블로킹하는 루프(`while(true)`)를 허용하지 않는다.
대신 매 프레임마다 호출되는 콜백을 등록한다.

```cpp
void game_loop_step() {
    // 한 프레임 처리 — 브라우저가 매 프레임 호출
}

int main() {
    game_init();
    emscripten_set_main_loop(game_loop_step, 0, 1);  // 0 = 브라우저 프레임레이트
    return 0;
}
```

네이티브 코드에서는 동일 로직을 `while` 루프로 실행한다.

## 파일 구성

| 파일 | 내용 |
|------|------|
| `homework.cpp` | 계산기 모듈 — `extern "C"`, `EMSCRIPTEN_KEEPALIVE`, 선형 메모리 |
| `homework2.cpp` | 통계 라이브러리 — 배열 포인터 전달, `HEAPF64` 패턴 |
| `game1.cpp` | 숫자 맞히기 게임 — 상태 머신, `emscripten_set_main_loop` 패턴 |

## 컴파일

```bash
# 네이티브 테스트 (emcc 없이 로직 검증)
g++ -std=c++23 -Wall -Wextra -Wpedantic homework.cpp  -o homework
g++ -std=c++23 -Wall -Wextra -Wpedantic homework2.cpp -o homework2
g++ -std=c++23 -Wall -Wextra -Wpedantic game1.cpp     -o game1

# WebAssembly 빌드 (Emscripten SDK 필요: https://emscripten.org)
emcc -std=c++23 homework.cpp -o calculator.html \
     -s EXPORTED_FUNCTIONS='["_wasm_add","_wasm_sub","_wasm_mul","_wasm_div",
                             "_wasm_factorial","_wasm_fibonacci",
                             "_wasm_greet","_wasm_alloc","_wasm_free"]' \
     -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","UTF8ToString","allocateUTF8"]'

emcc -std=c++23 homework2.cpp -o stats.html \
     -s EXPORTED_FUNCTIONS='["_stats_sum","_stats_mean","_stats_min","_stats_max",
                             "_stats_variance","_stats_stddev","_stats_median",
                             "_malloc","_free"]' \
     -s EXPORTED_RUNTIME_METHODS='["HEAPF64"]'

emcc -std=c++23 game1.cpp -o game.html \
     -s EXPORTED_FUNCTIONS='["_game_init","_game_guess","_game_remaining",
                             "_game_secret","_game_is_playing","_game_is_won",
                             "_game_attempts"]' \
     -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]'
```

## 참고

- Emscripten 공식 문서: https://emscripten.org/docs/
- MDN WebAssembly: WebAssembly JavaScript Interface
- Bro Code: C++ Full Course (WebAssembly 응용)
