#ifndef PRO_C_BENCHMARK_HPP
#define PRO_C_BENCHMARK_HPP

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace bench {

using Clock = std::chrono::steady_clock;
using Us = std::chrono::duration<double, std::micro>;

inline volatile std::uint64_t sink = 0;

struct Result {
    std::string name;
    double medianUs;
    double bestUs;
    double worstUs;
    std::uint64_t guard;
};

inline double median(std::vector<double> samples)
{
    std::ranges::sort(samples);
    const std::size_t count = samples.size();

    if ((count % 2U) == 0U) {
        return (samples[count / 2U - 1U] + samples[count / 2U]) / 2.0;
    }

    return samples[count / 2U];
}

template <typename F>
Result run(const std::string& name, int warmup, int iterations, F fn)
{
    for (int round = 0; round < warmup; ++round) {
        sink ^= static_cast<std::uint64_t>(fn());
    }

    std::vector<double> samples;
    samples.reserve(static_cast<std::size_t>(iterations));
    std::uint64_t guard = 0;

    for (int round = 0; round < iterations; ++round) {
        const auto start = Clock::now();
        const std::uint64_t value = static_cast<std::uint64_t>(fn());
        const auto end = Clock::now();

        sink ^= value + static_cast<std::uint64_t>(round);
        guard ^= value;
        samples.push_back(Us(end - start).count());
    }

    const auto [bestIt, worstIt] = std::minmax_element(samples.begin(), samples.end());
    return Result{name, median(samples), *bestIt, *worstIt, guard};
}

inline void print(const Result& result)
{
    std::cout << std::fixed << std::setprecision(2)
              << result.name
              << " median=" << result.medianUs << "us"
              << " best=" << result.bestUs << "us"
              << " worst=" << result.worstUs << "us"
              << " guard=" << result.guard << '\n';
}

inline void printMarkdownRow(const std::string& date,
                             const Result& baseline,
                             const Result& candidate)
{
    const double ratio = candidate.medianUs / baseline.medianUs;
    std::cout << "| " << date
              << " | " << baseline.name
              << " | " << candidate.name
              << " | " << std::fixed << std::setprecision(2) << baseline.medianUs
              << "us | " << candidate.medianUs
              << "us | " << std::setprecision(3) << ratio << "x |\n";
}

}  // namespace bench

#endif
