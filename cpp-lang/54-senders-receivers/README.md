# 세션 54: `std::execution`와 Senders/Receivers

## 학습 목표
1. P2300 계열이 말하는 sender, receiver, scheduler 역할을 구분한다
2. 현재 컴파일러의 네이티브 `std::execution` 지원 여부와 fallback 설계를 분리한다
3. `just -> then -> sync_wait` 형태의 최소 파이프라인을 직접 구현한다
4. Thread pool 위에서 파이프라인 전체를 오프로딩하는 방법을 연습한다
5. cancellation, error channel 없이도 실행 모델의 핵심 흐름을 설명할 수 있다

## 2026-06-28 기준 상태

- Senders/Receivers는 WG21 P2300 계열 제안을 중심으로 표준화가 진행 중이다.
- 이 저장소의 기본 검증 환경인 Apple Clang 21에서는 `__cpp_lib_execution`,
  `__cpp_lib_sender_receiver`가 모두 정의되지 않았다.
- `std::this_thread::sync_wait` 같은 이름도 현재 로컬 표준 라이브러리에서 찾을 수 없다.
- 따라서 이 세션은 네이티브 `std::execution` 대신 `mini_exec.hpp`로 핵심 흐름만
  축소한 fallback 모델을 구현한다.

## 핵심 개념

| 개념 | 설명 |
|------|------|
| sender | 값을 즉시 계산하지 않고 "나중에 실행할 작업"으로 감싼 객체 |
| receiver | sender가 결과나 실패를 전달하는 목적지 |
| scheduler | 작업을 어느 실행 문맥에서 돌릴지 결정하는 객체 |
| `then` | 이전 단계 결과를 다음 변환 단계로 넘기는 연결 연산 |
| `sync_wait` | 파이프라인을 실제로 실행하고 최종 결과를 기다리는 동기 경계 |

## 파일 구성

### `mini_exec.hpp`
- `just`, `then`, `sync_wait`
- 단일 값 채널만 다루는 `Sender`
- 최소 `ThreadPool`, `start`, `on`
- `__cpp_lib_sender_receiver` 기반 native 지원 플래그

### `homework.cpp`
**동기 sender 파이프라인**
- 문자열 설정 행을 토큰화
- 양수 검증 후 합계를 계산
- `just -> then -> then -> sync_wait` 흐름 확인

### `homework2.cpp`
**thread pool fallback**
- 주문 견적 파이프라인을 백그라운드 worker로 오프로딩
- `on(pool, sender)`로 전체 체인을 다른 실행 문맥에서 수행
- worker thread id와 결과 문자열 출력

### `game1.cpp`
**작업 보드 시뮬레이터**
- 여러 작업을 sender 파이프라인으로 변환
- `start(pool, sender)`로 동시에 발사
- 결과를 수집해 우선순위 기준 정렬

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

GCC나 libc++가 향후 네이티브 `std::execution`을 제공하더라도, 이 세션의 목적은
대형 실행 모델을 한 번에 다 구현하는 것이 아니라 sender 파이프라인의 핵심 감각을
작은 예제로 고정하는 데 있다.

## 참고 자료

- [WG21 P2300: `std::execution`](https://wg21.link/P2300)
- [Clang C++ 구현 현황](https://clang.llvm.org/cxx_status.html)
