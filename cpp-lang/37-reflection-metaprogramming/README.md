# Session 37: 정적 리플렉션 & 고급 메타프로그래밍 (Static Reflection & Advanced Metaprogramming)

## 학습 목표
- C++17 `if constexpr`을 활용한 컴파일 타임 분기
- 템플릿 메타프로그래밍(TMP) 고급 기법
- 타입 특성(Type Traits) 작성 및 활용
- 간단한 정적 리플렉션 구현
- Concept와 `requires` 절을 활용한 타입 제약

## 핵심 개념

### 1. if constexpr (C++17)
컴파일 타임에 분기하여 불필요한 코드는 제거:

```cpp
template<typename T>
void process(T value) {
    if constexpr (std::is_integral_v<T>) {
        std::cout << "Integer: " << value << '\n';
    } else if constexpr (std::is_floating_point_v<T>) {
        std::cout << "Float: " << value << '\n';
    }
}
```

### 2. 커스텀 Type Traits
메타함수로 타입 정보를 컴파일 타임에 분석:

```cpp
template<typename T>
struct is_pointer_to_int : std::false_type {};

template<>
struct is_pointer_to_int<int*> : std::true_type {};

template<typename T>
inline constexpr bool is_pointer_to_int_v = is_pointer_to_int<T>::value;
```

### 3. SFINAE (Substitution Failure Is Not An Error)
오버로드 해석 중 타입 불일치는 오류가 아닌 후보 제거:

```cpp
template<typename T>
std::enable_if_t<std::is_arithmetic_v<T>, T>
add(T a, T b) {
    return a + b;
}
```

### 4. 간단한 정적 리플렉션
구조체 멤버 개수 및 타입을 컴파일 타임에 파악:

```cpp
template<typename T>
struct struct_size;

template<>
struct struct_size<Point> {
    static constexpr size_t value = 3; // x, y, z
};
```

### 5. Concept & requires (C++20)
제네릭 코드에 명확한 요구사항 명시:

```cpp
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

template<Numeric T>
T maximum(T a, T b) {
    return a > b ? a : b;
}
```

## 학습 파일

| 파일 | 내용 |
|------|------|
| `homework.cpp` | 기본 if constexpr와 Type Traits |
| `homework2.cpp` | 고급 메타프로그래밍: 튜플 가공 및 리플렉션 |
| `game1.cpp` | 메타프로그래밍 활용 게임: 타입 안전 인벤토리 |

---

## 참고
- `<type_traits>`: 표준 타입 특성
- `std::enable_if`, `std::enable_if_t`: SFINAE 유틸
- `if constexpr`: 컴파일 타임 조건부 실행
- `requires`: 컨셉 제약(C++20)
