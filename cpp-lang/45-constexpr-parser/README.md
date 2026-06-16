# 세션 45: 컴파일 타임 정규 표현식 & 파서

## 학습 목표
1. `constexpr`/`consteval` 함수로 컴파일 타임 문자열 파싱을 구현한다
2. 유한 상태 기계(FSM)를 템플릿 메타프로그래밍으로 표현한다
3. 파서 컴비네이터 패턴(Seq, Alt, Many, Map)을 타입 수준에서 설계한다
4. 간단한 컴파일 타임 정규 표현식 매처를 `constexpr`로 구현한다
5. 런타임과 컴파일 타임 파서를 동일 코드로 작성하는 패턴을 이해한다

## 핵심 개념

| 개념 | 설명 |
|------|------|
| `constexpr` 파서 | 컴파일 타임에 실행 가능한 파싱 함수 |
| `consteval` | 반드시 컴파일 타임에만 실행되는 함수 |
| FSM (Finite State Machine) | 상태 전이 테이블로 정의한 인식기 |
| 파서 컴비네이터 | 작은 파서를 조합해 큰 파서를 만드는 FP 패턴 |
| `Seq<P1, P2>` | P1 다음 P2를 순서대로 인식 |
| `Alt<P1, P2>` | P1 또는 P2 중 하나를 인식 (선택) |
| `Many<P>` | P를 0회 이상 반복 인식 |
| `Map<P, F>` | P가 인식한 결과를 F로 변환 |
| `ParseResult<T>` | 파싱 결과(성공/실패)와 나머지 입력을 담는 타입 |

## 핵심 문법

```cpp
// constexpr 매처
constexpr bool match_digit(char c) { return c >= '0' && c <= '9'; }

// ParseResult 타입
template <typename T>
struct ParseResult {
    bool ok;
    T    value;
    std::string_view rest;
};

// 파서 컴비네이터 Seq
template <typename P1, typename P2>
struct Seq {
    static constexpr auto parse(std::string_view in) { ... }
};

// 컴파일 타임 검증
static_assert(match_integer("42abc").ok == true);
static_assert(match_integer("abc").ok   == false);
```

## 컴파일 명령어

```bash
g++ -std=c++23 -Wall -Wextra -Wpedantic homework.cpp  -o hw
g++ -std=c++23 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
g++ -std=c++23 -Wall -Wextra -Wpedantic game1.cpp     -o game
```
