#include <iostream>
#include <vector>
#include <memory_resource>
#include <cstring>

// 간단한 메모리 풀 구현
class SimplePoolResource : public std::pmr::memory_resource {
private:
    static constexpr std::size_t BLOCK_SIZE = 64;
    static constexpr std::size_t POOL_SIZE = 10;

    struct Block {
        std::byte data[BLOCK_SIZE];
        bool in_use;
        std::size_t size;
    };

    Block pool[POOL_SIZE];
    std::size_t allocations = 0;
    std::size_t deallocations = 0;

    void *do_allocate(std::size_t bytes, std::size_t alignment) override {
        if (bytes > BLOCK_SIZE) {
            throw std::bad_alloc();
        }

        for (std::size_t i = 0; i < POOL_SIZE; ++i) {
            if (!pool[i].in_use) {
                pool[i].in_use = true;
                pool[i].size = bytes;
                allocations++;
                std::cout << "✅ 풀에서 할당 (블록 " << i << ", " << bytes << " 바이트)\n";
                return pool[i].data;
            }
        }

        throw std::bad_alloc();
    }

    void do_deallocate(void *p, std::size_t bytes, std::size_t alignment) noexcept override {
        for (std::size_t i = 0; i < POOL_SIZE; ++i) {
            if (pool[i].data == p) {
                pool[i].in_use = false;
                deallocations++;
                std::cout << "❌ 풀에서 해제 (블록 " << i << ")\n";
                return;
            }
        }
    }

    bool do_is_equal(const memory_resource &other) const noexcept override {
        return this == &other;
    }

public:
    SimplePoolResource() {
        for (auto &block : pool) {
            block.in_use = false;
            block.size = 0;
        }
    }

    void show_stats() const {
        std::cout << "\n🔄 풀 통계:\n";
        std::cout << "  할당: " << allocations << "회\n";
        std::cout << "  해제: " << deallocations << "회\n";

        int free_blocks = 0;
        int used_blocks = 0;
        for (const auto &block : pool) {
            if (block.in_use) used_blocks++;
            else free_blocks++;
        }
        std::cout << "  사용 중: " << used_blocks << ", 가용: " << free_blocks << "\n";
    }
};

int main() {
    std::cout << "=== 메모리 풀 구현 ===\n\n";

    SimplePoolResource pool;

    std::cout << "1️⃣  메모리 풀에서 할당:\n";
    std::pmr::vector<int> v1(&pool);
    for (int i = 0; i < 3; ++i) {
        v1.push_back(i);
        std::cout << "벡터 요소 추가: " << i << "\n";
    }

    std::cout << "\n2️⃣  여러 벡터 생성:\n";
    std::pmr::vector<int> v2(&pool);
    std::pmr::vector<int> v3(&pool);
    v2.push_back(100);
    v3.push_back(200);
    std::cout << "v2: [";
    for (int x : v2) std::cout << x << " ";
    std::cout << "]\n";
    std::cout << "v3: [";
    for (int x : v3) std::cout << x << " ";
    std::cout << "]\n";

    pool.show_stats();

    std::cout << "\n3️⃣  벡터 해제:\n";
    v1.clear();
    v2.clear();
    v3.clear();

    pool.show_stats();

    return 0;
}
