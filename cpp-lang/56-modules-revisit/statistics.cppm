module;
#include "statistics.hpp"

export module statistics;

export namespace Stats {
using ::Stats::Summary;
using ::Stats::activeImportMode;
using ::Stats::mean;
using ::Stats::median;
using ::Stats::stdDeviation;
using ::Stats::summarize;
using ::Stats::variance;
}  // namespace Stats
