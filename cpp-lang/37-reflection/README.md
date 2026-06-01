# Session 37: 정적 리플렉션 & 고급 메타프로그래밍 (Static Reflection & Advanced Metaprogramming)

## 학습 목표
1. **타입 정보 추출**: 컴파일 타임에 타입 정보를 읽고 활용
2. **메타프로그래밍**: 템플릿을 사용한 강력한 프로그래밍 기법
3. **코드 제너레이션**: 컴파일 시간에 코드를 자동 생성
4. **구조체 분석**: 멤버 정보를 런타임에 활용

## 핵심 개념

### 리플렉션 (Reflection)
- **정적 리플렉션**: 컴파일 타임에 타입 정보 분석
- **동적 리플렉션**: 런타임에 타입 정보 분석
- **C++에서의 리플렉션**: boost::pfr, 템플릿, 매크로

### 메타프로그래밍 패턴
```cpp
// Type traits
template <typename T>
struct is_integral : std::false_type {};

template <>
struct is_integral<int> : std::true_type {};

// SFINAE (Substitution Failure Is Not An Error)
template <typename T>
typename std::enable_if<std::is_integral<T>::value>::type
process(T value) { /* 정수 타입만 허용 */ }

// 템플릿 특수화
template <typename T>
struct Printer { static void print() { /* ... */ } };

template <>
struct Printer<int> { static void print() { /* 정수 특화 */ } };
```

### 구조체 필드 열거
```cpp
struct Person {
    std::string name;
    int age;
    double salary;
};

// pf::tuple_cat를 사용한 자동 직렬화
auto as_tuple(const Person& p) {
    return std::make_tuple(p.name, p.age, p.salary);
}
```

### 매크로를 통한 코드 생성
```cpp
#define SERIALIZABLE(struct_name, ...) \
    struct struct_name { \
        __VA_ARGS__ \
    };
```

## 고급 기법

### 1. CRTP (Curiously Recurring Template Pattern)
```cpp
template <typename Derived>
class Base {
public:
    void print() {
        static_cast<Derived*>(this)->impl();
    }
};

class Derived : public Base<Derived> {
public:
    void impl() { /* ... */ }
};
```

### 2. Concepts & Constraints
```cpp
template <typename T>
concept Printable = requires(T t, std::ostream& os) {
    { os << t } -> std::convertible_to<std::ostream&>;
};

template <Printable T>
void print(const T& value) {
    std::cout << value << '\n';
}
```

### 3. 가변 템플릿 (Variadic Templates)
```cpp
template <typename... Args>
void print_all(Args... args) {
    (std::cout << ... << args);
}
```

## 과제 구성

| 파일 | 내용 |
|------|------|
| `homework.cpp` | CRTP를 사용한 비트맵 렌더러 |
| `homework2.cpp` | 가변 템플릿을 이용한 타입-안전 포맷 함수 |
| `game1.cpp` | 메타프로그래밍으로 구현한 카드 게임 |

## 컴파일 & 실행

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp -o hw
./hw

g++ -std=c++17 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
./hw2

g++ -std=c++17 -Wall -Wextra -Wpedantic game1.cpp -o game
./game
```

## 주의사항
- 컴파일 시간 증가: 템플릿 코드는 컴파일이 느릴 수 있음
- 오류 메시지: 템플릿 오류는 복잡하고 긴 스택 트레이스 생성
- 이식성: C++17 이상 필수, 컴파일러 의존성 있음
- 코드 가독성: 메타프로그래밍은 과용하지 않기
