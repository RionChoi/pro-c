# 세션 23 — C++23 신기능

## 학습 목표
- `std::expected<T,E>`로 예외 없는 오류 처리를 구현한다.
- `std::views::zip` / `enumerate`로 다중 범위를 동시에 순회한다.
- `if consteval`, `[[assume]]` 컴파일 타임 기능을 익힌다.
- 다차원 `operator[]`, `std::string::contains`, `std::to_underlying` 등 소형 신기능을 활용한다.

## GCC 13 지원 현황

| 기능 | 지원 |
|------|------|
| `std::expected` | ✅ |
| `std::views::zip` / `enumerate` | ✅ |
| `if consteval` | ✅ |
| `[[assume(...)]]` | ✅ |
| 다차원 `operator[](int, int)` | ✅ |
| `std::string::contains` | ✅ |
| `std::to_underlying` | ✅ |
| `std::print` / `std::println` | ❌ (GCC 14+) |
| `std::flat_map` | ❌ (GCC 14+) |
| Deducing this | ❌ (GCC 14+) |

## 핵심 개념

### std::expected
```cpp
#include <expected>

std::expected<int, std::string> divide(int a, int b) {
    if (b == 0) return std::unexpected("0으로 나눌 수 없음");
    return a / b;
}

auto r = divide(10, 2);
if (r) std::cout << *r;        // 성공: 값 접근
else   std::cout << r.error(); // 실패: 오류 접근
```

### std::views::zip / enumerate
```cpp
std::vector<int> v1 = {1, 2, 3};
std::vector<int> v2 = {4, 5, 6};

for (auto [a, b] : std::views::zip(v1, v2))
    std::cout << a << "+" << b;

for (auto [i, val] : std::views::enumerate(v1))
    std::cout << i << ":" << val;
```

## 과제
| 파일 | 내용 |
|------|------|
| `homework.cpp` | `std::expected` — 예외 없는 오류 처리 |
| `homework2.cpp` | C++23 범위/언어 신기능 종합 |
| `game1.cpp` | 던전 스킬 게임 — `std::expected` 체인 |

## 컴파일
```bash
g++ -std=c++23 -Wall -Wextra -Wpedantic homework.cpp  -o hw
g++ -std=c++23 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
g++ -std=c++23 -Wall -Wextra -Wpedantic game1.cpp     -o game
```
