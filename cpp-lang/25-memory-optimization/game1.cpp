// 세션 25 게임: 오브젝트 풀 파티클 시뮬레이터
//
// 오브젝트 풀(Object Pool)을 이용해 파티클을 빠르게 생성/소멸하는
// 텍스트 기반 시뮬레이터.
//
// 규칙:
//  - 매 프레임 파티클을 스폰(spawn)하고 위치를 갱신한다.
//  - 수명(life)이 0 이하가 된 파티클은 풀에 반환한다.
//  - 일정 수의 프레임이 지나면 통계를 출력하고 종료한다.

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <vector>

// ── 파티클 정의 ────────────────────────────────────────────────────────────

struct Particle {
    float x, y;         // 위치
    float vx, vy;       // 속도
    float life;         // 남은 수명 (0이면 비활성)
    bool  in_use;       // 풀에서 사용 중 여부

    void spawn(float px, float py, float pvx, float pvy, float plife) {
        x = px;  y = py;
        vx = pvx; vy = pvy;
        life = plife;
        in_use = true;
    }

    void update(float dt) {
        x    += vx * dt;
        y    += vy * dt;
        life -= dt;
    }

    bool alive() const { return life > 0.0f; }
};

// ── 오브젝트 풀 ────────────────────────────────────────────────────────────

template<typename T, std::size_t Capacity>
class ObjectPool {
public:
    ObjectPool() {
        for (std::size_t i = 0; i < Capacity; ++i) {
            pool_[i].in_use = false;
            free_list_[i]   = i;
        }
        free_count_ = Capacity;
    }

    // 풀에서 객체 대여 (없으면 nullptr)
    T* acquire() {
        if (free_count_ == 0) return nullptr;
        std::size_t idx = free_list_[--free_count_];
        pool_[idx].in_use = true;
        ++acquired_total_;
        return &pool_[idx];
    }

    // 객체 반환
    void release(T* obj) {
        assert(obj != nullptr);
        obj->in_use = false;
        std::size_t idx = static_cast<std::size_t>(obj - pool_.data());
        free_list_[free_count_++] = idx;
        ++released_total_;
    }

    std::size_t capacity()      const { return Capacity; }
    std::size_t in_use_count()  const { return Capacity - free_count_; }
    std::size_t acquired_total() const { return acquired_total_; }
    std::size_t released_total() const { return released_total_; }

private:
    std::array<T, Capacity>           pool_;
    std::array<std::size_t, Capacity> free_list_;
    std::size_t free_count_    = 0;
    std::size_t acquired_total_ = 0;
    std::size_t released_total_ = 0;
};

// ── 시뮬레이터 ────────────────────────────────────────────────────────────

static constexpr std::size_t POOL_CAP    = 256;
static constexpr int         TOTAL_FRAMES = 60;
static constexpr float       DT           = 0.016f;  // ~60 FPS
static constexpr int         SPAWN_PER_FRAME = 5;
static constexpr float       WORLD_W      = 80.0f;
static constexpr float       WORLD_H      = 24.0f;

using ParticlePool = ObjectPool<Particle, POOL_CAP>;

static float rand_float(float lo, float hi) {
    return lo + (hi - lo) * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}

static void render_frame(const ParticlePool& pool,
                         const std::vector<Particle*>& active,
                         int frame) {
    // 간단한 ASCII 캔버스
    constexpr int W = 40;
    constexpr int H = 12;
    std::array<std::array<char, W>, H> canvas;
    for (auto& row : canvas) row.fill('.');

    for (const Particle* p : active) {
        int cx = static_cast<int>(p->x / WORLD_W * (W - 1));
        int cy = static_cast<int>(p->y / WORLD_H * (H - 1));
        cx = std::max(0, std::min(W - 1, cx));
        cy = std::max(0, std::min(H - 1, cy));
        canvas[static_cast<std::size_t>(cy)][static_cast<std::size_t>(cx)] = '*';
    }

    std::cout << "\n프레임 " << std::setw(3) << frame
              << " | 활성 파티클: " << std::setw(3) << pool.in_use_count()
              << " / " << pool.capacity() << "\n";
    std::cout << std::string(W + 2, '-') << "\n";
    for (const auto& row : canvas) {
        std::cout << '|';
        for (char c : row) std::cout << c;
        std::cout << "|\n";
    }
    std::cout << std::string(W + 2, '-') << "\n";
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    ParticlePool pool;
    std::vector<Particle*> active;
    active.reserve(POOL_CAP);

    int spawned_attempts = 0;
    int spawn_failures   = 0;

    // 시뮬레이션 루프
    for (int frame = 1; frame <= TOTAL_FRAMES; ++frame) {
        // 스폰
        for (int s = 0; s < SPAWN_PER_FRAME; ++s) {
            ++spawned_attempts;
            Particle* p = pool.acquire();
            if (!p) { ++spawn_failures; continue; }
            p->spawn(
                rand_float(0.0f, WORLD_W),
                rand_float(0.0f, WORLD_H),
                rand_float(-5.0f, 5.0f),
                rand_float(-3.0f, 3.0f),
                rand_float(0.3f, 2.0f));
            active.push_back(p);
        }

        // 갱신 & 회수
        for (std::size_t i = 0; i < active.size(); ) {
            active[i]->update(DT);
            if (!active[i]->alive()) {
                pool.release(active[i]);
                active.erase(active.begin() + static_cast<long>(i));
            } else {
                ++i;
            }
        }

        // 매 10프레임마다 렌더링
        if (frame % 10 == 0)
            render_frame(pool, active, frame);
    }

    // 최종 통계
    std::cout << "\n========== 시뮬레이션 종료 ==========\n";
    std::cout << "총 프레임:          " << TOTAL_FRAMES     << "\n";
    std::cout << "스폰 시도:          " << spawned_attempts  << "\n";
    std::cout << "풀 부족으로 실패:   " << spawn_failures    << "\n";
    std::cout << "총 acquire 횟수:    " << pool.acquired_total() << "\n";
    std::cout << "총 release 횟수:    " << pool.released_total() << "\n";
    std::cout << "최종 활성 파티클:   " << pool.in_use_count() << "\n";
    std::cout << "\n[오브젝트 풀 효과]\n";
    std::cout << "  동적 할당(new) 없이 " << pool.acquired_total()
              << "번 객체를 재사용했습니다.\n";
    std::cout << "  힙 단편화 없이 O(1) 할당/해제를 달성했습니다.\n";
    return 0;
}
