#include "statistics.hpp"

#include <iostream>
#include <vector>

int main()
{
    const std::vector<double> values{82.0, 91.5, 77.0, 88.0, 95.0};
    const Stats::Summary summary = Stats::summarize(values);

#ifdef __cpp_modules
    constexpr long long modulesValue = __cpp_modules;
#else
    constexpr long long modulesValue = 0;
#endif

    std::cout << "__cpp_modules=" << modulesValue << '\n';
    std::cout << "consumer mode: " << Stats::activeImportMode() << '\n';
    std::cout << "mean=" << summary.mean
              << " median=" << summary.median
              << " stddev=" << summary.std_dev << '\n';
    return 0;
}
