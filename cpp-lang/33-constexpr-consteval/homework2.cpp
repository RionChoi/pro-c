// 세션 33 — 과제 2: 컴파일 타임 자료구조 & 메타프로그래밍
// FNV-1a 해시 · 문자열 유틸 · 버블 정렬 · 이진 탐색 · 정책 기반 누산기
// 컴파일: g++ -std=c++23 -Wall -Wextra -Wpedantic homework2.cpp -o hw33_2
// 엄격:   g++ -std=c++23 -Wall -Wextra -Wpedantic -Wshadow -Wconversion
//         -Wnull-dereference -Wdouble-promotion homework2.cpp -o hw33_2

#include <iostream>
#include <array>
#include <string_view>
#include <cstdint>
#include <algorithm>

// ── 1. 컴파일 타임 FNV-1a 해시 ───────────────────────────────────────────────
//
// 각 바이트를 XOR한 뒤 소수를 곱하는 비암호학적 해시.
// string_view를 받으므로 문자열 리터럴에 그대로 사용 가능하다.

constexpr std::uint32_t fnv1a(std::string_view sv) noexcept {
    std::uint32_t h = 2166136261u;
    for (char c : sv) {
        h ^= static_cast<std::uint32_t>(static_cast<unsigned char>(c));
        h *= 16777619u;
    }
    return h;
}

// ── 2. 컴파일 타임 문자열 유틸리티 ──────────────────────────────────────────

constexpr bool is_palindrome(std::string_view sv) noexcept {
    if (sv.empty()) return true;
    std::size_t lo = 0, hi = sv.size() - 1;
    while (lo < hi) {
        if (sv[lo] != sv[hi]) return false;
        ++lo; --hi;
    }
    return true;
}

constexpr int count_char(std::string_view sv, char target) noexcept {
    int cnt = 0;
    for (char c : sv)
        if (c == target) ++cnt;
    return cnt;
}

constexpr std::size_t sv_find(std::string_view haystack, std::string_view needle) noexcept {
    if (needle.empty()) return 0;
    if (needle.size() > haystack.size()) return std::string_view::npos;
    for (std::size_t i = 0; i + needle.size() <= haystack.size(); ++i) {
        bool match = true;
        for (std::size_t j = 0; j < needle.size(); ++j) {
            if (haystack[i + j] != needle[j]) { match = false; break; }
        }
        if (match) return i;
    }
    return std::string_view::npos;
}

// ── 3. 컴파일 타임 버블 정렬 ─────────────────────────────────────────────────

template<typename T, std::size_t N>
constexpr std::array<T, N> bubble_sort(std::array<T, N> arr) noexcept {
    for (std::size_t i = 0; i < N - 1; ++i)
        for (std::size_t j = 0; j < N - i - 1; ++j)
            if (arr[j] > arr[j + 1])
                std::swap(arr[j], arr[j + 1]);
    return arr;
}

// ── 4. 컴파일 타임 이진 탐색 ─────────────────────────────────────────────────

template<typename T, std::size_t N>
constexpr int binary_search_ct(const std::array<T, N>& arr, T target) noexcept {
    int lo = 0, hi = static_cast<int>(N) - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[static_cast<std::size_t>(mid)] == target) return mid;
        if (arr[static_cast<std::size_t>(mid)] <  target) lo = mid + 1;
        else                                               hi = mid - 1;
    }
    return -1;
}

// ── 5. 정책 기반 컴파일 타임 누산기 ──────────────────────────────────────────
//
// 정책 구조체에 apply() 메서드를 정의하면 accumulate_ct가 어떤 연산이든
// 컴파일 타임에 수행할 수 있다.

struct SumPolicy {
    constexpr double apply(double acc, double x) const noexcept { return acc + x; }
};
struct ProductPolicy {
    constexpr double apply(double acc, double x) const noexcept { return acc * x; }
};
struct MaxPolicy {
    constexpr double apply(double acc, double x) const noexcept {
        return acc < x ? x : acc;
    }
};
struct MinPolicy {
    constexpr double apply(double acc, double x) const noexcept {
        return acc > x ? x : acc;
    }
};

template<typename Policy, std::size_t N>
constexpr double accumulate_ct(const std::array<double, N>& arr,
                                Policy pol, double init) noexcept {
    double acc = init;
    for (double x : arr)
        acc = pol.apply(acc, x);
    return acc;
}

// ── 6. 컴파일 타임 GCD / LCM ─────────────────────────────────────────────────

constexpr long long gcd_ct(long long a, long long b) noexcept {
    while (b != 0) { long long t = b; b = a % b; a = t; }
    return a;
}

constexpr long long lcm_ct(long long a, long long b) noexcept {
    return (a / gcd_ct(a, b)) * b;
}

// ── main ─────────────────────────────────────────────────────────────────────

int main() {
    // 컴파일 타임 검증
    static_assert(fnv1a("hello") == fnv1a("hello"),   "해시 일관성");
    static_assert(fnv1a("hello") != fnv1a("world"),   "해시 충돌");

    static_assert(is_palindrome("racecar") == true);
    static_assert(is_palindrome("hello")   == false);
    static_assert(count_char("hello world", 'l') == 3);
    static_assert(sv_find("hello world", "world") == 6);
    static_assert(sv_find("hello", "xyz") == std::string_view::npos);

    constexpr std::array<int, 8> unsorted = {5, 3, 8, 1, 9, 2, 7, 4};
    constexpr std::array<int, 8> sorted   = bubble_sort(unsorted);
    static_assert(sorted[0] == 1 && sorted[7] == 9);

    static_assert(binary_search_ct(sorted, 7)  == 5);
    static_assert(binary_search_ct(sorted, 1)  == 0);
    static_assert(binary_search_ct(sorted, 10) == -1);

    constexpr std::array<double, 5> data = {1.0, 2.0, 3.0, 4.0, 5.0};
    static_assert(accumulate_ct(data, SumPolicy{},     0.0)  == 15.0);
    static_assert(accumulate_ct(data, ProductPolicy{}, 1.0)  == 120.0);
    static_assert(accumulate_ct(data, MaxPolicy{},     0.0)  == 5.0);
    static_assert(accumulate_ct(data, MinPolicy{},     1e9)  == 1.0);

    static_assert(gcd_ct(48, 18) == 6);
    static_assert(lcm_ct(4,  6)  == 12);

    // 출력
    std::cout << "=== 세션 33 과제2: 컴파일 타임 자료구조 & 메타프로그래밍 ===\n\n";

    std::cout << "[ 1. FNV-1a 컴파일 타임 해시 ]\n";
    for (std::string_view word : {"hello", "world", "C++23", "constexpr", "consteval"})
        std::cout << "  \"" << word << "\" -> " << fnv1a(word) << "\n";

    std::cout << "\n[ 2. 컴파일 타임 문자열 유틸리티 ]\n";
    for (std::string_view w : {"racecar", "level", "hello", "kayak", "noon"})
        std::cout << "  \"" << w << "\" 회문: " << (is_palindrome(w) ? "예" : "아니오") << "\n";
    std::cout << "  \"hello world\" 에서 'l' 개수: " << count_char("hello world", 'l') << "\n";
    std::cout << "  \"hello world\" 에서 \"world\" 위치: " << sv_find("hello world", "world") << "\n";

    std::cout << "\n[ 3. 컴파일 타임 버블 정렬 ]\n";
    std::cout << "  원본: ";
    for (int x : unsorted) std::cout << x << " ";
    std::cout << "\n  정렬: ";
    for (int x : sorted) std::cout << x << " ";
    std::cout << "\n";

    std::cout << "\n[ 4. 컴파일 타임 이진 탐색 (정렬된 배열) ]\n";
    for (int target : {1, 5, 7, 9, 10})
        std::cout << "  탐색(" << target << ") -> 인덱스 "
                  << binary_search_ct(sorted, target) << "\n";

    std::cout << "\n[ 5. 정책 기반 컴파일 타임 누산기 (1~5) ]\n";
    std::cout << "  합계:    " << accumulate_ct(data, SumPolicy{},     0.0) << "\n";
    std::cout << "  곱:      " << accumulate_ct(data, ProductPolicy{}, 1.0) << "\n";
    std::cout << "  최댓값: " << accumulate_ct(data, MaxPolicy{},     0.0) << "\n";
    std::cout << "  최솟값: " << accumulate_ct(data, MinPolicy{},     1e9) << "\n";

    std::cout << "\n[ 6. 컴파일 타임 GCD / LCM ]\n";
    std::cout << "  gcd(48, 18) = " << gcd_ct(48, 18) << "\n";
    std::cout << "  lcm(4,  6)  = " << lcm_ct(4,  6)  << "\n";
    std::cout << "  gcd(100, 75)= " << gcd_ct(100, 75) << "\n";
    std::cout << "  lcm(12, 18) = " << lcm_ct(12, 18)  << "\n";

    return 0;
}
