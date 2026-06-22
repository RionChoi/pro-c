# 세션 21 — Concepts & Ranges (C++20)

## 학습 목표
- `concept` 키워드로 템플릿 제약 조건을 명시적으로 표현한다.
- `requires` 절과 requires 표현식으로 세밀한 제약을 건다.
- `std::ranges` 알고리즘과 `std::views` 어댑터로 파이프라인을 구성한다.
- 파이프 연산자 `|`로 범위 변환을 체이닝한다.

## 핵심 개념

### Concepts
```cpp
// concept 정의
template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

// 제약 함수 (requires 절)
template <typename T>
requires Numeric<T>
T square(T x) { return x * x; }

// 약식 함수 템플릿
Numeric auto square(Numeric auto x) { return x * x; }
```

### Ranges & Views
```cpp
#include <ranges>
#include <algorithm>

std::vector<int> v = {1, 2, 3, 4, 5, 6};

// 파이프라인: 짝수 필터 → 제곱 변환 → 처음 3개
auto result = v
    | std::views::filter([](int n){ return n % 2 == 0; })
    | std::views::transform([](int n){ return n * n; })
    | std::views::take(3);
```

## 과제
| 파일 | 내용 |
|------|------|
| `homework.cpp` | 커스텀 Concepts 정의 및 적용 |
| `homework2.cpp` | Ranges / Views 파이프라인 |
| `game1.cpp` | 숫자 퀴즈 게임 — Concepts + Ranges 응용 |

## 컴파일
```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic homework.cpp -o hw
g++ -std=c++20 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
g++ -std=c++20 -Wall -Wextra -Wpedantic game1.cpp -o game
```
