# 세션 52: Static Reflection 실험

## 학습 목표
1. C++26 정적 리플렉션의 reflection value, metafunction, splice 개념을 이해한다
2. 현재 컴파일러의 네이티브 reflection 지원 여부와 표준 채택 상태를 구분한다
3. 멤버 포인터와 `std::tuple`로 portable reflection facade를 구현한다
4. 구조체 필드를 일반화된 코드로 순회하고 직렬화한다
5. 메타데이터 기반 설정 적용과 검증 흐름을 설계한다

## 2026-06-25 기준 상태

- C++26 작업 초안의 정적 리플렉션 모델은 P2996R13을 기반으로 한다.
- 핵심 모델은 `std::meta::info`, `^^` reflection operator, metafunction, splice로 구성된다.
- Clang 공식 C++ 상태표에서 P2996R13 reflection은 아직 미지원으로 표시된다.
- 따라서 이 세션은 네이티브 문법을 직접 요구하지 않고, 교체 가능한 portable facade로
  필드 순회 API를 먼저 구현한다.

## 핵심 개념

| 개념 | 설명 |
|------|------|
| reflection value | 프로그램 요소를 컴파일 타임 값으로 표현한 메타데이터 |
| reflection operator | C++26 제안에서 `^^`로 프로그램 요소의 reflection value를 얻는 연산자 |
| splice | reflection value가 나타내는 프로그램 요소를 코드 문맥에 다시 삽입하는 문법 |
| field descriptor | 필드 이름과 멤버 포인터를 묶은 portable 메타데이터 |
| reflection facade | 네이티브 지원 전후에 동일한 필드 순회 API를 제공하는 얇은 추상화 |

## 파일 구성

### `reflection.hpp`
- `meta::Field` 필드 descriptor
- 사용자 정의 `meta::Reflect<T>` 특수화 지점
- `meta::Reflectable` concept
- `meta::for_each_field()` 범용 필드 순회

### `homework.cpp`
**타입 메타데이터 출력기**
- `Developer` 구조체의 필드 메타데이터 등록
- 필드 이름, 타입 분류, 현재 값 출력
- 필드 수와 native/fallback 모드 확인

### `homework2.cpp`
**리플렉션 기반 JSON 직렬화**
- 중첩된 reflectable 구조체 정의
- 문자열, 숫자, bool, 구조체를 재귀적으로 직렬화
- 타입별 수동 serializer 없이 필드 descriptor를 순회

### `game1.cpp`
**타입 안전 설정 콘솔**
- 문자열 입력을 실제 필드 타입으로 변환
- 필드 이름으로 설정값 적용
- 포트, worker 수, 필수 문자열 검증
- 최종 설정과 오류 목록 출력

## 컴파일 명령어

현재 예제는 facade fallback을 사용하므로 C++20 이상에서 실행된다.

```bash
clang++ -std=c++2c -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion homework.cpp -o hw

clang++ -std=c++2c -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion homework2.cpp -o hw2

clang++ -std=c++2c -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion game1.cpp -o game
```

GCC에서는 `-std=c++26`을 사용한다. C++26 모드가 없는 환경에서는 `-std=c++20`으로
fallback 구현을 검증할 수 있다.

## 참고 자료

- [WG21 P2996R13: Reflection for C++26](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/p2996r13.html)
- [Clang C++ 구현 현황](https://clang.llvm.org/cxx_status.html)
