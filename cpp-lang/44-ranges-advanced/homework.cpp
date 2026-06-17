// Session 44: C++20 Ranges 심화 - lazy pipeline과 커스텀 stride view
#include <algorithm>
#include <iostream>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

template <std::ranges::view V>
class StrideView : public std::ranges::view_interface<StrideView<V>> {
public:
    StrideView() = default;
    StrideView(V base, std::ranges::range_difference_t<V> step)
        : base_(std::move(base)), step_(step) {}

    class Iterator {
    public:
        using BaseIterator = std::ranges::iterator_t<V>;
        using difference_type = std::ranges::range_difference_t<V>;
        using value_type = std::ranges::range_value_t<V>;

        Iterator() = default;
        Iterator(BaseIterator current, BaseIterator end, difference_type step)
            : current_(current), end_(end), step_(step) {}

        decltype(auto) operator*() const { return *current_; }

        Iterator& operator++() {
            current_ = std::ranges::next(current_, step_, end_);
            return *this;
        }

        bool operator==(std::default_sentinel_t) const {
            return current_ == end_;
        }

    private:
        BaseIterator current_{};
        BaseIterator end_{};
        difference_type step_{1};
    };

    Iterator begin() {
        return Iterator{std::ranges::begin(base_), std::ranges::end(base_), step_};
    }

    std::default_sentinel_t end() const {
        return {};
    }

private:
    V base_{};
    std::ranges::range_difference_t<V> step_{1};
};

template <std::ranges::viewable_range R>
auto stride(R&& range, std::ranges::range_difference_t<std::views::all_t<R>> step) {
    return StrideView<std::views::all_t<R>>{std::views::all(std::forward<R>(range)), step};
}

struct SensorReading {
    std::string id;
    double temperature;
    bool online;
};

int main() {
    std::vector<SensorReading> readings{
        {"A-01", 21.5, true},
        {"A-02", 18.0, false},
        {"B-01", 24.2, true},
        {"B-02", 27.8, true},
        {"C-01", 19.7, true},
        {"C-02", 31.1, true}
    };

    std::cout << "=== hot online sensors ===\n";
    auto hot_names = readings
        | std::views::filter([](const SensorReading& r) {
            return r.online && r.temperature >= 24.0;
        })
        | std::views::transform([](const SensorReading& r) {
            return r.id + "=" + std::to_string(r.temperature);
        });

    for (const auto& label : hot_names) {
        std::cout << label << '\n';
    }

    std::cout << "\n=== every second reading ===\n";
    for (const auto& reading : stride(readings, 2)) {
        std::cout << reading.id << ' ' << reading.temperature << '\n';
    }

    std::ranges::sort(readings, {}, &SensorReading::temperature);
    std::cout << "\n=== sorted by projection ===\n";
    for (const auto& reading : readings | std::views::take(3)) {
        std::cout << reading.id << ' ' << reading.temperature << '\n';
    }
}
