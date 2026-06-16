// Session 46: 슬랩 할당자 & PMR 통합
// [1] SlabAllocator<T> — 고정 크기 오브젝트 풀
// [2] ArenaResource — std::pmr::memory_resource 어댑터
// [3] PMR 컨테이너와의 통합
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <new>
#include <memory_resource>
#include <vector>
#include <string>
#include <array>
#include <iostream>
#include <string_view>
#include <algorithm>

// ─────────────────────────────────────────────
// 정렬 유틸리티
// ─────────────────────────────────────────────

constexpr std::size_t align_up(std::size_t n, std::size_t align) noexcept {
    return (n + align - 1) & ~(align - 1);
}

// ─────────────────────────────────────────────
// [1] SlabAllocator<T, Cap> — 프리 리스트 풀
// ─────────────────────────────────────────────

template <typename T, std::size_t Cap>
class SlabAllocator {
    union Slot {
        alignas(T) std::byte storage[sizeof(T)];
        Slot* next;
    };

public:
    SlabAllocator() noexcept {
        for (std::size_t i = 0; i + 1 < Cap; ++i)
            slots_[i].next = &slots_[i + 1];
        slots_[Cap - 1].next = nullptr;
        free_head_ = &slots_[0];
    }

    ~SlabAllocator() = default;
    SlabAllocator(const SlabAllocator&)            = delete;
    SlabAllocator& operator=(const SlabAllocator&) = delete;

    template <typename... Args>
    T* create(Args&&... args) {
        if (!free_head_) return nullptr;
        Slot* slot  = free_head_;
        free_head_  = slot->next;
        ++in_use_;
        return ::new(slot->storage) T(std::forward<Args>(args)...);
    }

    void destroy(T* p) noexcept {
        if (!p) return;
        p->~T();
        auto* slot   = reinterpret_cast<Slot*>(p);
        slot->next   = free_head_;
        free_head_   = slot;
        --in_use_;
    }

    std::size_t capacity() const noexcept { return Cap; }
    std::size_t in_use()   const noexcept { return in_use_; }
    std::size_t free_count() const noexcept { return Cap - in_use_; }

    void print_stats(std::string_view label) const {
        std::cout << "[SlabAllocator:" << label << "] "
                  << "cap=" << Cap
                  << "  in_use=" << in_use_
                  << "  free=" << free_count() << "\n";
    }

private:
    Slot        slots_[Cap];
    Slot*       free_head_{};
    std::size_t in_use_{};
};

// ─────────────────────────────────────────────
// [2] ArenaResource — PMR 어댑터
// ─────────────────────────────────────────────

class ArenaResource : public std::pmr::memory_resource {
public:
    explicit ArenaResource(void* buf, std::size_t cap,
                           std::pmr::memory_resource* upstream =
                               std::pmr::null_memory_resource()) noexcept
        : base_(static_cast<std::byte*>(buf)),
          capacity_(cap),
          upstream_(upstream) {}

    std::size_t used()     const noexcept { return offset_; }
    std::size_t capacity() const noexcept { return capacity_; }
    std::size_t allocs()   const noexcept { return alloc_count_; }
    std::size_t deallocs() const noexcept { return dealloc_count_; }

    void reset() noexcept { offset_ = 0; alloc_count_ = 0; dealloc_count_ = 0; }

    void print_stats(std::string_view label) const {
        std::cout << "[ArenaResource:" << label << "] "
                  << "used=" << used() << "/" << capacity()
                  << "  allocs=" << allocs()
                  << "  deallocs=" << deallocs() << "\n";
    }

private:
    void* do_allocate(std::size_t bytes, std::size_t align) override {
        std::size_t aligned = align_up(offset_, align);
        if (aligned + bytes > capacity_) {
            return upstream_->allocate(bytes, align);
        }
        void* ptr = base_ + aligned;
        offset_ = aligned + bytes;
        ++alloc_count_;
        return ptr;
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t align) override {
        ++dealloc_count_;
        // 아레나 범위 밖(upstream으로 흘러간 것)만 upstream에 반환
        auto* bp = static_cast<std::byte*>(p);
        if (bp < base_ || bp >= base_ + capacity_) {
            upstream_->deallocate(p, bytes, align);
        }
        // 아레나 내부 블록은 reset() 시 일괄 해제 (no-op)
    }

    bool do_is_equal(const std::pmr::memory_resource& o) const noexcept override {
        return this == &o;
    }

    std::byte*                  base_{};
    std::size_t                 capacity_{};
    std::size_t                 offset_{};
    std::size_t                 alloc_count_{};
    std::size_t                 dealloc_count_{};
    std::pmr::memory_resource*  upstream_{};
};

// ─────────────────────────────────────────────
// 데모용 타입
// ─────────────────────────────────────────────

struct Node {
    int   value;
    Node* left{};
    Node* right{};
    explicit Node(int v) : value(v) {}
};

struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float lifetime;
    int   id;
};

// ─────────────────────────────────────────────
// main
// ─────────────────────────────────────────────

int main() {
    std::cout << "=== [1] SlabAllocator<Node, 16> ===\n";
    {
        SlabAllocator<Node, 16> pool;
        pool.print_stats("Node/16");

        // BST 구성
        auto* root  = pool.create(10);
        auto* left  = pool.create(5);
        auto* right = pool.create(15);
        root->left  = left;
        root->right = right;

        std::cout << "루트=" << root->value
                  << " 왼=" << left->value
                  << " 오=" << right->value << "\n";
        pool.print_stats("3개 생성");

        pool.destroy(left);
        pool.print_stats("left 해제");

        auto* reused = pool.create(7);
        std::cout << "재사용 노드=" << reused->value << "\n";
        pool.print_stats("재사용 후");

        pool.destroy(root);
        pool.destroy(right);
        pool.destroy(reused);
        pool.print_stats("전체 해제");
    }

    std::cout << "\n=== [2] SlabAllocator<Particle, 32> — OOM 처리 ===\n";
    {
        SlabAllocator<Particle, 4> tiny_pool;
        Particle* ptrs[6]{};
        for (int i = 0; i < 6; ++i) {
            ptrs[i] = tiny_pool.create(float(i), 0.0f, 0.0f,
                                       0.0f, 1.0f, 0.0f,
                                       3.0f, i);
            std::cout << "  [" << i << "] " << (ptrs[i] ? "✅" : "❌ OOM") << "\n";
        }
        tiny_pool.print_stats("cap=4,alloc=6");

        for (auto* p : ptrs) tiny_pool.destroy(p);
    }

    std::cout << "\n=== [3] ArenaResource + std::pmr::vector ===\n";
    {
        alignas(std::max_align_t) std::array<std::byte, 4096> buf{};
        ArenaResource arena(buf.data(), buf.size());

        {
            std::pmr::vector<int> nums(&arena);
            for (int i = 0; i < 100; ++i) nums.push_back(i * i);

            std::cout << "nums.size()=" << nums.size()
                      << "  sum=" << [&]{
                          long long s = 0;
                          for (int v : nums) s += v;
                          return s;
                      }() << "\n";
            arena.print_stats("vector<int>/100");
        }
        arena.print_stats("vector 소멸 후");
    }

    std::cout << "\n=== [4] ArenaResource + std::pmr::string ===\n";
    {
        alignas(std::max_align_t) std::array<std::byte, 2048> buf{};
        ArenaResource arena(buf.data(), buf.size());

        std::pmr::vector<std::pmr::string> words(&arena);
        for (auto sv : {"hello", "world", "arena", "allocator", "pmr", "c++20"}) {
            words.push_back(std::pmr::string(sv, &arena));
        }

        for (const auto& w : words) std::cout << "  " << w << "\n";
        arena.print_stats("strings/6개");
    }

    std::cout << "\n=== [5] 아레나 리셋 & 재사용 ===\n";
    {
        alignas(std::max_align_t) std::array<std::byte, 1024> buf{};
        ArenaResource arena(buf.data(), buf.size());

        for (int round = 1; round <= 3; ++round) {
            std::pmr::vector<double> frame_data(&arena);
            for (int i = 0; i < 20; ++i) frame_data.push_back(double(i) * round);
            std::cout << "라운드 " << round
                      << ": used=" << arena.used()
                      << " allocs=" << arena.allocs() << "\n";
            // 프레임 끝 — 아레나 리셋 (전체 해제)
            arena.reset();
        }
    }

    std::cout << "\n=== 세션 46 Part 2 완료 ===\n";
    return 0;
}
