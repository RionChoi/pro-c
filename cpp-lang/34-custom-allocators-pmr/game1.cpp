#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <array>
#include <cstring>

template<typename T>
class StackAllocator {
private:
    static constexpr std::size_t STACK_SIZE = 2097152;
    std::array<char, STACK_SIZE> stack;
    std::size_t offset = 0;

public:
    using value_type = T;

    StackAllocator() = default;

    template<typename U>
    StackAllocator(const StackAllocator<U>&) : offset(0) {}

    T* allocate(std::size_t n) {
        std::size_t needed = n * sizeof(T);
        if (offset + needed > STACK_SIZE) {
            throw std::bad_alloc();
        }
        T* result = reinterpret_cast<T*>(stack.data() + offset);
        offset += needed;
        return result;
    }

    void deallocate(T*, std::size_t) noexcept {}

    void reset() {
        offset = 0;
    }

    std::size_t getUsedMemory() const {
        return offset;
    }
};

struct GameResult {
    int allocations;
    long long time_ms;
    double throughput;
};

GameResult benchmarkAllocations(int num_allocations) {
    auto start = std::chrono::high_resolution_clock::now();
    {
        std::vector<int> allocs;
        for (int i = 0; i < num_allocations; ++i) {
            allocs.push_back(rand() % 1000);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();

    long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    double throughput = (elapsed > 0) ? (double)num_allocations / elapsed * 1000 : 0;

    return {num_allocations, elapsed, throughput};
}

GameResult benchmarkStackAllocations(int num_allocations) {
    auto start = std::chrono::high_resolution_clock::now();
    {
        std::vector<int, StackAllocator<int>> allocs;
        for (int i = 0; i < num_allocations; ++i) {
            allocs.push_back(rand() % 1000);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();

    long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    double throughput = (elapsed > 0) ? (double)num_allocations / elapsed * 1000 : 0;

    return {num_allocations, elapsed, throughput};
}

void playMemoryGame() {
    std::cout << "\n╔════════════════════════════════════════╗\n";
    std::cout << "║  MEMORY ALLOCATION RACE GAME          ║\n";
    std::cout << "║  Standard Allocator vs Stack          ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";

    srand(static_cast<unsigned>(time(nullptr)));

    const int ROUNDS = 5;
    const int ALLOCATIONS_PER_ROUND[] = {1000, 5000, 10000, 50000, 100000};

    std::cout << "\n┌─ Round Setup ─────────────────────────┐\n";
    std::cout << "│ Each round: allocate N random values │\n";
    std::cout << "│ Metric: allocations per millisecond  │\n";
    std::cout << "└───────────────────────────────────────┘\n";

    int stack_wins = 0;
    int standard_wins = 0;

    for (int round = 0; round < ROUNDS; ++round) {
        int num_alloc = ALLOCATIONS_PER_ROUND[round];
        std::cout << "\n┌─ ROUND " << (round + 1) << " : " << num_alloc << " allocations ─";
        for (int i = 0; i < (27 - std::to_string(num_alloc).length()); ++i) std::cout << "─";
        std::cout << "┐\n";

        GameResult standard = benchmarkAllocations(num_alloc);
        GameResult stack = benchmarkStackAllocations(num_alloc);

        std::cout << "│ Standard Allocator                  │\n";
        std::cout << "│   Time: " << standard.time_ms << "ms";
        for (int i = 0; i < 25 - std::to_string(standard.time_ms).length(); ++i) std::cout << " ";
        std::cout << "│\n";
        std::cout << "│   Throughput: " << standard.throughput << " alloc/ms";
        for (int i = 0; i < 13 - std::to_string((long long)standard.throughput).length(); ++i) std::cout << " ";
        std::cout << "│\n";

        std::cout << "│ Stack Allocator                     │\n";
        std::cout << "│   Time: " << stack.time_ms << "ms";
        for (int i = 0; i < 27 - std::to_string(stack.time_ms).length(); ++i) std::cout << " ";
        std::cout << "│\n";
        std::cout << "│   Throughput: " << stack.throughput << " alloc/ms";
        for (int i = 0; i < 13 - std::to_string((long long)stack.throughput).length(); ++i) std::cout << " ";
        std::cout << "│\n";

        if (stack.time_ms < standard.time_ms) {
            std::cout << "│ ✓ Stack Allocator WINS!            │\n";
            stack_wins++;
        } else {
            std::cout << "│ ✓ Standard Allocator WINS!         │\n";
            standard_wins++;
        }

        std::cout << "└───────────────────────────────────────┘\n";
    }

    std::cout << "\n╔════════════════════════════════════════╗\n";
    std::cout << "║  FINAL SCORE                          ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";
    std::cout << "║ Stack Allocator:     " << stack_wins << " rounds              ║\n";
    std::cout << "║ Standard Allocator:  " << standard_wins << " rounds              ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";

    if (stack_wins > standard_wins) {
        std::cout << "║ 🏆 STACK ALLOCATOR CHAMPION! 🏆      ║\n";
        std::cout << "║ Pre-allocated memory is much faster! ║\n";
    } else if (standard_wins > stack_wins) {
        std::cout << "║ Modern allocators are optimized!     ║\n";
    } else {
        std::cout << "║ It's a tie! Both are competitive.    ║\n";
    }

    std::cout << "╚════════════════════════════════════════╝\n";
}

int main() {
    std::cout << "╭────────────────────────────────────────╮\n";
    std::cout << "│  MEMORY ALLOCATION STRATEGY GAME       │\n";
    std::cout << "│                                        │\n";
    std::cout << "│  Learn how different allocators      │\n";
    std::cout << "│  affect performance in real-world     │\n";
    std::cout << "│  scenarios.                           │\n";
    std::cout << "╰────────────────────────────────────────╯\n";

    playMemoryGame();

    std::cout << "\n╔════════════════════════════════════════╗\n";
    std::cout << "║  KEY INSIGHTS                         ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";
    std::cout << "║ • Stack Allocators: O(1) allocation   ║\n";
    std::cout << "║ • No fragmentation risk                ║\n";
    std::cout << "║ • Perfect for fixed-size workloads     ║\n";
    std::cout << "║ • Excellent cache locality             ║\n";
    std::cout << "║                                        ║\n";
    std::cout << "║ • Standard Allocators: Flexible        ║\n";
    std::cout << "║ • Can allocate arbitrary sizes         ║\n";
    std::cout << "║ • Better for variable workloads        ║\n";
    std::cout << "║ • Worth the allocation overhead        ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";

    return 0;
}
