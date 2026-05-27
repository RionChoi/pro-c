// 모듈 인터페이스 유닛: statistics
// 컴파일: g++ -std=c++23 -fmodules-ts -Wall -Wextra -Wpedantic -c homework.cpp

module;
#include <vector>
#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <string>
#include <utility>

export module statistics;

export namespace Stats {

    struct Summary {
        double mean;
        double median;
        double std_dev;
        double min_val;
        double max_val;
    };

    double mean(const std::vector<double>& v);
    double median(std::vector<double> v);
    double variance(const std::vector<double>& v);
    double std_deviation(const std::vector<double>& v);
    Summary summarize(const std::vector<double>& v);

}

namespace Stats {

    double mean(const std::vector<double>& v) {
        if (v.empty()) throw std::invalid_argument("빈 벡터");
        return std::accumulate(v.begin(), v.end(), 0.0)
               / static_cast<double>(v.size());
    }

    double median(std::vector<double> v) {
        if (v.empty()) throw std::invalid_argument("빈 벡터");
        std::sort(v.begin(), v.end());
        size_t n = v.size();
        return (n % 2 == 0)
               ? (v[n / 2 - 1] + v[n / 2]) / 2.0
               : v[n / 2];
    }

    double variance(const std::vector<double>& v) {
        double m = mean(v);
        double sum = 0.0;
        for (double x : v)
            sum += (x - m) * (x - m);
        return sum / static_cast<double>(v.size());
    }

    double std_deviation(const std::vector<double>& v) {
        return std::sqrt(variance(v));
    }

    Summary summarize(const std::vector<double>& v) {
        if (v.empty()) throw std::invalid_argument("빈 벡터");
        auto [mn_it, mx_it] = std::minmax_element(v.begin(), v.end());
        return {mean(v), median(v), std_deviation(v), *mn_it, *mx_it};
    }

}
