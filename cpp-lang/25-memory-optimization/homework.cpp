// 세션 25 과제 1: 구조체 레이아웃 분석 + 메모리 풀
// 목표: 패딩/정렬을 직접 측정하고 간단한 선형 메모리 풀을 구현한다.

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <memory>
#include <new>
#include <string>

// ── 1. 구조체 레이아웃 ──────────────────────────────────────────────────────

struct Unoptimized {
    char   a;   // 1 byte
    double b;   // 8 bytes  (7 bytes padding before b)
    char   c;   // 1 byte
    int    d;   // 4 bytes  (3 bytes padding before d)
};              // total: 1+7+8+1+3+4 = 24 bytes

struct Optimized {
    double b;   // 8 bytes
    int    d;   // 4 bytes
    char   a;   // 1 byte
    char   c;   // 1 byte
                // 2 bytes tail padding
};              // total: 16 bytes

static void show_layout() {
    std::cout << "=== 구조체 레이아웃 분석 ===\n";
    std::cout << "Unoptimized 크기: " << sizeof(Unoptimized) << " bytes\n";
    std::cout << "  a offset: " << offsetof(Unoptimized, a) << "\n";
    std::cout << "  b offset: " << offsetof(Unoptimized, b) << "\n";
    std::cout << "  c offset: " << offsetof(Unoptimized, c) << "\n";
    std::cout << "  d offset: " << offsetof(Unoptimized, d) << "\n";

    std::cout << "\nOptimized 크기:   " << sizeof(Optimized) << " bytes\n";
    std::cout << "  b offset: " << offsetof(Optimized, b) << "\n";
    std::cout << "  d offset: " << offsetof(Optimized, d) << "\n";
    std::cout << "  a offset: " << offsetof(Optimized, a) << "\n";
    std::cout << "  c offset: " << offsetof(Optimized, c) << "\n";

    const std::size_t saved = sizeof(Unoptimized) - sizeof(Optimized);
    std::cout << "\n멤버 재배치로 절약된 공간: " << saved << " bytes / 객체\n";
}

// ── 2. 선형 메모리 풀 ──────────────────────────────────────────────────────

class LinearPool {
public:
    explicit LinearPool(std::size_t capacity)
        : buffer_(new std::byte[capacity])
        , capacity_(capacity)
        , used_(0)
    {}

    // 정렬된 블록 할당 (실패 시 nullptr)
    [[nodiscard]] void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) {
        std::size_t space = capacity_ - used_;
        void*       ptr   = buffer_.get() + used_;
        if (std::align(alignment, size, ptr, space)) {
            // ptr이 정렬 후 위치로 이동했으므로 used_를 갱신
            used_ = static_cast<std::size_t>(
                static_cast<std::byte*>(ptr) - buffer_.get()) + size;
            return ptr;
        }
        return nullptr;
    }

    // 풀 전체 초기화 (개별 해제 없음)
    void reset() { used_ = 0; }

    std::size_t used()      const { return used_; }
    std::size_t capacity()  const { return capacity_; }

private:
    std::unique_ptr<std::byte[]> buffer_;
    std::size_t capacity_;
    std::size_t used_;
};

struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float life;
};

static void demo_pool() {
    std::cout << "\n=== 선형 메모리 풀 데모 ===\n";

    LinearPool pool(4096);

    // 풀에서 Particle 10개 할당
    constexpr int COUNT = 10;
    Particle* particles[COUNT];

    for (int i = 0; i < COUNT; ++i) {
        particles[i] = static_cast<Particle*>(
            pool.allocate(sizeof(Particle), alignof(Particle)));
        assert(particles[i] != nullptr);
        // 초기화
        particles[i]->x    = static_cast<float>(i);
        particles[i]->y    = 0.0f;
        particles[i]->z    = 0.0f;
        particles[i]->vx   = 1.0f;
        particles[i]->vy   = 0.5f;
        particles[i]->vz   = 0.0f;
        particles[i]->life = 1.0f;
    }

    std::cout << "할당 후 사용량: " << pool.used() << " / " << pool.capacity() << " bytes\n";

    // 시뮬레이션 1 스텝
    const float dt = 0.016f;
    for (int i = 0; i < COUNT; ++i) {
        particles[i]->x    += particles[i]->vx * dt;
        particles[i]->y    += particles[i]->vy * dt;
        particles[i]->life -= 0.1f;
    }

    std::cout << "파티클 0 위치: ("
              << particles[0]->x << ", "
              << particles[0]->y << ", "
              << particles[0]->z << ")\n";

    pool.reset();
    std::cout << "풀 리셋 후 사용량: " << pool.used() << " bytes\n";
}

// ── 3. alignas 실습 ────────────────────────────────────────────────────────

static void demo_alignas() {
    std::cout << "\n=== alignas 실습 ===\n";

    alignas(64) char cache_block[64];
    std::memset(cache_block, 0, sizeof(cache_block));

    std::cout << "cache_block 주소:   " << static_cast<void*>(cache_block) << "\n";
    std::cout << "64바이트 정렬 여부: "
              << ((reinterpret_cast<uintptr_t>(cache_block) % 64 == 0)
                      ? "정렬됨 ✓"
                      : "정렬 안됨 ✗")
              << "\n";

    std::cout << "\nalignof 값 예시:\n";
    std::cout << "  char    : " << alignof(char)    << "\n";
    std::cout << "  int     : " << alignof(int)     << "\n";
    std::cout << "  double  : " << alignof(double)  << "\n";
    std::cout << "  Particle: " << alignof(Particle) << "\n";
}

int main() {
    show_layout();
    demo_pool();
    demo_alignas();
    return 0;
}
