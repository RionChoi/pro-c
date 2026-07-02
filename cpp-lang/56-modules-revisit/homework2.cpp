#if defined(PRO_C_USE_MODULES)
import statistics;
#else
#include "statistics.hpp"
#endif

#include <iostream>
#include <vector>

int main()
{
    const std::vector<double> scores{72.0, 84.5, 91.0, 66.5, 88.0, 79.5};
    const Stats::Summary summary = Stats::summarize(scores);

    std::cout << "mode: " << Stats::activeImportMode() << '\n';
    std::cout << "class mean: " << summary.mean << '\n';
    std::cout << "class median: " << summary.median << '\n';
    std::cout << "range: " << summary.min_val << " .. " << summary.max_val << '\n';
    return 0;
}
