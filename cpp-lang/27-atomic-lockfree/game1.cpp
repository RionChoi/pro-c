// 세션 27 미니 게임: 멀티스레드 반응속도 & 원자 카운터 배틀
//
// 규칙:
//  - N명의 플레이어(스레드)가 동시에 대기한다.
//  - 심판 스레드가 랜덤 딜레이 후 atomic_flag로 스타트 신호를 보낸다.
//  - 각 플레이어는 신호를 감지하는 즉시 원자 카운터를 최대한 많이 증가시킨다.
//  - 제한 시간(ROUND_MS) 후 심판이 게임을 종료한다.
//  - 각 플레이어의 기여도(increment 횟수)를 출력하고 1등을 발표한다.
//
// 핵심 원자 연산:
//  - std::atomic_flag  : 스타트/스톱 신호
//  - std::atomic<int>  : 공유 글로벌 카운터
//  - std::atomic<long> : 플레이어별 점수 (개인 집계 후 merge)

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

constexpr int  PLAYERS   = 6;      // 플레이어 수
constexpr int  ROUND_MS  = 500;    // 라운드 시간 (밀리초)
constexpr int  ROUNDS    = 3;      // 라운드 수
constexpr int  MAX_WAIT  = 2000;   // 스타트 신호 최대 딜레이 (ms)

// ── 공유 상태 ──────────────────────────────────────────────────────────────

struct GameState {
    std::atomic<bool>       started{false};   // 라운드 시작 신호
    std::atomic<bool>       finished{false};  // 라운드 종료 신호
    std::atomic<long long>  global_counter{0};
    std::vector<std::atomic<long long>> scores; // 플레이어별 점수

    GameState() : scores(PLAYERS) {
        for (auto& s : scores) s.store(0, std::memory_order_relaxed);
    }

    void reset() {
        started.store(false,  std::memory_order_seq_cst);
        finished.store(false, std::memory_order_seq_cst);
        global_counter.store(0, std::memory_order_relaxed);
        for (auto& s : scores) s.store(0, std::memory_order_relaxed);
    }
};

// ── 플레이어 스레드 ───────────────────────────────────────────────────────

void player_thread(GameState& state, int id) {
    // 스타트 신호 대기 (busy-wait — acquire 보장)
    while (!state.started.load(std::memory_order_acquire))
        std::this_thread::yield();

    long long local = 0;
    // 종료 신호가 올 때까지 글로벌 카운터를 최대한 빠르게 증가
    while (!state.finished.load(std::memory_order_acquire)) {
        state.global_counter.fetch_add(1, std::memory_order_relaxed);
        ++local;
    }
    // 개인 점수 기록 (relaxed: 이 시점에서 배리어 불필요)
    state.scores[id].store(local, std::memory_order_relaxed);
}

// ── 심판 스레드 ───────────────────────────────────────────────────────────

void referee(GameState& state, int round) {
    // 랜덤 딜레이 후 스타트
    int wait_ms = 200 + std::rand() % MAX_WAIT;
    std::cout << "  [라운드 " << round + 1 << "] "
              << wait_ms << "ms 후 출발...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));

    state.started.store(true, std::memory_order_release);
    auto t0 = std::chrono::steady_clock::now();

    std::this_thread::sleep_for(std::chrono::milliseconds(ROUND_MS));

    state.finished.store(true, std::memory_order_release);
    auto t1 = std::chrono::steady_clock::now();

    long long actual_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    std::cout << "  라운드 종료 (실제 " << actual_ms << "ms)\n";
}

// ── 메인 ─────────────────────────────────────────────────────────────────

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::cout << "============================================\n";
    std::cout << "  멀티스레드 반응속도 & 카운터 배틀\n";
    std::cout << "  플레이어: " << PLAYERS << "명  |  라운드: " << ROUNDS
              << "개  |  라운드 시간: " << ROUND_MS << "ms\n";
    std::cout << "============================================\n\n";

    std::vector<long long> total_scores(PLAYERS, 0);

    for (int r = 0; r < ROUNDS; ++r) {
        GameState state;

        // 플레이어 스레드 시작
        std::vector<std::thread> players;
        for (int i = 0; i < PLAYERS; ++i)
            players.emplace_back(player_thread, std::ref(state), i);

        // 심판
        referee(state, r);

        for (auto& t : players) t.join();

        // 결과 출력
        long long round_total = state.global_counter.load(std::memory_order_relaxed);
        std::cout << "  글로벌 카운터: " << round_total << "\n";

        int    best_id    = 0;
        long long best_score = 0;
        for (int i = 0; i < PLAYERS; ++i) {
            long long sc = state.scores[i].load(std::memory_order_relaxed);
            total_scores[i] += sc;
            double pct = round_total > 0 ? 100.0 * sc / round_total : 0.0;
            std::cout << "  플레이어 " << (i + 1) << ": "
                      << std::setw(10) << sc << "회  ("
                      << std::fixed << std::setprecision(1) << pct << "%)\n";
            if (sc > best_score) { best_score = sc; best_id = i; }
        }
        std::cout << "  이번 라운드 1등: 플레이어 " << (best_id + 1) << " ★\n\n";
    }

    // 총점 집계
    std::cout << "============================================\n";
    std::cout << "  최종 결과\n";
    std::cout << "============================================\n";

    int    champ_id    = 0;
    long long champ_score = 0;
    for (int i = 0; i < PLAYERS; ++i) {
        std::cout << "  플레이어 " << (i + 1) << " 누적: "
                  << std::setw(12) << total_scores[i] << "회\n";
        if (total_scores[i] > champ_score) {
            champ_score = total_scores[i];
            champ_id    = i;
        }
    }
    std::cout << "\n  🏆 챔피언: 플레이어 " << (champ_id + 1)
              << "  (" << champ_score << "회)\n";
    std::cout << "============================================\n";

    return 0;
}
