# 세션 51: C++26 표준 현황 점검

## 학습 목표
1. 현재 ISO 표준과 작업 중인 C++26 초안을 구분한다
2. `-std=c++26`과 `-std=c++2c` 컴파일러 플래그의 차이를 이해한다
3. `__cplusplus`와 SD-6 기능 테스트 매크로로 실제 지원 여부를 확인한다
4. 표준 채택 여부와 컴파일러 구현 여부를 별도로 기록한다
5. 미지원 기능에 fallback을 두는 호환성 점검 흐름을 설계한다

## 2026-06-24 기준 상태

- 현재 발행된 ISO C++ 표준은 C++23이다.
- C++26은 작업 중인 차기 표준이며 구현 상태는 컴파일러와 표준 라이브러리마다 다르다.
- GCC는 C++26 실험 모드에 `-std=c++26`을 사용한다.
- Clang은 C++26 실험 모드에 `-std=c++2c`를 사용한다.
- 기능 지원은 컴파일러 버전만 추측하지 말고 SD-6 기능 테스트 매크로로 확인한다.

## 핵심 개념

| 개념 | 설명 |
|------|------|
| `__cplusplus` | 선택한 C++ 언어 모드를 나타내는 구현 정의 값 |
| SD-6 매크로 | 개별 언어/라이브러리 기능의 지원 여부와 리비전을 나타내는 매크로 |
| draft status | 기능이 C++26 작업 초안에 채택되었는지 나타내는 표준화 상태 |
| implementation status | 현재 컴파일러와 표준 라이브러리가 기능을 구현했는지 나타내는 상태 |
| fallback | 목표 기능이 없을 때 사용하는 대체 구현 또는 제한된 동작 |

## 과제

### homework.cpp
**컴파일러 기능 진단기**
- 컴파일러 이름과 `__cplusplus` 출력
- C++26 후보 기능의 SD-6 매크로 값 출력
- 지원, 부분 지원, 미지원 상태 분류

### homework2.cpp
**기능 요구사항 호환성 검사**
- 프로젝트가 요구하는 최소 SD-6 매크로 값 정의
- 현재 컴파일러 값과 비교
- native, fallback, unavailable 상태를 표로 출력

### game1.cpp
**C++26 도입 준비도 시뮬레이터**
- 여러 프로젝트 프로필의 기능 요구사항 정의
- 현재 환경에서 직접 사용 가능한 기능과 fallback이 필요한 기능 계산
- 준비도 점수와 권장 도입 전략 출력

## 컴파일 명령어

### Clang / Apple Clang
```bash
clang++ -std=c++2c -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion homework.cpp -o hw
```

### GCC 14 이상
```bash
g++ -std=c++26 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion homework.cpp -o hw
```

`homework2.cpp`와 `game1.cpp`도 같은 플래그로 컴파일한다.

## 참고 자료

- [ISO C++ 현재 표준 상태](https://isocpp.org/std/the-standard)
- [Clang C++ 구현 현황](https://clang.llvm.org/cxx_status.html)
- [GCC C++ 구현 현황](https://gcc.gnu.org/projects/cxx-status.html)
