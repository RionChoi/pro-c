# 세션 41: C++20 Concepts 심화

## 학습 목표
1. `requires`-expression 의 4가지 형태(simple, type, compound, nested)를 이해한다
2. 중첩 제약(nested constraints)과 제약 결합(`&&`, `||`)을 활용한다
3. 커스텀 Concept 라이브러리를 설계하고 재사용한다
4. Concept을 이용한 조건부 멤버 함수 활성화 패턴을 익힌다
5. `std::` Concepts(`std::integral`, `std::totally_ordered` 등)를 조합한다

## 핵심 개념

| 개념 | 설명 |
|------|------|
| `concept` | 타입에 대한 컴파일 타임 불리언 제약 |
| `requires` 절 | 템플릿 파라미터에 제약을 추가하는 키워드 |
| requires-expression | `requires { ... }` 형태로 표현식 유효성을 검사 |
| simple requirement | `expr;` — 표현식이 유효한지 확인 |
| type requirement | `typename T::type;` — 타입이 존재하는지 확인 |
| compound requirement | `{ expr } -> concept;` — 표현식 + 반환 타입 검사 |
| nested requirement | `requires constraint;` — 내부에서 다시 제약 검사 |
| 제약 conjunction | `C1 && C2` — 두 Concept 모두 만족 |
| 제약 disjunction | `C1 \|\| C2` — 둘 중 하나 만족 |
| Concept 부분 순서 | 더 구체적인 Concept이 오버로드 해소 시 우선 선택됨 |

## 핵심 문법

```cpp
// 기본 Concept 정의
template <typename T>
concept Printable = requires(T t) {
    { std::cout << t } -> std::same_as<std::ostream&>;
};

// 중첩 requires
template <typename T>
concept Container = requires(T c) {
    typename T::value_type;
    { c.begin() } -> std::input_iterator;
    { c.size()  } -> std::convertible_to<std::size_t>;
    requires std::default_initializable<typename T::value_type>;
};

// 제약 결합
template <typename T>
concept SortableContainer = Container<T> && requires(T c) {
    { c[0] } -> std::totally_ordered;
};
```

## 컴파일 명령어

```bash
g++ -std=c++23 -Wall -Wextra -Wpedantic homework.cpp  -o hw
g++ -std=c++23 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
g++ -std=c++23 -Wall -Wextra -Wpedantic game1.cpp     -o game
```
