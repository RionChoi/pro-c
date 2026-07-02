#ifndef PRO_C_STATISTICS_HPP
#define PRO_C_STATISTICS_HPP

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace Stats {

struct Summary {
    double mean;
    double median;
    double std_dev;
    double min_val;
    double max_val;
};

inline double mean(const std::vector<double>& values)
{
    if (values.empty()) {
        throw std::invalid_argument("empty dataset");
    }

    return std::accumulate(values.begin(), values.end(), 0.0)
           / static_cast<double>(values.size());
}

inline double median(std::vector<double> values)
{
    if (values.empty()) {
        throw std::invalid_argument("empty dataset");
    }

    std::ranges::sort(values);
    const std::size_t count = values.size();

    if ((count % 2U) == 0U) {
        return (values[count / 2U - 1U] + values[count / 2U]) / 2.0;
    }

    return values[count / 2U];
}

inline double variance(const std::vector<double>& values)
{
    const double average = mean(values);
    double sum = 0.0;

    for (double value : values) {
        const double distance = value - average;
        sum += distance * distance;
    }

    return sum / static_cast<double>(values.size());
}

inline double stdDeviation(const std::vector<double>& values)
{
    return std::sqrt(variance(values));
}

inline Summary summarize(const std::vector<double>& values)
{
    if (values.empty()) {
        throw std::invalid_argument("empty dataset");
    }

    const auto [minIt, maxIt] = std::minmax_element(values.begin(), values.end());
    return Summary{
        mean(values),
        median(values),
        stdDeviation(values),
        *minIt,
        *maxIt,
    };
}

inline std::string activeImportMode()
{
#if defined(PRO_C_USE_MODULES)
    return "named module";
#else
    return "header fallback";
#endif
}

}  // namespace Stats

#endif
