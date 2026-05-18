// 세션 25 과제 2: 캐시 미스 비교 벤치마크 + False Sharing 시연
// 목표: 행/열 접근 패턴의 성능 차이를 측정하고, false sharing 문제를 이해한다.

#include <atomic>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

using Clock = std::chrono::high_resolution_clock;
using Ms    = std::chrono::duration<double, std::milli>;

// ── 1. 행(row) vs 열(column) 접근 벤치마크 ────────────────────────────────

static constexpr int MAT_N = 1024;

static long long bench_row_major(const std::vector<int>& mat) {
    long long sum = 0;
    for (int i = 0; i < MAT_N; ++i)
        for (int j = 0; j < MAT_N; ++j)
            sum += mat[static_cast<std::size_t>(i * MAT_N + j)];
    return sum;
}

static long long bench_col_major(const std::vector<int>& mat) {
    long long sum = 0;
    for (int j = 0; j < MAT_N; ++j)
        for (int i = 0; i < MAT_N; ++i)
            sum += mat[static_cast<std::size_t>(i * MAT_N + j)];
    return sum;
}

static void benchmark_access_patterns() {
    std::cout << "=== 접근 패턴 벤치마크 (" << MAT_N << "x" << MAT_N << " 행렬) ===\n";

    std::vector<int> mat(static_cast<std::size_t>(MAT_N * MAT_N));
    for (int i = 0; i < MAT_N * MAT_N; ++i)
        mat[static_cast<std::size_t>(i)] = i % 100;

    // 워밍업
    (void)bench_row_major(mat);
    (void)bench_col_major(mat);

    // 행 방향 (캐시 친화적)
    auto t0  = Clock::now();
    auto sum_row = bench_row_major(mat);
    auto t1  = Clock::now();

    // 열 방향 (캐시 비친화적)
    auto t2  = Clock::now();
    auto sum_col = bench_col_major(mat);
    auto t3  = Clock::now();

    double row_ms = Ms(t1 - t0).count();
    double col_ms = Ms(t3 - t2).count();

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "행 방향 합산: " << sum_row << "  시간: " << row_ms << " ms\n";
    std::cout << "열 방향 합산: " << sum_col << "  시간: " << col_ms << " ms\n";
    std::cout << "속도비 (열/행): " << (col_ms / row_ms) << "x\n";
}

// ── 2. False Sharing 시연 ──────────────────────────────────────────────────

struct alignas(1) PackedCounters {
    std::atomic<int> a{0};
    std::atomic<int> b{0};
    // a와 b가 같은 캐시 라인에 위치할 가능성이 높음
};

struct PaddedCounterA {
    alignas(64) std::atomic<int> value{0};
};

struct PaddedCounterB {
    alignas(64) std::atomic<int> value{0};
};

static constexpr int ITER = 10'000'000;

static double bench_shared(PackedCounters& c) {
    auto t0 = Clock::now();
    std::thread t1([&c]() {
        for (int i = 0; i < ITER; ++i)
            c.a.fetch_add(1, std::memory_order_relaxed);
    });
    std::thread t2([&c]() {
        for (int i = 0; i < ITER; ++i)
            c.b.fetch_add(1, std::memory_order_relaxed);
    });
    t1.join();
    t2.join();
    return Ms(Clock::now() - t0).count();
}

static double bench_padded(PaddedCounterA& ca, PaddedCounterB& cb) {
    auto t0 = Clock::now();
    std::thread t1([&ca]() {
        for (int i = 0; i < ITER; ++i)
            ca.value.fetch_add(1, std::memory_order_relaxed);
    });
    std::thread t2([&cb]() {
        for (int i = 0; i < ITER; ++i)
            cb.value.fetch_add(1, std::memory_order_relaxed);
    });
    t1.join();
    t2.join();
    return Ms(Clock::now() - t0).count();
}

static void benchmark_false_sharing() {
    std::cout << "\n=== False Sharing 벤치마크 ===\n";

    PackedCounters  shared;
    PaddedCounterA  pa;
    PaddedCounterB  pb;

    // 워밍업
    {
        PackedCounters  ws;
        PaddedCounterA  wpa;
        PaddedCounterB  wpb;
        bench_shared(ws);
        bench_padded(wpa, wpb);
    }

    double shared_ms = bench_shared(shared);
    double padded_ms = bench_padded(pa, pb);

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "PackedCounters  (false sharing 가능): " << shared_ms << " ms\n";
    std::cout << "PaddedCounters  (캐시 라인 격리):      " << padded_ms << " ms\n";

    if (padded_ms < shared_ms)
        std::cout << "캐시 라인 패딩으로 " << (shared_ms / padded_ms) << "x 빠름\n";
    else
        std::cout << "참고: 이 환경에서는 false sharing 효과가 미미할 수 있습니다.\n";
}

// ── 3. Structure of Arrays (SoA) vs Array of Structures (AoS) ─────────────

struct AoS_Particle {
    float x, y, z;   // 위치
    float life;       // 수명
    int   active;     // 활성 여부
};

struct SoA_Particles {
    std::vector<float> x, y, z;
    std::vector<float> life;
    std::vector<int>   active;
};

static constexpr int P_COUNT = 100'000;

static void benchmark_soa_vs_aos() {
    std::cout << "\n=== AoS vs SoA 벤치마크 (" << P_COUNT << "개 파티클) ===\n";

    // AoS 초기화
    std::vector<AoS_Particle> aos(P_COUNT);
    for (int i = 0; i < P_COUNT; ++i)
        aos[static_cast<std::size_t>(i)] = {static_cast<float>(i), 0, 0, 1.0f, 1};

    // SoA 초기화
    SoA_Particles soa;
    soa.x.resize(P_COUNT);
    soa.y.resize(P_COUNT, 0.0f);
    soa.z.resize(P_COUNT, 0.0f);
    soa.life.resize(P_COUNT, 1.0f);
    soa.active.resize(P_COUNT, 1);
    for (int i = 0; i < P_COUNT; ++i)
        soa.x[static_cast<std::size_t>(i)] = static_cast<float>(i);

    // AoS: x 좌표만 합산 (non-contiguous access)
    auto t0 = Clock::now();
    double aos_sum = 0.0;
    for (int i = 0; i < P_COUNT; ++i)
        aos_sum += aos[static_cast<std::size_t>(i)].x;
    double aos_ms = Ms(Clock::now() - t0).count();

    // SoA: x 좌표만 합산 (contiguous access)
    auto t1 = Clock::now();
    double soa_sum = 0.0;
    for (int i = 0; i < P_COUNT; ++i)
        soa_sum += soa.x[static_cast<std::size_t>(i)];
    double soa_ms = Ms(Clock::now() - t1).count();

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "AoS x-sum: " << aos_sum << "  시간: " << aos_ms << " ms\n";
    std::cout << "SoA x-sum: " << soa_sum << "  시간: " << soa_ms << " ms\n";
    std::cout << "참고: SoA는 특정 필드만 처리할 때 캐시 효율이 높습니다.\n";
}

int main() {
    benchmark_access_patterns();
    benchmark_false_sharing();
    benchmark_soa_vs_aos();
    return 0;
}
