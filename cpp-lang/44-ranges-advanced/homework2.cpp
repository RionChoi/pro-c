// Session 44: Ranges 심화 — 커스텀 무한 Generator View & 파이프라인 최적화
#include <ranges>
#include <algorithm>
#include <vector>
#include <iostream>
#include <numeric>
#include <cmath>

// ─────────────────────────────────────────────
// 커스텀 무한 Generator View (소수 생성기)
// ─────────────────────────────────────────────

class PrimesView : public std::ranges::view_interface<PrimesView> {
    struct Iterator {
        using value_type        = int;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        int current_ = 2;

        Iterator() = default;
        explicit Iterator(int start) : current_(start) {}

        int operator*() const { return current_; }

        Iterator& operator++() {
            ++current_;
            while (!is_prime(current_)) ++current_;
            return *this;
        }
        Iterator operator++(int) { auto tmp = *this; ++*this; return tmp; }
        bool operator==(std::default_sentinel_t) const noexcept { return false; }

    private:
        static bool is_prime(int n) {
            if (n < 2) return false;
            if (n == 2) return true;
            if (n % 2 == 0) return false;
            for (int i = 3; i <= static_cast<int>(std::sqrt(static_cast<double>(n))); i += 2)
                if (n % i == 0) return false;
            return true;
        }
    };

public:
    auto begin() const { return Iterator{2}; }
    auto end()   const { return std::default_sentinel; }
};

// ─────────────────────────────────────────────
// 커스텀 Fibonacci Generator View
// ─────────────────────────────────────────────

class FibView : public std::ranges::view_interface<FibView> {
    struct Iterator {
        using value_type        = long long;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        long long a_ = 0, b_ = 1;

        long long operator*() const { return a_; }
        Iterator& operator++() {
            auto next = a_ + b_;
            a_ = b_;
            b_ = next;
            return *this;
        }
        Iterator operator++(int) { auto tmp = *this; ++*this; return tmp; }
        bool operator==(std::default_sentinel_t) const noexcept { return false; }
    };

public:
    auto begin() const { return Iterator{}; }
    auto end()   const { return std::default_sentinel; }
};

// ─────────────────────────────────────────────
// Window Sliding View — 연속 N개 원소의 창
// ─────────────────────────────────────────────

template <std::ranges::forward_range V, int W>
class WindowView : public std::ranges::view_interface<WindowView<V, W>> {
    V base_;

    struct Iterator {
        std::ranges::iterator_t<V> cur_;
        std::ranges::sentinel_t<V> end_;

        using value_type        = std::array<std::ranges::range_value_t<V>, W>;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        value_type operator*() const {
            value_type win{};
            auto it = cur_;
            for (int i = 0; i < W && it != end_; ++i, ++it)
                win[static_cast<std::size_t>(i)] = *it;
            return win;
        }

        Iterator& operator++() { ++cur_; return *this; }
        Iterator operator++(int) { auto tmp = *this; ++*this; return tmp; }
        bool operator==(std::default_sentinel_t) const {
            auto it = cur_;
            for (int i = 0; i < W; ++i, ++it)
                if (it == end_) return true;
            return false;
        }
    };

public:
    WindowView() = default;
    explicit WindowView(V base) : base_(std::move(base)) {}

    auto begin() { return Iterator{std::ranges::begin(base_), std::ranges::end(base_)}; }
    auto end()   { return std::default_sentinel; }
};

template <int W>
struct WindowAdaptor : std::ranges::range_adaptor_closure<WindowAdaptor<W>> {
    template <std::ranges::viewable_range R>
    auto operator()(R&& r) const {
        return WindowView<std::views::all_t<R>, W>(std::views::all(std::forward<R>(r)));
    }
};

template <int W>
inline constexpr WindowAdaptor<W> window{};

// ─────────────────────────────────────────────
// main
// ─────────────────────────────────────────────

int main() {
    std::cout << "=== 무한 소수 Generator ===\n";

    // 처음 15개 소수
    std::cout << "  처음 15개 소수: ";
    for (int p : PrimesView{} | std::views::take(15))
        std::cout << p << ' ';
    std::cout << '\n';

    // 100 이상 첫 10개 소수
    std::cout << "  100 이상 첫 10개: ";
    for (int p : PrimesView{}
                 | std::views::drop_while([](int n){ return n < 100; })
                 | std::views::take(10))
        std::cout << p << ' ';
    std::cout << '\n';

    // 쌍둥이 소수 (p, p+2) — zip 활용
    std::cout << "  쌍둥이 소수 (처음 5쌍): ";
    int count = 0;
    for (int p : PrimesView{}) {
        if (count >= 5) break;
        // 소수 범위에서 p+2도 소수인지 직접 확인
        auto is_prime = [](int n) {
            if (n < 2) return false;
            if (n == 2) return true;
            if (n % 2 == 0) return false;
            for (int i = 3; i <= static_cast<int>(std::sqrt(static_cast<double>(n))); i += 2)
                if (n % i == 0) return false;
            return true;
        };
        if (is_prime(p + 2)) {
            std::cout << "(" << p << "," << p+2 << ") ";
            ++count;
        }
    }
    std::cout << '\n';

    std::cout << "\n=== 피보나치 Generator ===\n";

    std::cout << "  처음 12개: ";
    for (long long v : FibView{} | std::views::take(12))
        std::cout << v << ' ';
    std::cout << '\n';

    // 1000 이하 짝수 피보나치 합
    auto fib_even_sum = std::ranges::fold_left(
        FibView{}
            | std::views::take_while([](long long v){ return v <= 1000; })
            | std::views::filter([](long long v){ return v % 2 == 0; }),
        0LL, std::plus<long long>{}
    );
    std::cout << "  1000 이하 짝수 합: " << fib_even_sum << '\n';

    std::cout << "\n=== Sliding Window View (W=3) ===\n";

    std::vector<int> data{1, 2, 3, 4, 5, 6, 7};
    std::cout << "  data: ";
    for (int v : data) std::cout << v << ' ';
    std::cout << '\n';

    std::cout << "  window<3> 합계: ";
    for (auto win : data | window<3>) {
        int sum = 0;
        for (int v : win) sum += v;
        std::cout << sum << ' ';
    }
    std::cout << '\n';

    std::cout << "  window<3> (filter sum>10): ";
    for (auto win : data | window<3>) {
        int sum = 0;
        for (int v : win) sum += v;
        if (sum > 10) {
            std::cout << '[';
            for (int v : win) std::cout << v << ' ';
            std::cout << "\b] ";
        }
    }
    std::cout << '\n';

    std::cout << "\n=== 복합 파이프라인: 소수 × 피보나치 공통 원소 ===\n";
    // 1~100에서 소수이면서 피보나치 수인 것
    std::vector<int> fib_vals;
    for (long long v : FibView{} | std::views::take_while([](long long x){ return x <= 100; }))
        fib_vals.push_back(static_cast<int>(v));

    std::cout << "  피보나치 소수(100이하): ";
    for (int p : PrimesView{}
                 | std::views::take_while([](int n){ return n <= 100; })) {
        if (std::ranges::find(fib_vals, p) != fib_vals.end())
            std::cout << p << ' ';
    }
    std::cout << '\n';

    return 0;
}
