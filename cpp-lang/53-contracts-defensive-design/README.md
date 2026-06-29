# 세션 53: Contracts 기반 방어적 설계

## 학습 목표
1. C++26 Contracts의 사전조건, 사후조건, assertion 역할을 구분한다
2. 현재 컴파일러별 Contracts 지원 상태와 실험 플래그 차이를 이해한다
3. 표준 문법을 직접 요구하지 않는 portable contract facade를 설계한다
4. API 경계에서 입력 검증, 상태 불변식, 결과 검증을 분리한다
5. 계약 위반을 디버깅 가능한 진단 정보로 기록한다

## 2026-06-26 기준 상태

- C++26 Contracts는 P2900R14 기반으로 작업 초안에 반영된 기능이다.
- Clang 공식 C++ 상태표에서는 C++26 Contracts가 아직 미지원으로 표시된다.
- GCC 공식 C++ 상태표에서는 GCC 16부터 Contracts 항목과 `__cpp_contracts >= 202502L`
  기능 테스트 매크로가 표시된다.
- 이 저장소의 기본 검증 환경은 Apple Clang이므로, 본 세션은 네이티브 Contracts 문법 대신
  `CONTRACT_PRE`, `CONTRACT_POST`, `CONTRACT_ASSERT` fallback을 사용한다.

## 핵심 개념

| 개념 | 설명 |
|------|------|
| precondition | 함수 호출자가 지켜야 하는 입력 조건 |
| postcondition | 함수 구현이 반환 전에 보장해야 하는 결과 조건 |
| assertion | 함수 내부 알고리즘과 불변식이 유지되는지 확인하는 조건 |
| invariant | 객체가 공개 API 호출 전후에 항상 만족해야 하는 상태 규칙 |
| contract facade | 표준 지원 전후에 같은 의미를 유지하도록 감싼 검증 계층 |

## 파일 구성

### `contract.hpp`
- `lesson_contracts::ContractViolation` 예외 타입
- 위반 종류, 표현식, 파일, 라인, 함수명을 포함하는 `ViolationInfo`
- `CONTRACT_PRE`, `CONTRACT_POST`, `CONTRACT_ASSERT` 매크로
- `__cpp_contracts` 기반 native 지원 여부 플래그

### `homework.cpp`
**계약 기반 은행 계좌 API**
- 입금/출금/송금 API의 사전조건과 사후조건 분리
- 잔액 음수 금지 invariant 검증
- 위반 예제를 안전하게 포착하고 진단 정보 출력

### `homework2.cpp`
**고정 용량 큐의 방어적 설계**
- 템플릿 기반 `BoundedQueue<T, Capacity>` 구현
- `push`, `pop`, `front`에 pre/postcondition 적용
- 순환 버퍼 내부 index invariant 검증

### `game1.cpp`
**계약 검증 던전 시뮬레이터**
- 플레이어 행동 API에 계약 적용
- 정상 턴과 잘못된 명령을 같은 엔진에서 처리
- 계약 위반이 게임 상태를 깨지 않고 진단으로 남는지 확인

## 컴파일 명령어

현재 예제는 fallback facade를 사용하므로 C++20 이상에서 실행된다.

```bash
clang++ -std=c++2c -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion homework.cpp -o hw

clang++ -std=c++2c -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion homework2.cpp -o hw2

clang++ -std=c++2c -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion game1.cpp -o game
```

GCC에서는 C++26 모드가 필요하면 `-std=c++26`을 사용한다. GCC의 네이티브 Contracts
실험은 컴파일러 버전과 플래그 요구사항이 바뀔 수 있으므로, 이 세션의 검증 기준은
portable fallback 동작이다.

## 참고 자료

- [WG21 P2900R14: Contracts for C++](https://wg21.link/P2900R14)
- [Clang C++ 구현 현황](https://clang.llvm.org/cxx_status.html)
- [GCC C++ 구현 현황](https://gcc.gnu.org/projects/cxx-status.html)
