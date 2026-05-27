# 세션 32: C++20 모듈 시스템 (Modules)

## 학습 목표

- C++20 모듈의 필요성과 헤더 파일의 한계를 이해한다
- `export module`, 전역 모듈 프래그먼트, `import` 문법을 익힌다
- 모듈 인터페이스 유닛(MIU)과 소비자 파일의 역할을 구분한다
- 기존 헤더 기반 코드를 모듈로 전환하는 방법을 실습한다

## 핵심 개념

### 왜 모듈인가?

헤더 파일의 문제점:
- `#include`는 텍스트 치환 — 같은 헤더를 수백 번 파싱
- 매크로 오염 (include 순서에 따라 동작 달라짐)
- 순환 의존성 문제
- 컴파일 속도 저하

모듈의 장점:
- 선컴파일된 모듈 인터페이스(`.gcm`) 재사용 → 빌드 속도 향상
- 명시적 내보내기 (`export`) — 의도하지 않은 심볼 노출 없음
- 매크로 격리 — 모듈 내부 매크로가 소비자 파일에 영향 X
- 순서 독립적 임포트

### 기본 문법

```cpp
// 모듈 인터페이스 유닛 (math_module.cpp)
module;                     // 전역 모듈 프래그먼트 시작
#include <cmath>            // 레거시 헤더는 여기에 포함

export module math_module;  // 이 파일이 math_module을 정의·내보냄

export double circle_area(double r);   // 내보내는 선언
double helper(double x);               // 내보내지 않는 내부 함수

// 구현
double circle_area(double r) { return 3.14159 * r * r; }
double helper(double x) { return x * 2; }
```

```cpp
// 소비자 파일 (main.cpp)
#include <iostream>         // 레거시 헤더는 import 전에 포함
import math_module;         // 모듈 임포트

int main() {
    std::cout << circle_area(5.0) << "\n";
}
```

### 전역 모듈 프래그먼트 (Global Module Fragment)

`module;` ~ `export module 이름;` 사이 구간.  
레거시 헤더(`<cmath>`, `<vector>` 등)만 포함 가능.  
여기 포함된 이름은 모듈 외부로 자동 내보내지지 않음.

### export 종류

```cpp
export int x;                      // 변수
export double f(double);           // 함수
export class Foo { ... };          // 클래스
export namespace Bar { ... }       // 네임스페이스 블록

export {                           // 묶음 내보내기
    void a();
    void b();
}
```

### 모듈 파티션 (Module Partitions)

대형 모듈을 여러 파일로 분할할 때 사용:

```cpp
// mylib-math.cpp
export module mylib:math;
export int add(int a, int b) { return a + b; }

// mylib.cpp (기본 인터페이스)
export module mylib;
export import :math;    // 파티션 재내보내기
```

## 파일 구성

| 파일 | 역할 |
|------|------|
| `homework.cpp` | `statistics` 모듈 인터페이스 유닛 (평균·중앙값·표준편차·요약) |
| `homework2.cpp` | `statistics` 모듈 소비자 — 학생 성적 분석기 |
| `game1.cpp` | `statistics` 모듈 소비자 — 주사위 통계 대결 게임 |

## 컴파일 방법

> **주의**: 소비자 파일을 빌드할 때 모듈 인터페이스 유닛을 함께 컴파일해야 합니다.  
> GCC 13에서는 `-fmodules-ts` 플래그가 필요합니다.

```bash
# homework.cpp (모듈 인터페이스) 단독 검증
g++ -std=c++23 -fmodules-ts -Wall -Wextra -Wpedantic -c homework.cpp

# homework2 빌드 (모듈 + 소비자를 함께 컴파일)
g++ -std=c++23 -fmodules-ts -Wall -Wextra -Wpedantic \
    homework.cpp homework2.cpp -o hw2
./hw2

# game1 빌드
g++ -std=c++23 -fmodules-ts -Wall -Wextra -Wpedantic \
    homework.cpp game1.cpp -o game1
./game1
```

## 중요 규칙

1. 소비자 파일에서는 **`#include` 를 `import` 보다 먼저** 작성해야 GCC 13에서 충돌이 없다
2. 모듈 내부의 전역 모듈 프래그먼트에 포함된 헤더는 소비자에게 자동으로 재내보내지지 않는다
3. GCC 13은 `private module fragment`를 아직 지원하지 않는다
4. 모듈 파일에 `main()`을 넣을 수 없다 — 소비자 파일에서 정의한다
