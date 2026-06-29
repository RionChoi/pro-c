#include "reclaim.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <mutex>
#include <optional>
#include <thread>
#include <vector>

int main()
{
    reclaim::AtomicStack<int> stack;
    for (int value = 1; value <= 8; ++value) {
        stack.push(value);
    }

    std::mutex resultsMutex;
    std::vector<int> results;

    auto consumer = [&stack, &results, &resultsMutex] {
        while (true) {
            std::optional<int> value = stack.pop();
            if (!value.has_value()) {
                return;
            }

            std::lock_guard<std::mutex> lock(resultsMutex);
            results.push_back(*value);
        }
    };

    std::thread left(consumer);
    std::thread right(consumer);
    left.join();
    right.join();

    const std::size_t reclaimed = stack.reclaimNow();

    std::ranges::sort(results);
    assert(results.size() == 8U);
    for (int index = 0; index < 8; ++index) {
        assert(results[static_cast<std::size_t>(index)] == index + 1);
    }

    std::cout << "popped:";
    for (int value : results) {
        std::cout << ' ' << value;
    }
    std::cout << "\nreclaimed now: " << reclaimed
              << "\npending retired: " << stack.pendingRetired() << '\n';
    return 0;
}
