// Session 46: 고성능 메모리 아레나 & 풀
// [1] LinearArena — 범프 포인터 할당자
// [2] StackArena  — 마커 기반 LIFO 할당자
// [3] 정렬 지원 및 할당 통계
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <new>
#include <iostream>
#include <string_view>
#include <array>

// ─────────────────────────────────────────────
// 정렬 유틸리티
// ─────────────────────────────────────────────

constexpr std::size_t align_up(std::size_t n, std::size_t align) noexcept {
    return (n + align - 1) & ~(align - 1);
}

static_assert(align_up(1,  8) == 8);
static_assert(align_up(8,  8) == 8);
static_assert(align_up(9,  8) == 16);
static_assert(align_up(17, 16) == 32);

// ─────────────────────────────────────────────
// [1] LinearArena — 단방향 범프 포인터
// ─────────────────────────────────────────────

class LinearArena {
public:
    explicit LinearArena(void* buf, std::size_t cap) noexcept
        : base_(static_cast<std::byte*>(buf)), capacity_(cap) {}

    void* alloc(std::size_t size, std::size_t align = alignof(std::max_align_t)) noexcept {
        std::size_t aligned = align_up(offset_, align);
        if (aligned + size > capacity_) return nullptr;
        void* ptr = base_ + aligned;
        offset_ = aligned + size;
        ++alloc_count_;
        peak_ = (offset_ > peak_) ? offset_ : peak_;
        return ptr;
    }

    template <typename T, typename... Args>
    T* create(Args&&... args) {
        void* p = alloc(sizeof(T), alignof(T));
        if (!p) return nullptr;
        return ::new(p) T(std::forward<Args>(args)...);
    }

    void reset() noexcept { offset_ = 0; alloc_count_ = 0; }

    std::size_t used()        const noexcept { return offset_; }
    std::size_t capacity()    const noexcept { return capacity_; }
    std::size_t peak()        const noexcept { return peak_; }
    std::size_t alloc_count() const noexcept { return alloc_count_; }
    double      utilization() const noexcept {
        return capacity_ ? (100.0 * offset_ / capacity_) : 0.0;
    }

    void print_stats(std::string_view label) const {
        std::cout << "[LinearArena:" << label << "] "
                  << "used=" << used() << "/" << capacity()
                  << "  peak=" << peak()
                  << "  allocs=" << alloc_count()
                  << "  util=" << utilization() << "%\n";
    }

private:
    std::byte*  base_{};
    std::size_t capacity_{};
    std::size_t offset_{};
    std::size_t peak_{};
    std::size_t alloc_count_{};
};

// ─────────────────────────────────────────────
// [2] StackArena — 마커(marker) 기반 LIFO 해제
// ─────────────────────────────────────────────

class StackArena {
public:
    using Marker = std::size_t;

    explicit StackArena(void* buf, std::size_t cap) noexcept
        : base_(static_cast<std::byte*>(buf)), capacity_(cap) {}

    void* alloc(std::size_t size, std::size_t align = alignof(std::max_align_t)) noexcept {
        std::size_t aligned = align_up(offset_, align);
        if (aligned + size > capacity_) return nullptr;
        void* ptr = base_ + aligned;
        offset_ = aligned + size;
        return ptr;
    }

    template <typename T, typename... Args>
    T* create(Args&&... args) {
        void* p = alloc(sizeof(T), alignof(T));
        if (!p) return nullptr;
        return ::new(p) T(std::forward<Args>(args)...);
    }

    Marker save()                 const noexcept { return offset_; }
    void   restore(Marker m)            noexcept { offset_ = m; }
    void   reset()                      noexcept { offset_ = 0; }

    std::size_t used()     const noexcept { return offset_; }
    std::size_t capacity() const noexcept { return capacity_; }

    void print_stats(std::string_view label) const {
        std::cout << "[StackArena:" << label << "] "
                  << "used=" << used() << "/" << capacity() << "\n";
    }

private:
    std::byte*  base_{};
    std::size_t capacity_{};
    std::size_t offset_{};
};

// ─────────────────────────────────────────────
// 데모용 구조체
// ─────────────────────────────────────────────

struct Vec3 { float x, y, z; };
struct Transform { Vec3 pos; Vec3 rot; Vec3 scale; };
struct Particle { Vec3 pos; Vec3 vel; float lifetime; int id; };

// ─────────────────────────────────────────────
// main
// ─────────────────────────────────────────────

int main() {
    std::cout << "=== [1] LinearArena 기본 동작 ===\n";
    {
        alignas(std::max_align_t) std::array<std::byte, 1024> buf{};
        LinearArena arena(buf.data(), buf.size());

        auto* p1 = arena.create<Vec3>(1.0f, 2.0f, 3.0f);
        auto* p2 = arena.create<Transform>();
        auto* p3 = arena.alloc(100);  // 100바이트 raw 블록

        std::cout << "Vec3 at "      << static_cast<void*>(p1)
                  << " val=(" << p1->x << "," << p1->y << "," << p1->z << ")\n";
        std::cout << "Transform at " << static_cast<void*>(p2) << "\n";
        std::cout << "raw[100] at "  << p3 << "\n";
        arena.print_stats("demo");

        arena.reset();
        std::cout << "reset 후: ";
        arena.print_stats("demo");
    }

    std::cout << "\n=== [2] LinearArena — 정렬 검증 ===\n";
    {
        alignas(std::max_align_t) std::array<std::byte, 512> buf{};
        LinearArena arena(buf.data(), buf.size());

        auto check_align = [](void* p, std::size_t align, std::string_view name) {
            uintptr_t addr = reinterpret_cast<uintptr_t>(p);
            bool ok = (addr % align) == 0;
            std::cout << "  " << name << " addr=" << addr
                      << " align=" << align << (ok ? " ✅" : " ❌") << "\n";
        };

        auto* a = arena.alloc(1,  1);
        auto* b = arena.alloc(4,  4);
        auto* c = arena.alloc(8,  8);
        auto* d = arena.alloc(16, 16);

        check_align(a, 1,  "1B  블록");
        check_align(b, 4,  "4B  정렬");
        check_align(c, 8,  "8B  정렬");
        check_align(d, 16, "16B 정렬");
        arena.print_stats("align-check");
    }

    std::cout << "\n=== [3] LinearArena — 파티클 배치 할당 ===\n";
    {
        alignas(std::max_align_t) std::array<std::byte, 4096> buf{};
        LinearArena arena(buf.data(), buf.size());

        constexpr int N = 50;
        Particle* particles = static_cast<Particle*>(
            arena.alloc(sizeof(Particle) * N, alignof(Particle)));

        for (int i = 0; i < N; ++i) {
            particles[i] = {{float(i), 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 5.0f, i};
        }
        std::cout << "파티클 " << N << "개 할당 완료\n";
        std::cout << "particles[0].id=" << particles[0].id
                  << "  particles[49].id=" << particles[49].id << "\n";
        arena.print_stats("particles");
    }

    std::cout << "\n=== [4] StackArena — 마커 기반 해제 ===\n";
    {
        alignas(std::max_align_t) std::array<std::byte, 512> buf{};
        StackArena arena(buf.data(), buf.size());

        auto* base_obj = arena.create<Vec3>(0.0f, 0.0f, 0.0f);
        auto marker1 = arena.save();
        std::cout << "marker1=" << marker1 << " (base_obj 이후)\n";

        auto* temp1 = arena.create<Transform>();
        auto* temp2 = arena.create<Particle>();
        auto marker2 = arena.save();
        std::cout << "marker2=" << marker2 << " (임시 객체 2개 후)\n";
        (void)temp1; (void)temp2;

        arena.create<Vec3>(9.0f, 9.0f, 9.0f);
        std::cout << "추가 할당 후 used=" << arena.used() << "\n";

        arena.restore(marker2);
        std::cout << "marker2 복원 후 used=" << arena.used() << "\n";

        arena.restore(marker1);
        std::cout << "marker1 복원 후 used=" << arena.used() << "\n";
        std::cout << "base_obj 여전히 유효: ("
                  << base_obj->x << "," << base_obj->y << "," << base_obj->z << ")\n";
    }

    std::cout << "\n=== [5] OOM 처리 ===\n";
    {
        alignas(std::max_align_t) std::array<std::byte, 64> small_buf{};
        LinearArena tiny(small_buf.data(), small_buf.size());

        auto* ok  = tiny.alloc(32);
        auto* oom = tiny.alloc(64);
        std::cout << "32B 할당: " << (ok  ? "✅" : "❌ OOM") << "\n";
        std::cout << "64B 할당: " << (oom ? "✅" : "❌ OOM (예상 동작)") << "\n";
        tiny.print_stats("tiny");
    }

    std::cout << "\n=== 세션 46 Part 1 완료 ===\n";
    return 0;
}
