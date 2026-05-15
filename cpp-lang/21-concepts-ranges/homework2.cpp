// 세션 21 — 확장 과제: Ranges & Views
// 적용 개념: std::ranges 알고리즘, std::views 어댑터, 파이프라인 체이닝
#include <iostream>
#include <vector>
#include <string>
#include <ranges>
#include <algorithm>
#include <numeric>

// ── 유틸리티 ─────────────────────────────────────────────────────────────────
template <std::ranges::range R>
void printRange(const std::string& label, R&& r) {
    std::cout << "  " << label << ": [";
    bool first = true;
    for (auto&& v : r) {
        if (!first) std::cout << ", ";
        std::cout << v;
        first = false;
    }
    std::cout << "]\n";
}

int main() {
    std::cout << "=== Ranges & Views 과제 ===\n\n";

    std::vector<int> nums = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};

    // ── 1. std::ranges 알고리즘 ───────────────────────────────────────────────
    std::cout << "[1] std::ranges 알고리즘\n";
    printRange("원본", nums);

    auto sorted = nums;
    std::ranges::sort(sorted);
    printRange("정렬", sorted);

    auto it = std::ranges::find(nums, 9);
    std::cout << "  find(9): " << (it != nums.end() ? "찾음" : "없음") << "\n";

    int cnt = static_cast<int>(std::ranges::count(nums, 5));
    std::cout << "  count(5): " << cnt << "\n";

    auto maxIt = std::ranges::max_element(nums);
    std::cout << "  max: " << *maxIt << "\n";

    // ── 2. std::views 어댑터 ─────────────────────────────────────────────────
    std::cout << "\n[2] std::views 어댑터\n";

    // filter: 짝수만
    auto evens = nums | std::views::filter([](int n){ return n % 2 == 0; });
    printRange("filter 짝수", evens);

    // transform: 제곱
    auto squares = nums | std::views::transform([](int n){ return n * n; });
    printRange("transform 제곱", squares);

    // take: 처음 4개
    auto first4 = nums | std::views::take(4);
    printRange("take(4)", first4);

    // drop: 앞 3개 건너뜀
    auto dropped = nums | std::views::drop(3);
    printRange("drop(3)", dropped);

    // reverse
    auto rev = nums | std::views::reverse;
    printRange("reverse", rev);

    // ── 3. 파이프라인 체이닝 ─────────────────────────────────────────────────
    std::cout << "\n[3] 파이프라인 체이닝\n";

    // 짝수 → 제곱 → 처음 3개
    auto pipeline1 = nums
        | std::views::filter([](int n){ return n % 2 == 0; })
        | std::views::transform([](int n){ return n * n; })
        | std::views::take(3);
    printRange("짝수→제곱→take(3)", pipeline1);

    // 5 초과 → 역순 → 처음 3개
    auto pipeline2 = nums
        | std::views::filter([](int n){ return n > 5; })
        | std::views::reverse
        | std::views::take(3);
    printRange(">5 → reverse → take(3)", pipeline2);

    // ── 4. std::views::iota (범위 생성) ──────────────────────────────────────
    std::cout << "\n[4] iota + 파이프라인\n";

    // 1~20 중 3의 배수
    auto multiples3 = std::views::iota(1, 21)
        | std::views::filter([](int n){ return n % 3 == 0; });
    printRange("1~20 중 3의 배수", multiples3);

    // 1~10 홀수의 합
    auto odds = std::views::iota(1, 11)
        | std::views::filter([](int n){ return n % 2 != 0; });
    int oddSum = std::accumulate(odds.begin(), odds.end(), 0);
    std::cout << "  1~10 홀수 합: " << oddSum << "\n";

    // ── 5. 문자열 ranges ──────────────────────────────────────────────────────
    std::cout << "\n[5] 문자열 Ranges\n";
    std::vector<std::string> words = {"apple", "banana", "cherry", "date", "elderberry"};
    printRange("원본", words);

    // 길이 5 초과
    auto longWords = words
        | std::views::filter([](const std::string& s){ return s.size() > 5; });
    printRange("길이>5", longWords);

    // 첫 글자만 변환
    auto initials = words
        | std::views::transform([](const std::string& s){ return s[0]; });
    printRange("첫 글자", initials);

    return 0;
}
