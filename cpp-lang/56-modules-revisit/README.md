# 세션 56: Modules와 `import std` 재검토

## 학습 목표
1. 세션 32의 named module 예제를 현재 컴파일러 현실에 맞게 다시 정리한다
2. named module과 `import std`를 같은 문제로 묶지 않고 분리해서 본다
3. 모듈 지원이 부족한 환경에서도 헤더 fallback으로 같은 소비자 코드를 유지한다
4. `statistics` 예제를 header path와 module path 두 방식으로 빌드하는 흐름을 익힌다
5. 툴체인 readiness를 문서와 코드에서 동시에 확인하는 습관을 만든다

## 2026-06-30 기준 상태

- Clang 공식 C++ 상태표는 C++20 modules를 지원 항목으로 표시한다.
- 하지만 이 저장소의 기본 검증 환경인 Apple Clang 21에서는 `import std;`가 바로
  동작하지 않았고, named module 문법도 현재 기본 빌드 플래그만으로는 받아들여지지 않았다.
- GCC 계열은 `-fmodules-ts` 기반 named module 경로를 여전히 실무적 fallback으로 쓸 수
  있지만, 이번 저장소 검증 기준은 Apple Clang에서도 돌아가는 코드여야 한다.
- 따라서 이번 세션은 `statistics.cppm`을 선택적 경로로 두고, 기본 소비자는
  `statistics.hpp` 헤더 fallback으로 유지한다.

## 핵심 개념

| 개념 | 설명 |
|------|------|
| named module | 사용자가 정의한 모듈 인터페이스 유닛 (`export module statistics;`) |
| `import std` | 표준 라이브러리를 모듈 형태로 불러오려는 경로 |
| fallback | 모듈이 안 되는 환경에서 같은 API를 헤더로 소비하는 대체 경로 |
| readiness probe | 특정 문법이 현재 툴체인에서 실제로 되는지 확인하는 작은 빌드 실험 |
| optional module path | 기본 빌드를 깨지 않으면서 지원되는 컴파일러에서만 켜는 경로 |

## 파일 구성

### `statistics.hpp`
- 세션 32의 통계 API를 inline 헤더로 정리
- 기본 빌드 경로
- 현재 소비 방식(`named module` 또는 `header fallback`) 문자열 제공

### `statistics.cppm`
- `statistics.hpp`를 재내보내는 선택적 named module 인터페이스
- 지원되는 컴파일러에서만 사용

### `homework.cpp`
**툴체인 readiness 리포트**
- `__cpp_modules` 확인
- 통계 API 호출
- 현재 소비 경로 출력

### `homework2.cpp`
**성적 분석기 재구성**
- 모듈 가능 환경에서는 `import statistics`
- 기본값은 `#include "statistics.hpp"`
- 학생 점수 요약 출력

### `game1.cpp`
**주사위 통계 게임 재구성**
- 같은 소비 패턴을 게임 예제로 반복
- 라운드별 평균과 승자 판단 출력

## 컴파일 명령어

기본 검증 경로는 현재 저장소 환경에서 항상 동작하는 헤더 fallback이다.

```bash
clang++ -std=c++2c -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion homework.cpp -o hw

clang++ -std=c++2c -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion homework2.cpp -o hw2

clang++ -std=c++2c -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion game1.cpp -o game
```

named module 경로는 지원되는 GCC 계열에서만 선택적으로 사용한다.

```bash
g++ -std=c++23 -fmodules-ts -DPRO_C_USE_MODULES \
  statistics.cppm homework2.cpp -o hw2

g++ -std=c++23 -fmodules-ts -DPRO_C_USE_MODULES \
  statistics.cppm game1.cpp -o game
```

`import std`는 이번 세션에서 기본 전제로 두지 않는다. 현재 저장소 검증 환경에서
실패하기 때문에, named module 재구성까지를 목표선으로 삼는다.

## 참고 자료

- [Clang C++ 구현 현황](https://clang.llvm.org/cxx_status.html)
- [GCC C++ 구현 현황](https://gcc.gnu.org/projects/cxx-status.html)
