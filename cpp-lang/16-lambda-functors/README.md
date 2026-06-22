# C++-16: 람다 & 함수 객체 (Lambda & Functors)

## 학습 목표
1. 람다 함수 (Lambda Functions) 문법과 캡처 (Capture)
2. 함수 객체 (Functors / Function Objects)
3. std::function으로 함수 포인터 일반화
4. 알고리즘과 함께 람다 사용 (std::sort, std::for_each 등)
5. 람다의 상태 관리와 캡처 메커니즘

## 핵심 개념

### 람다 함수 기본 문법
```cpp
// [캡처](매개변수) -> 반환타입 { 본체 };
auto square = [](int x) -> int { return x * x; };
int result = square(5);  // 25
```

### 캡처 (Capture)
- `[=]` — 값으로 캡처 (복사)
- `[&]` — 참조로 캡처
- `[x, &y]` — x는 값, y는 참조로 캡처

### 함수 객체 (Functor)
```cpp
struct Multiplier {
    int factor;
    Multiplier(int f) : factor(f) {}
    int operator()(int x) const {
        return x * factor;
    }
};
Multiplier mul(3);
int result = mul(5);  // 15
```

### std::function
```cpp
#include <functional>
std::function<int(int)> func;
func = [](int x) { return x * 2; };
int result = func(10);  // 20
```

### 알고리즘과 람다
```cpp
std::vector<int> v = {3, 1, 4, 1, 5};
std::sort(v.begin(), v.end(), 
    [](int a, int b) { return a > b; });  // 내림차순
```

## 과제
- **homework.cpp**: 기본 람다 함수, 캡처 메커니즘
- **homework2.cpp**: 함수 객체와 std::function 활용
- **game1.cpp**: 람다와 알고리즘을 활용한 간단한 게임 로직
