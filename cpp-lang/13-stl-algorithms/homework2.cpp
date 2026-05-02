#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>  // std::accumulate, std::iota
#include <string>

int main() {
    // 학습: std::transform, std::accumulate, std::copy_if 고급 사용

    std::cout << "=== STL 알고리즘 고급 ===\n\n";

    // 1. std::transform — 각 원소 변환
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<int> squared(numbers.size());

    std::transform(numbers.begin(), numbers.end(), squared.begin(),
                   [](int x) { return x * x; });

    std::cout << "원본: ";
    for (int num : numbers) {
        std::cout << num << " ";
    }
    std::cout << "\n제곱값: ";
    for (int num : squared) {
        std::cout << num << " ";
    }
    std::cout << "\n\n";

    // 2. std::accumulate — 합계 계산
    int sum = std::accumulate(numbers.begin(), numbers.end(), 0);
    std::cout << "합계: " << sum << "\n";

    int product = std::accumulate(numbers.begin(), numbers.end(), 1,
                                  [](int a, int b) { return a * b; });
    std::cout << "곱: " << product << "\n\n";

    // 3. std::copy_if — 조건부 복사
    std::vector<int> evens;
    std::copy_if(numbers.begin(), numbers.end(), std::back_inserter(evens),
                 [](int num) { return num % 2 == 0; });

    std::cout << "짝수만 필터링: ";
    for (int num : evens) {
        std::cout << num << " ";
    }
    std::cout << "\n\n";

    // 4. std::all_of, std::any_of, std::none_of — 조건 검사
    bool all_positive = std::all_of(numbers.begin(), numbers.end(),
                                    [](int num) { return num > 0; });
    std::cout << "모두 양수인가? " << (all_positive ? "예" : "아니오") << "\n";

    bool any_greater_5 = std::any_of(numbers.begin(), numbers.end(),
                                     [](int num) { return num > 5; });
    std::cout << "5보다 큰 수가 있는가? " << (any_greater_5 ? "예" : "아니오") << "\n";

    bool none_negative = std::none_of(numbers.begin(), numbers.end(),
                                      [](int num) { return num < 0; });
    std::cout << "음수가 없는가? " << (none_negative ? "예" : "아니오") << "\n\n";

    // 5. std::fill — 벡터 채우기
    std::vector<int> filled(10);
    std::fill(filled.begin(), filled.end(), 42);

    std::cout << "42로 채운 벡터: ";
    for (int num : filled) {
        std::cout << num << " ";
    }
    std::cout << "\n\n";

    // 6. std::generate — 생성 함수로 채우기
    std::vector<int> generated(10);
    int counter = 1;
    std::generate(generated.begin(), generated.end(),
                  [&counter]() { return counter++; });

    std::cout << "생성된 순번: ";
    for (int num : generated) {
        std::cout << num << " ";
    }
    std::cout << "\n";

    return 0;
}
