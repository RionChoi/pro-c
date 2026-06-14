// Session 44: C++20 Ranges 심화 — 커스텀 View & Range Adaptor 파이프라인
#include <ranges>
#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <cmath>

// ─────────────────────────────────────────────
// [1] 표준 range adaptor 심화 활용
// ─────────────────────────────────────────────

void demo_standard_adaptors() {
    std::cout << "=== [1] 표준 Range Adaptor 심화 ===\n";

    std::vector<int> nums{1,2,3,4,5,6,7,8,9,10};

    // filter + transform + take 파이프라인
    auto pipeline = nums
        | std::views::filter([](int n){ return n % 2 == 0; })
        | std::views::transform([](int n){ return n * n; })
        | std::views::take(3);

    std::cout << "  짝수의 제곱 (앞 3개): ";
    for (int v : pipeline) std::cout << v << ' ';
    std::cout << '\n';

    // iota — 무한 수열에서 피보나치 인덱스 추출
    auto squares = std::views::iota(1)
        | std::views::transform([](int n){ return n * n; })
        | std::views::take(8);

    std::cout << "  1~8의 제곱(iota): ";
    for (int v : squares) std::cout << v << ' ';
    std::cout << '\n';

    // reverse + drop
    auto tail = nums | std::views::reverse | std::views::drop(3);
    std::cout << "  뒤에서 3개 제외 후 역순: ";
    for (int v : tail) std::cout << v << ' ';
    std::cout << '\n';

    // join — 중첩 range 펼치기
    std::vector<std::vector<int>> nested{{1,2},{3,4},{5,6}};
    std::cout << "  join: ";
    for (int v : nested | std::views::join) std::cout << v << ' ';
    std::cout << '\n';

    // split — 문자열 분리
    std::string csv = "apple,banana,cherry,date";
    std::cout << "  split by ',': ";
    for (auto part : csv | std::views::split(',')) {
        std::cout << std::string_view(part) << ' ';
    }
    std::cout << "\n\n";
}

// ─────────────────────────────────────────────
// [2] enumerate & zip (C++23)
// ─────────────────────────────────────────────

void demo_enumerate_zip() {
    std::cout << "=== [2] enumerate & zip (C++23) ===\n";

    std::vector<std::string> fruits{"사과", "바나나", "체리", "대추"};
    std::vector<int>         prices{1200,   800,     2500,   600};

    // enumerate
    std::cout << "  enumerate:\n";
    for (auto [i, fruit] : fruits | std::views::enumerate) {
        std::cout << "    [" << i << "] " << fruit << '\n';
    }

    // zip
    std::cout << "  zip (과일 + 가격):\n";
    for (auto [fruit, price] : std::views::zip(fruits, prices)) {
        std::cout << "    " << fruit << ": " << price << "원\n";
    }
    std::cout << '\n';
}

// ─────────────────────────────────────────────
// [3] 커스텀 view — StridedView
// ─────────────────────────────────────────────

// N 간격으로 원소를 선택하는 view
template <std::ranges::view V>
class StridedView : public std::ranges::view_interface<StridedView<V>> {
    V base_;
    std::ranges::range_difference_t<V> stride_;

    class Iterator {
        std::ranges::iterator_t<V> cur_;
        std::ranges::sentinel_t<V> end_;
        std::ranges::range_difference_t<V> stride_;
    public:
        using value_type        = std::ranges::range_value_t<V>;
        using difference_type   = std::ranges::range_difference_t<V>;
        using iterator_category = std::input_iterator_tag;

        Iterator() = default;
        Iterator(std::ranges::iterator_t<V> cur,
                 std::ranges::sentinel_t<V> end,
                 difference_type stride)
            : cur_(cur), end_(end), stride_(stride) {}

        value_type operator*() const { return *cur_; }

        Iterator& operator++() {
            for (difference_type i = 0; i < stride_ && cur_ != end_; ++i)
                ++cur_;
            return *this;
        }
        Iterator operator++(int) { auto tmp = *this; ++*this; return tmp; }

        bool operator==(std::default_sentinel_t) const { return cur_ == end_; }
    };

public:
    StridedView() = default;
    StridedView(V base, std::ranges::range_difference_t<V> stride)
        : base_(std::move(base)), stride_(stride) {}

    auto begin() { return Iterator(std::ranges::begin(base_),
                                   std::ranges::end(base_), stride_); }
    auto end()   { return std::default_sentinel; }
};

// range adaptor closure
struct StridedAdaptorClosure {
    std::ptrdiff_t stride;

    template <std::ranges::viewable_range R>
    auto operator()(R&& r) const {
        return StridedView(std::views::all(std::forward<R>(r)), stride);
    }
};

auto strided(std::ptrdiff_t n) {
    return StridedAdaptorClosure{n};
}

void demo_custom_view() {
    std::cout << "=== [3] 커스텀 StridedView ===\n";

    std::vector<int> v{0,1,2,3,4,5,6,7,8,9,10,11};

    std::cout << "  stride(1): ";
    for (int x : v | strided(1)) std::cout << x << ' ';
    std::cout << '\n';

    std::cout << "  stride(3): ";
    for (int x : v | strided(3)) std::cout << x << ' ';
    std::cout << '\n';

    std::cout << "  stride(4): ";
    for (int x : v | strided(4)) std::cout << x << ' ';
    std::cout << '\n';

    // 파이프라인과 결합
    auto pipeline = v
        | std::views::filter([](int n){ return n % 2 == 0; })
        | strided(2);

    std::cout << "  짝수 | stride(2): ";
    for (int x : pipeline) std::cout << x << ' ';
    std::cout << "\n\n";
}

// ─────────────────────────────────────────────
// [4] projection — 알고리즘 + 프로젝션
// ─────────────────────────────────────────────

struct Student {
    std::string name;
    int score;
    double gpa;
};

void demo_projection() {
    std::cout << "=== [4] Projection 활용 ===\n";

    std::vector<Student> students{
        {"Alice", 92, 3.8},
        {"Bob",   78, 3.1},
        {"Carol", 88, 3.5},
        {"Dave",  95, 3.9},
        {"Eve",   71, 2.9},
    };

    // 점수 기준 정렬
    std::ranges::sort(students, std::less<>{}, &Student::score);
    std::cout << "  점수순 정렬:\n";
    for (auto& s : students)
        std::cout << "    " << s.name << ": " << s.score << '\n';

    // max 찾기 (GPA 기준)
    auto top = std::ranges::max_element(students, {}, &Student::gpa);
    std::cout << "  최고 GPA: " << top->name << " (" << top->gpa << ")\n";

    // 조건 검색
    auto found = std::ranges::find_if(students, [](int s){ return s >= 90; },
                                      &Student::score);
    if (found != students.end())
        std::cout << "  점수 90+ 첫 번째: " << found->name << '\n';

    // transform_view + projection 으로 이름 추출
    std::cout << "  이름 목록: ";
    for (auto& name : students | std::views::transform(&Student::name))
        std::cout << name << ' ';
    std::cout << "\n\n";
}

// ─────────────────────────────────────────────
// [5] lazy evaluation 성능 비교
// ─────────────────────────────────────────────

void demo_lazy_vs_eager() {
    std::cout << "=== [5] Lazy Evaluation ===\n";

    constexpr int N = 1'000'000;

    // lazy: 10개만 실제로 계산됨
    auto lazy_result = std::views::iota(1, N + 1)
        | std::views::filter([](int n){ return n % 7 == 0; })
        | std::views::transform([](int n){ return n * 2; })
        | std::views::take(10);

    std::cout << "  iota(1~1M) | 7의 배수 | *2 | take(10) — lazy:\n  ";
    for (int v : lazy_result) std::cout << v << ' ';
    std::cout << '\n';

    // ranges::fold_left — 합산
    auto sum_range = std::views::iota(1, 101);
    auto sum = std::ranges::fold_left(sum_range, 0, std::plus<>{});
    std::cout << "  fold_left 1~100 합계: " << sum << '\n';
    std::cout << '\n';
}

int main() {
    demo_standard_adaptors();
    demo_enumerate_zip();
    demo_custom_view();
    demo_projection();
    demo_lazy_vs_eager();

    std::cout << "=== 세션 44 완료: C++20 Ranges 심화 ===\n";
    return 0;
}
