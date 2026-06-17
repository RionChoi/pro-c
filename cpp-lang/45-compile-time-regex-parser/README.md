# 세션 45: 컴파일 타임 정규 표현식 & 파서

## 학습 목표
1. 외부 라이브러리 없이 `constexpr` FSM으로 패턴 검증기를 만든다
2. 컴파일 타임에 문자열 형식을 검증하는 `static_assert` 패턴을 익힌다
3. 작은 파서 컴비네이터를 조합해 숫자, 식별자, key=value 문법을 파싱한다
4. 런타임 입력에도 같은 파서 로직을 재사용한다
5. 파서 실패 위치와 에러 메시지를 명시적으로 전달한다

## 핵심 개념

| 개념 | 설명 |
|------|------|
| constexpr FSM | 컴파일 타임 실행 가능한 유한 상태 기계 |
| parser combinator | 작은 파서를 함수처럼 조합해 큰 문법을 만드는 방식 |
| parse result | 성공 여부, 다음 위치, 파싱 값을 함께 담는 결과 타입 |
| static_assert validation | 잘못된 리터럴을 컴파일 단계에서 차단하는 검증 |
| zero allocation parser | 입력 문자열을 복사하지 않고 위치만 이동하는 파서 |

## 핵심 문법

```cpp
constexpr bool ok = is_date("2026-06-17");
static_assert(ok);
```

## 컴파일 명령어

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic homework.cpp  -o hw
g++ -std=c++20 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
g++ -std=c++20 -Wall -Wextra -Wpedantic game1.cpp     -o game
```
