# 세션 58: 벤치마크와 성능 회귀 관리

## 학습 목표
1. 큰 프레임워크 없이도 `std::chrono`로 재현 가능한 microbenchmark를 작성한다
2. warmup, 반복 측정, median/best/worst를 한 번에 기록하는 최소 harness를 만든다
3. 절대 시간보다 같은 실행 안의 baseline 대비 비율을 우선 본다
4. 성능 회귀를 "이전보다 느린가"가 아니라 "기준 알고리즘보다 여전히 낫나"로 점검한다
5. 사람이 복사해 기록하기 쉬운 Markdown 한 줄 포맷을 남긴다

## 2026-07-02 기준 상태

- 이 저장소는 외부 벤치마크 라이브러리를 사용하지 않는다.
- 따라서 이번 세션은 `std::chrono::steady_clock` 기반의 작은 harness만 추가한다.
- 핵심은 절대 수치의 정확도를 과장하지 않고, 동일 실행 내 baseline/candidate 비교와
  기록 포맷을 남기는 것이다.
- 현재 로컬 Apple Clang 21 환경에서는 이 fallback harness로 충분히 median/best/worst와
  간단한 regression gate를 실습할 수 있다.

## 핵심 개념

| 개념 | 설명 |
|------|------|
| warmup | 첫 실행 비용을 본 측정에서 빼기 위한 사전 호출 |
| median | 잡음이 있을 때 평균보다 덜 흔들리는 대표값 |
| guard value | 컴파일러가 연산을 통째로 지우지 못하게 남기는 결과값 |
| baseline | 비교 기준이 되는 단순 알고리즘 또는 기존 구현 |
| regression gate | candidate가 baseline보다 충분히 빠른지 확인하는 작은 규칙 |

## 파일 구성

### `benchmark.hpp`
- `run(name, warmup, iterations, fn)`
- `Result { medianUs, bestUs, worstUs, guard }`
- 콘솔 출력과 Markdown 기록 한 줄 출력

### `homework.cpp`
**접근 패턴 벤치마크**
- 연속 접근과 stride 접근 비교
- 캐시 친화성 차이를 median 기준으로 확인

### `homework2.cpp`
**reserve 효과 벤치마크**
- `std::vector`에 reserve 유무를 비교
- 결과를 Markdown 행으로도 출력

### `game1.cpp`
**성능 회귀 게이트**
- `vector.erase(begin())`를 baseline
- `deque.pop_front()`를 candidate
- candidate가 baseline보다 느리면 회귀 경고

## 컴파일 명령어

```bash
clang++ -std=c++2c -O2 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion homework.cpp -o hw

clang++ -std=c++2c -O2 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion homework2.cpp -o hw2

clang++ -std=c++2c -O2 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
  -Wnull-dereference -Wdouble-promotion game1.cpp -o game
```

벤치마크는 최적화가 켜진 빌드에서 보는 편이 맞으므로, 이번 세션은 `-O2`를 기준으로 한다.

## 기록 양식

복사해서 문서에 붙이기 위한 최소 양식:

```text
| 날짜 | baseline | candidate | baseline median | candidate median | ratio |
```

## 참고 자료

- [Clang Users Manual](https://clang.llvm.org/docs/UsersManual.html)
