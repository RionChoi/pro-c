#include <iostream>
#include <memory_resource>
#include <vector>
#include <string>
#include <cstring>
#include <array>

class PoolResource : public std::pmr::memory_resource {
private:
    static constexpr std::size_t POOL_SIZE = 4096;
    std::array<char, POOL_SIZE> pool;
    std::size_t offset = 0;
    int allocation_count = 0;

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        if (offset + bytes > POOL_SIZE) {
            std::cerr << "PoolResource exhausted! Requested: " << bytes
                      << ", Available: " << (POOL_SIZE - offset) << "\n";
            throw std::bad_alloc();
        }

        char* result = pool.data() + offset;
        offset += bytes;
        allocation_count++;

        std::cout << "[PoolResource] Allocated " << bytes << " bytes at offset "
                  << (offset - bytes) << " (allocation #" << allocation_count << ")\n";
        return result;
    }

    void do_deallocate(void*, std::size_t, std::size_t) noexcept override {
        std::cout << "[PoolResource] Deallocate called (no-op in pool)\n";
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

public:
    std::size_t getUsedMemory() const { return offset; }
    int getAllocationCount() const { return allocation_count; }
};

class StatisticsResource : public std::pmr::memory_resource {
private:
    std::pmr::memory_resource* upstream;
    std::size_t total_allocated = 0;
    std::size_t total_deallocated = 0;
    int allocation_count = 0;

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        void* ptr = upstream->allocate(bytes, alignment);
        total_allocated += bytes;
        allocation_count++;
        std::cout << "[StatisticsResource] Allocated " << bytes << " bytes "
                  << "(total: " << total_allocated << " bytes, call #" << allocation_count << ")\n";
        return ptr;
    }

    void do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment) noexcept override {
        upstream->deallocate(ptr, bytes, alignment);
        total_deallocated += bytes;
        std::cout << "[StatisticsResource] Deallocated " << bytes << " bytes\n";
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

public:
    StatisticsResource(std::pmr::memory_resource* upstream = std::pmr::get_default_resource())
        : upstream(upstream) {}

    void printStats() const {
        std::cout << "\n=== StatisticsResource Statistics ===\n";
        std::cout << "Total allocated: " << total_allocated << " bytes\n";
        std::cout << "Total deallocated: " << total_deallocated << " bytes\n";
        std::cout << "Currently in use: " << (total_allocated - total_deallocated) << " bytes\n";
        std::cout << "Allocation calls: " << allocation_count << "\n";
    }
};

int main() {
    std::cout << "=== std::pmr with PoolResource ===\n";
    {
        PoolResource pool_resource;
        std::pmr::polymorphic_allocator<int> alloc(&pool_resource);

        std::cout << "\nCreating pmr::vector with PoolResource:\n";
        std::pmr::vector<int> vec(&pool_resource);
        vec = {10, 20, 30, 40, 50};

        std::cout << "\nCreating pmr::string with PoolResource:\n";
        std::pmr::string str(&pool_resource);
        str = "Memory pooling!";
        std::cout << "String: " << str << "\n";

        std::cout << "\nPool usage: " << pool_resource.getUsedMemory() << " bytes\n";
        std::cout << "Allocations: " << pool_resource.getAllocationCount() << "\n";
    }

    std::cout << "\n=== std::pmr with StatisticsResource ===\n";
    {
        StatisticsResource stats_resource;

        std::cout << "\nCreating vectors with StatisticsResource:\n";
        std::pmr::vector<double> v1(&stats_resource);
        v1 = {1.1, 2.2, 3.3};

        std::pmr::vector<int> v2(&stats_resource);
        v2 = {100, 200, 300, 400, 500};

        std::cout << "\nCreating string:\n";
        std::pmr::string msg(&stats_resource);
        msg = "Custom memory tracking";

        std::cout << "\nClearing vectors:\n";
        v1.clear();
        v2.clear();
        msg.clear();

        stats_resource.printStats();
    }

    std::cout << "\n=== Comparison: Default vs PoolResource vs StatisticsResource ===\n";
    {
        std::cout << "\n1. Default allocator:\n";
        {
            std::pmr::vector<int> default_vec;
            default_vec = {1, 2, 3, 4, 5};
            std::cout << "Default vector: size=" << default_vec.size() << "\n";
        }

        std::cout << "\n2. PoolResource:\n";
        {
            PoolResource pool;
            std::pmr::vector<int> pool_vec(&pool);
            pool_vec = {1, 2, 3, 4, 5};
            std::cout << "Pool vector: size=" << pool_vec.size() << "\n";
            std::cout << "Pool usage: " << pool.getUsedMemory() << " bytes\n";
        }

        std::cout << "\n3. StatisticsResource:\n";
        {
            StatisticsResource stats;
            std::pmr::vector<int> stats_vec(&stats);
            stats_vec = {1, 2, 3, 4, 5};
            std::cout << "Stats vector: size=" << stats_vec.size() << "\n";
            stats.printStats();
        }
    }

    return 0;
}
