#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::cout << "=== 람다 함수 기본 ===\n\n";
    
    // 람다 함수 1: 제곱
    auto square = [](int x) -> int {
        return x * x;
    };
    std::cout << "5의 제곱: " << square(5) << "\n";
    
    // 람다 함수 2: 절댓값
    auto absolute = [](int x) -> int {
        return (x < 0) ? -x : x;
    };
    std::cout << "-10의 절댓값: " << absolute(-10) << "\n\n";
    
    // 캡처: 값으로 캡처 [=]
    std::cout << "=== 값으로 캡처 [=] ===\n";
    int multiplier = 3;
    auto multiply = [multiplier](int x) -> int {
        return x * multiplier;
    };
    std::cout << "7 * " << multiplier << " = " << multiply(7) << "\n";
    
    // multiplier를 변경해도 람다 함수 내부는 영향 없음
    multiplier = 10;
    std::cout << "multiplier를 10으로 변경 후 multiply(7) = " << multiply(7) << "\n";
    std::cout << "(캡처 값은 3으로 유지)\n\n";
    
    // 캡처: 참조로 캡처 [&]
    std::cout << "=== 참조로 캡처 [&] ===\n";
    int counter = 0;
    auto increment = [&counter]() -> void {
        counter++;
    };
    std::cout << "초기 counter: " << counter << "\n";
    increment();
    increment();
    increment();
    std::cout << "3번 증가 후 counter: " << counter << "\n\n";
    
    // 벡터에 람다 적용
    std::cout << "=== 벡터에 람다 적용 ===\n";
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    
    std::cout << "원본 벡터: ";
    for (int n : numbers) std::cout << n << " ";
    std::cout << "\n";
    
    // transform: 각 원소에 람다 적용
    std::vector<int> squared(numbers.size());
    std::transform(numbers.begin(), numbers.end(), squared.begin(),
                   [](int x) { return x * x; });
    
    std::cout << "제곱한 벡터: ";
    for (int n : squared) std::cout << n << " ";
    std::cout << "\n\n";
    
    // for_each: 각 원소 출력
    std::cout << "=== for_each로 출력 ===\n";
    std::for_each(numbers.begin(), numbers.end(),
                  [](int x) { std::cout << x << " "; });
    std::cout << "\n";
    
    // count_if: 조건 만족하는 개수
    int even_count = std::count_if(numbers.begin(), numbers.end(),
                                   [](int x) { return x % 2 == 0; });
    std::cout << "\n짝수 개수: " << even_count << "\n";
    
    return 0;
}
