#include "benchmark.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <iostream>
#include <vector>

std::uint64_t consumeVectorFront(std::size_t count)
{
    std::vector<int> values;
    values.reserve(count);
    for (std::size_t index = 0; index < count; ++index) {
        values.push_back(static_cast<int>(index));
    }

    std::uint64_t sum = 0;
    while (!values.empty()) {
        sum += static_cast<std::uint64_t>(values.front());
        values.erase(values.begin());
    }
    return sum;
}

std::uint64_t consumeDequeFront(std::size_t count)
{
    std::deque<int> values;
    for (std::size_t index = 0; index < count; ++index) {
        values.push_back(static_cast<int>(index));
    }

    std::uint64_t sum = 0;
    while (!values.empty()) {
        sum += static_cast<std::uint64_t>(values.front());
        values.pop_front();
    }
    return sum;
}

int main()
{
    constexpr std::size_t count = 6000U;

    const auto baseline = bench::run("vector-erase-front", 2, 5, [] {
        return consumeVectorFront(count);
    });
    const auto candidate = bench::run("deque-pop-front", 2, 5, [] {
        return consumeDequeFront(count);
    });

    bench::print(baseline);
    bench::print(candidate);
    bench::printMarkdownRow("2026-07-02", baseline, candidate);

    const bool regression = candidate.medianUs >= baseline.medianUs;
    std::cout << "regression gate: " << (regression ? "FAIL" : "PASS") << '\n';

    assert(baseline.guard == candidate.guard);
    assert(!regression);
    return 0;
}
