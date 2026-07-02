#include "benchmark.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

std::uint64_t buildWithoutReserve(std::size_t count)
{
    std::vector<int> values;
    for (std::size_t index = 0; index < count; ++index) {
        values.push_back(static_cast<int>(index));
    }

    std::uint64_t sum = 0;
    for (int value : values) {
        sum += static_cast<std::uint64_t>(value);
    }
    return sum;
}

std::uint64_t buildWithReserve(std::size_t count)
{
    std::vector<int> values;
    values.reserve(count);
    for (std::size_t index = 0; index < count; ++index) {
        values.push_back(static_cast<int>(index));
    }

    std::uint64_t sum = 0;
    for (int value : values) {
        sum += static_cast<std::uint64_t>(value);
    }
    return sum;
}

int main()
{
    constexpr std::size_t count = 120000U;

    const auto baseline = bench::run("no-reserve", 2, 7, [] {
        return buildWithoutReserve(count);
    });
    const auto candidate = bench::run("reserve", 2, 7, [] {
        return buildWithReserve(count);
    });

    bench::print(baseline);
    bench::print(candidate);
    bench::printMarkdownRow("2026-07-02", baseline, candidate);

    assert(baseline.guard == candidate.guard);
    return 0;
}
