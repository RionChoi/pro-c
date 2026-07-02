#if defined(PRO_C_USE_MODULES)
import statistics;
#else
#include "statistics.hpp"
#endif

#include <iostream>
#include <vector>

int main()
{
    const std::vector<double> knight{4.0, 6.0, 3.0, 5.0, 6.0};
    const std::vector<double> dragon{2.0, 6.0, 6.0, 1.0, 4.0};

    const Stats::Summary knightSummary = Stats::summarize(knight);
    const Stats::Summary dragonSummary = Stats::summarize(dragon);

    std::cout << "mode: " << Stats::activeImportMode() << '\n';
    std::cout << "knight avg: " << knightSummary.mean << '\n';
    std::cout << "dragon avg: " << dragonSummary.mean << '\n';
    std::cout << "winner: "
              << (knightSummary.mean >= dragonSummary.mean ? "knight" : "dragon")
              << '\n';
    return 0;
}
