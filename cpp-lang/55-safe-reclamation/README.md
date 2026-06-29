# 세션 55: Safe Reclamation과 동시성 안전성

## 학습 목표
1. lock-free 자료구조에서 "노드를 언제 delete할 수 있는가"가 핵심 문제임을 이해한다
2. 해저드 포인터와 epoch/RCU 계열이 같은 문제를 다른 방식으로 푸는 점을 구분한다
3. 해저드 포인터는 개별 포인터 보호, epoch/RCU는 읽기 구간 보호라는 감각을 잡는다
4. retire list, grace period, scan-and-reclaim의 최소 흐름을 구현한다
5. 현재 표준 라이브러리 지원과 별개로 실전에서 필요한 안전 회수 사고방식을 익힌다

## 2026-06-29 기준 상태

- C++ 표준화 논의에서는 hazard pointer 계열과 RCU 계열 메모리 회수 API가 각각 별도
  제안으로 다뤄진다.
- 이 저장소의 기본 검증 환경은 해당 기능의 네이티브 표준 라이브러리 제공을 가정할 수
  없으므로, 이번 세션은 개념을 설명하는 작은 fallback 모델로 진행한다.
- 해저드 포인터는 세션 47의 lock-free 예제를 더 안전하게 읽기 위한 보호 기법이고,
  epoch/RCU는 읽기-많은 구조에서 grace period 이후 일괄 회수를 노리는 방식이다.

## 핵심 개념

| 개념 | 설명 |
|------|------|
| retire | 즉시 `delete`하지 않고 "나중에 회수" 대기열로 넘기는 단계 |
| hazard pointer | 스레드가 현재 접근 중인 포인터를 공개해 삭제를 막는 방식 |
| grace period | 이전 읽기 구간이 모두 끝났다고 볼 수 있는 시점 |
| epoch reclamation | 전역 epoch를 돌려 오래된 retire 항목만 회수하는 방식 |
| RCU 스타일 읽기 | 읽기 측은 매우 가볍게 보호하고, 쓰기 측이 회수 타이밍을 책임지는 패턴 |

## 파일 구성

### `reclaim.hpp`
- `HazardDomain`, `RetireList`, `AtomicStack`
- `EpochDomain`과 `Guard`
- 해저드 포인터 기반 즉시 보호 + epoch 기반 지연 회수 최소 구현

### `homework.cpp`
**hazard pointer stack**
- lock-free 스택 pop에 해저드 포인터 적용
- 두 consumer가 동시에 pop
- retire list와 즉시 reclaim 흐름 확인

### `homework2.cpp`
**epoch/RCU 스타일 config store**
- immutable 설정 스냅샷 교체
- reader는 가벼운 guard만 사용
- writer는 old snapshot을 retire 후 grace period 뒤 회수

### `game1.cpp`
**quest board scavenger**
- 여러 worker가 작업 카드를 동시에 pop
- 해저드 포인터로 노드 회수 안전성 유지
- 팀별 점수 집계와 cleanup 상태 출력

## 컴파일 명령어

현재 예제는 fallback 모델을 사용하므로 C++20 이상에서 실행된다.

```bash
clang++ -std=c++2c -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion homework.cpp -o hw

clang++ -std=c++2c -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion homework2.cpp -o hw2

clang++ -std=c++2c -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion game1.cpp -o game
```

이번 세션은 "완전한 범용 reclamation 라이브러리"를 만드는 것이 아니라, delete 시점을
미루는 최소 규칙이 왜 필요한지 손으로 확인하는 데 목적이 있다.

## 참고 자료

- [WG21 P2530: Hazard Pointers for C++](https://wg21.link/P2530)
- [WG21 P2545: Read-Copy Update (RCU) for C++](https://wg21.link/P2545)
