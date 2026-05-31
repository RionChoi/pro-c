# Session 37: 정적 리플렉션 & 고급 메타프로그래밍

## 학습 목표
- 컴파일 타임 타입 정보 접근 및 조작 (Static Reflection)
- SFINAE, 개념(Concepts)을 활용한 고급 메타프로그래밍
- `std::enable_if`, `std::is_*`, 타입 특성(Type Traits) 심화
- 리플렉션 기반 직렬화, 출력 생성 등 실용적 활용

## 핵심 개념

### 1. Type Traits와 Type 검사
```cpp
template<typename T>
struct is_floating_point {
    static constexpr bool value = std::is_floating_point_v<T>;
};

// std::enable_if를 이용한 조건부 함수 오버로딩
template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
void process_int(T val) { }
```

### 2. SFINAE (Substitution Failure Is Not An Error)
```cpp
// detect_callable: 호출 가능 검사
template<typename T>
decltype(std::declval<T>()()) test(int);  // SFINAE 활용

// 후보(candidate)가 치환 실패하면 다른 오버로드 시도
```

### 3. C++20 Concepts (개념)
```cpp
template<typename T>
concept Integral = std::is_integral_v<T>;

template<Integral T>
void handle(T value) { }  // 정수 타입만 받음
```

### 4. std::tuple과 Variadic 템플릿 반복
```cpp
template<typename... Args>
void iterate_tuple(std::tuple<Args...>& t) {
    std::apply([](auto&... args) {
        // 각 멤버에 대해 작업
    }, t);
}
```

### 5. 리플렉션 활용: 자동 직렬화
```cpp
struct Point { int x, y; };
// 템플릿으로 멤버 개수 계산
template<typename T>
constexpr auto member_count() {
    if constexpr (is_two_member_struct<T>::value)
        return 2;
    // ...
}
```

## 파일 구성

| 파일 | 내용 |
|------|------|
| `homework.cpp` | Type Traits와 SFINAE를 이용한 다형성 |
| `homework2.cpp` | C++20 Concepts와 개념 기반 디자인 |
| `game1.cpp` | 타입 반영 기반 게임 객체 직렬화기 |

## 컴파일
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp -o hw37
g++ -std=c++20 -Wall -Wextra -Wpedantic homework2.cpp -o hw37_2
g++ -std=c++17 -Wall -Wextra -Wpedantic game1.cpp -o game37
```
