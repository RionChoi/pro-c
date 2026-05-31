// WebAssembly 숫자 맞히기 게임 — emscripten_set_main_loop 패턴
//
// 브라우저에서 JavaScript 연동 예:
//   Module._game_init();
//   // 사용자 입력 후:
//   const hint = Module._game_guess(guess);
//   // hint: 0=정답, -1=낮음, 1=높음, -2=게임종료
//
// Emscripten 빌드:
//   emcc -std=c++23 game1.cpp -o game.html
//        -s EXPORTED_FUNCTIONS='["_game_init","_game_guess","_game_remaining",
//                                "_game_secret","_game_is_playing","_game_is_won",
//                                "_game_attempts"]'
//        -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]'
// 네이티브 테스트:
//   g++ -std=c++23 -Wall -Wextra -Wpedantic game1.cpp -o game1

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define WASM_EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define WASM_EXPORT
#endif

#include <cstdlib>
#include <ctime>
#include <iostream>

// ─── 게임 상태 (WebAssembly 선형 메모리에 상주) ───────────────────────────────

namespace {
    constexpr int MAX_ATTEMPTS = 7;
    constexpr int MIN_VAL      = 1;
    constexpr int MAX_VAL      = 100;

    int  g_secret   = 0;
    int  g_attempts = 0;
    bool g_won      = false;
    bool g_playing  = false;
}

// ─── WebAssembly 공개 API ─────────────────────────────────────────────────────

extern "C" {

// 새 게임 시작 — JS: Module._game_init()
WASM_EXPORT void game_init() {
    g_secret   = (rand() % MAX_VAL) + MIN_VAL;
    g_attempts = 0;
    g_won      = false;
    g_playing  = true;
}

// 숫자 추측 — 반환값: 0=정답, -1=낮음, 1=높음, -2=게임종료
// JS: const hint = Module._game_guess(guess);
WASM_EXPORT int game_guess(int guess) {
    if (!g_playing) return -2;
    ++g_attempts;
    if (guess == g_secret) {
        g_won     = true;
        g_playing = false;
        return 0;
    }
    if (g_attempts >= MAX_ATTEMPTS) {
        g_playing = false;
        return -2;
    }
    return (guess < g_secret) ? -1 : 1;
}

WASM_EXPORT int game_remaining()  { return MAX_ATTEMPTS - g_attempts; }
WASM_EXPORT int game_attempts()   { return g_attempts; }
WASM_EXPORT int game_is_playing() { return g_playing ? 1 : 0; }
WASM_EXPORT int game_is_won()     { return g_won     ? 1 : 0; }

// 비밀 번호는 게임 종료 후에만 공개
WASM_EXPORT int game_secret() { return g_playing ? -1 : g_secret; }

} // extern "C"

// ─── 네이티브 인터랙티브 루프 ─────────────────────────────────────────────────
// 브라우저에서는 이 루프가 emscripten_set_main_loop(step_fn, 0, 1)으로 대체된다.
// step_fn은 한 프레임씩 처리하고 즉시 반환하여 브라우저가 렌더링을 계속할 수 있게 한다.

#ifndef __EMSCRIPTEN__
int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    std::cout << "=== WebAssembly 숫자 맞히기 게임 ===\n";
    std::cout << MIN_VAL << "~" << MAX_VAL << " 사이의 숫자를 "
              << MAX_ATTEMPTS << "번 안에 맞추세요.\n";
    std::cout << "(브라우저 빌드에서는 emscripten_set_main_loop으로 루프를 대체합니다)\n\n";

    game_init();

    bool running = true;
    while (running) {
        std::cout << "[남은 기회: " << game_remaining() << "] 추측: ";

        int guess;
        if (!(std::cin >> guess)) break;

        switch (game_guess(guess)) {
            case 0:
                std::cout << "정답! " << game_attempts()
                          << "번 만에 맞췄습니다!\n";
                running = false;
                break;
            case -1:
                std::cout << "더 높은 숫자입니다.\n";
                break;
            case 1:
                std::cout << "더 낮은 숫자입니다.\n";
                break;
            case -2:
                std::cout << "기회 소진! 정답은 "
                          << game_secret() << " 이었습니다.\n";
                running = false;
                break;
            default:
                break;
        }
    }

    return 0;
}
#endif
