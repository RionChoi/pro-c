# 세션 31: std::variant & 타입 소거 (Type Erasure)

## 학습 목표

- `std::variant`로 타입 안전한 유니온을 구현한다.
- `std::visit`와 방문자 패턴(Visitor Pattern)으로 다형성을 표현한다.
- `std::holds_alternative`, `std::get`, `std::get_if`의 차이를 이해한다.
- `std::any`로 런타임 이종 컨테이너를 만든다.
- `Result<T, E>` 패턴으로 예외 없이 오류를 전파한다.
- `std::variant`를 상태 머신(State Machine)에 적용한다.

---

## 핵심 개념

### std::variant
여러 타입 중 하나를 담는 타입 안전 유니온.

```cpp
#include <variant>

using Shape = std::variant<Circle, Rectangle, Triangle>;

Shape s = Circle{5.0};
```

### std::visit
variant에 방문자 함수를 적용한다. `if-else` 없이 각 타입별 동작을 명확하게 분리한다.

```cpp
struct AreaVisitor {
    double operator()(const Circle& c)    const { return M_PI * c.r * c.r; }
    double operator()(const Rectangle& r) const { return r.w * r.h; }
};

double area = std::visit(AreaVisitor{}, shape);
```

제네릭 람다로도 작성 가능:

```cpp
std::visit([](const auto& v) {
    using T = std::decay_t<decltype(v)>;
    if constexpr (std::is_same_v<T, Circle>) { /* ... */ }
}, shape);
```

### std::holds_alternative / std::get / std::get_if

```cpp
if (std::holds_alternative<Circle>(s)) { /* Circle 타입임 */ }

Circle& c = std::get<Circle>(s);           // 타입 불일치 시 예외
Circle* p = std::get_if<Circle>(&s);       // 타입 불일치 시 nullptr
```

### Result<T, E> 패턴
예외 없이 오류를 함수 반환값으로 전달하는 현대적 패턴.

```cpp
template <typename T, typename E>
using Result = std::variant<T, E>;

Result<double, DivError> safe_div(double a, double b) {
    if (b == 0.0) return DivError{"0으로 나눌 수 없음"};
    return a / b;
}
```

### std::monostate
기본 상태(빈 값)를 variant에 포함할 때 사용한다.

```cpp
std::variant<std::monostate, int, std::string> v;  // 초기에는 monostate
```

---

## 파일 구성

| 파일 | 내용 |
|------|------|
| `homework.cpp`  | 도형 시스템 — Circle·Rectangle·Triangle variant, AreaVisitor/PerimeterVisitor |
| `homework2.cpp` | Result\<T,E\> 패턴 — safe_div, safe_sqrt, parse_int |
| `game1.cpp`     | 텍스트 RPG — MainMenu·Exploring·InCombat·GameOver 상태 머신 |

---

## 컴파일

```bash
g++ -std=c++23 -Wall -Wextra -Wpedantic homework.cpp  -o hw1  && ./hw1
g++ -std=c++23 -Wall -Wextra -Wpedantic homework2.cpp -o hw2  && ./hw2
g++ -std=c++23 -Wall -Wextra -Wpedantic game1.cpp     -o game && ./game
```

---

## 왜 variant인가?

| 방법 | 장점 | 단점 |
|------|------|------|
| 가상 함수 (동적 다형성) | 런타임 확장 가능 | 힙 할당, vtable 오버헤드 |
| `std::variant` (정적 다형성) | 스택 할당, 컴파일 타임 검사 | 열린 집합 불가 |
| `union` (C 스타일) | 가장 빠름 | 타입 안전 없음, UB 위험 |

`std::variant`는 **닫힌 타입 집합**에서 타입 안전을 보장하면서 가상 함수의 오버헤드를 피할 수 있는 현대적 방법이다.
