// Session 46: 아레나 기반 게임 프레임 시뮬레이터
// 프레임마다 스크래치 아레나를 사용하고 리셋 — 실시간 메모리 재활용 패턴
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <new>
#include <array>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <cstdio>

// ─────────────────────────────────────────────
// LinearArena
// ─────────────────────────────────────────────

constexpr std::size_t align_up(std::size_t n, std::size_t a) noexcept {
    return (n + a - 1) & ~(a - 1);
}

class LinearArena {
public:
    explicit LinearArena(void* buf, std::size_t cap) noexcept
        : base_(static_cast<std::byte*>(buf)), capacity_(cap) {}

    void* alloc(std::size_t size, std::size_t align = alignof(std::max_align_t)) noexcept {
        std::size_t a = align_up(offset_, align);
        if (a + size > capacity_) return nullptr;
        void* p = base_ + a;
        offset_ = a + size;
        ++alloc_count_;
        return p;
    }

    template <typename T, typename... Args>
    T* create(Args&&... args) {
        void* p = alloc(sizeof(T), alignof(T));
        return p ? ::new(p) T(std::forward<Args>(args)...) : nullptr;
    }

    template <typename T>
    T* alloc_array(std::size_t n) {
        void* p = alloc(sizeof(T) * n, alignof(T));
        return static_cast<T*>(p);
    }

    void reset() noexcept { offset_ = 0; alloc_count_ = 0; }

    std::size_t used()        const noexcept { return offset_; }
    std::size_t capacity()    const noexcept { return capacity_; }
    std::size_t alloc_count() const noexcept { return alloc_count_; }

private:
    std::byte*  base_{};
    std::size_t capacity_{};
    std::size_t offset_{};
    std::size_t alloc_count_{};
};

// ─────────────────────────────────────────────
// 게임 엔티티 타입
// ─────────────────────────────────────────────

struct Vec2 { float x, y; };

struct Entity {
    int   id;
    Vec2  pos;
    Vec2  vel;
    float health;
    bool  active;
};

struct Bullet {
    Vec2  pos;
    Vec2  dir;
    float speed;
    int   owner_id;
};

struct FrameStats {
    int         frame;
    int         entity_count;
    int         bullet_count;
    int         collision_count;
    std::size_t scratch_used;
    std::size_t scratch_allocs;
};

// ─────────────────────────────────────────────
// LCG 난수 생성기
// ─────────────────────────────────────────────

struct Rng {
    unsigned state;
    explicit Rng(unsigned seed = 42u) : state(seed) {}
    unsigned next() { return state = state * 1664525u + 1013904223u; }
    float nextf(float lo, float hi) {
        return lo + (hi - lo) * (float(next() & 0xFFFFu) / 65535.0f);
    }
};

// ─────────────────────────────────────────────
// 게임 월드
// ─────────────────────────────────────────────

class GameWorld {
public:
    static constexpr int   MAX_ENTITIES = 32;
    static constexpr int   MAX_BULLETS  = 64;
    static constexpr float MAP_SIZE     = 100.0f;

    explicit GameWorld(LinearArena& persistent) : rng_(12345u) {
        entities_ = persistent.alloc_array<Entity>(MAX_ENTITIES);
        bullets_  = persistent.alloc_array<Bullet>(MAX_BULLETS);
        assert(entities_ && bullets_);

        for (int i = 0; i < 8; ++i) {
            entities_[i] = {
                i,
                {rng_.nextf(10, 90), rng_.nextf(10, 90)},
                {rng_.nextf(-2.0f, 2.0f), rng_.nextf(-2.0f, 2.0f)},
                100.0f,
                true
            };
            entity_count_ = i + 1;
        }
    }

    FrameStats update(int frame, LinearArena& scratch) {
        // 프레임 임시 데이터는 scratch 아레나에서 할당
        int* alive_ids   = scratch.alloc_array<int>(MAX_ENTITIES);
        int  alive_count = 0;

        // 엔티티 이동 + 경계 반사
        for (int i = 0; i < entity_count_; ++i) {
            auto& e = entities_[i];
            if (!e.active) continue;
            e.pos.x += e.vel.x;
            e.pos.y += e.vel.y;
            if (e.pos.x < 0.0f || e.pos.x > MAP_SIZE) e.vel.x = -e.vel.x;
            if (e.pos.y < 0.0f || e.pos.y > MAP_SIZE) e.vel.y = -e.vel.y;
            e.pos.x = std::clamp(e.pos.x, 0.0f, MAP_SIZE);
            e.pos.y = std::clamp(e.pos.y, 0.0f, MAP_SIZE);
            alive_ids[alive_count++] = i;
        }

        // 프레임 발사 이벤트 (scratch)
        struct FireEvent { int entity_id; Vec2 target; };
        auto* events   = scratch.alloc_array<FireEvent>(4);
        int   ev_count = 0;

        if (alive_count >= 2 && bullet_count_ < MAX_BULLETS - 4) {
            for (int k = 0; k < std::min(2, alive_count); ++k) {
                int eid = alive_ids[k];
                int tid = alive_ids[(k + 1) % alive_count];
                events[ev_count++] = {eid, entities_[tid].pos};
            }
        }

        for (int k = 0; k < ev_count; ++k) {
            auto& fe  = events[k];
            auto& src = entities_[fe.entity_id];
            Vec2 dir  = {fe.target.x - src.pos.x, fe.target.y - src.pos.y};
            float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
            if (len > 0.001f) { dir.x /= len; dir.y /= len; }
            bullets_[bullet_count_++] = {src.pos, dir, 5.0f, fe.entity_id};
        }

        // 불릿 이동 + 충돌
        int new_bc = 0, collisions = 0;
        for (int b = 0; b < bullet_count_; ++b) {
            bullets_[b].pos.x += bullets_[b].dir.x * bullets_[b].speed;
            bullets_[b].pos.y += bullets_[b].dir.y * bullets_[b].speed;

            bool hit = (bullets_[b].pos.x < 0 || bullets_[b].pos.x > MAP_SIZE
                     || bullets_[b].pos.y < 0 || bullets_[b].pos.y > MAP_SIZE);

            for (int i = 0; i < entity_count_ && !hit; ++i) {
                if (!entities_[i].active) continue;
                if (entities_[i].id == bullets_[b].owner_id) continue;
                float dx = entities_[i].pos.x - bullets_[b].pos.x;
                float dy = entities_[i].pos.y - bullets_[b].pos.y;
                if (dx*dx + dy*dy < 25.0f) {
                    entities_[i].health -= 10.0f;
                    if (entities_[i].health <= 0.0f) entities_[i].active = false;
                    ++collisions;
                    hit = true;
                }
            }
            if (!hit) bullets_[new_bc++] = bullets_[b];
        }
        bullet_count_ = new_bc;

        std::size_t s_used   = scratch.used();
        std::size_t s_allocs = scratch.alloc_count();

        // 프레임 끝 — 스크래치 아레나 O(1) 리셋
        scratch.reset();

        return {frame, alive_count, bullet_count_, collisions, s_used, s_allocs};
    }

    int alive_count() const {
        int c = 0;
        for (int i = 0; i < entity_count_; ++i) if (entities_[i].active) ++c;
        return c;
    }

private:
    Entity*  entities_{};
    Bullet*  bullets_{};
    int      entity_count_{};
    int      bullet_count_{};
    Rng      rng_;
};

// ─────────────────────────────────────────────
// main
// ─────────────────────────────────────────────

int main() {
    std::cout << "╔══════════════════════════════════════════╗\n";
    std::cout << "║  아레나 기반 게임 프레임 시뮬레이터       ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n\n";

    alignas(std::max_align_t) std::array<std::byte, 8192> persistent_buf{};
    LinearArena persistent_arena(persistent_buf.data(), persistent_buf.size());

    alignas(std::max_align_t) std::array<std::byte, 4096> scratch_buf{};
    LinearArena scratch_arena(scratch_buf.data(), scratch_buf.size());

    GameWorld world(persistent_arena);
    std::cout << "퍼시스턴트 아레나 초기 사용: "
              << persistent_arena.used() << " bytes\n\n";

    std::cout << "=== 게임 루프 (10 프레임) ===\n";
    std::printf("%-6s %-8s %-8s %-10s %-12s %-12s\n",
                "Frame", "Alive", "Bullets", "Hits", "ScrUsed", "ScrAllocs");
    std::cout << std::string(60, '-') << "\n";

    std::size_t total_scratch = 0;
    for (int f = 1; f <= 10; ++f) {
        auto s = world.update(f, scratch_arena);
        total_scratch += s.scratch_used;
        std::printf("  %2d    %5d    %6d      %5d     %7zu       %7zu\n",
                    s.frame, s.entity_count, s.bullet_count,
                    s.collision_count, s.scratch_used, s.scratch_allocs);
    }

    std::cout << "\n=== 메모리 통계 ===\n";
    std::cout << "퍼시스턴트 아레나: " << persistent_arena.used()
              << " / " << persistent_arena.capacity() << " bytes\n";
    std::cout << "스크래치 아레나:   " << scratch_buf.size()
              << " bytes (매 프레임 재사용)\n";
    std::cout << "스크래치 누적 바이트: " << total_scratch << "\n";
    std::cout << "남은 엔티티: " << world.alive_count() << "\n";

    std::cout << "\n=== 핵심 관찰 ===\n";
    std::cout << "  ✅ 스크래치 아레나 reset() = O(1) — 개별 free 없음\n";
    std::cout << "  ✅ 퍼시스턴트 오브젝트는 게임 내내 안전\n";
    std::cout << "  ✅ 힙 할당 0회 — 전체 시뮬레이션이 스택 버퍼 위에서 동작\n";

    std::cout << "\n=== 게임 완료 ===\n";
    return 0;
}
