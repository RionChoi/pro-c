#include "benchmark.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

std::uint64_t sumContiguous(const std::vector<int>& values)
{
    std::uint64_t sum = 0;
    for (int value : values) {
        sum += static_cast<std::uint64_t>(value);
    }
    return sum;
}

std::uint64_t sumStrided(const std::vector<int>& values, std::size_t stride)
{
    std::uint64_t sum = 0;
    for (std::size_t offset = 0; offset < stride; ++offset) {
        for (std::size_t index = offset; index < values.size(); index += stride) {
            sum += static_cast<std::uint64_t>(values[index]);
        }
    }
    return sum;
}

int main()
{
    std::vector<int> values(1U << 18U);
    for (std::size_t index = 0; index < values.size(); ++index) {
        values[index] = static_cast<int>(index % 97U);
    }

    const auto contiguous = bench::run("contiguous", 3, 9, [&values] {
        return sumContiguous(values);
    });
    const auto strided = bench::run("stride-64", 3, 9, [&values] {
        return sumStrided(values, 64U);
    });

    bench::print(contiguous);
    bench::print(strided);

    assert(contiguous.guard == strided.guard);
    return 0;
}
