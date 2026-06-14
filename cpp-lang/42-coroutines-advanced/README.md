# 세션 42: C++20 Coroutines 심화

## 학습 목표
1. 코루틴 함수가 `promise_type`을 통해 상태 머신으로 변환되는 흐름을 이해한다
2. `Generator<T>`를 직접 구현해 `co_yield` 기반 지연 시퀀스를 만든다
3. `Task<T>`를 직접 구현해 `co_return` 기반 비동기 작업 모델을 익힌다
4. `co_await`의 awaiter 3요소(`await_ready`, `await_suspend`, `await_resume`)를 이해한다
5. Generator와 Task를 조합한 간단한 비동기 파이프라인을 설계한다

## 핵심 개념

| 개념 | 설명 |
|------|------|
| coroutine | 실행을 중단하고 재개할 수 있는 함수 |
| promise_type | 코루틴의 반환 객체, 저장 값, 예외 처리 방식을 정의하는 타입 |
| coroutine_handle | 중단된 코루틴 프레임을 재개하거나 파괴하는 핸들 |
| `co_yield` | 값을 하나 내보내고 코루틴을 일시 중단 |
| `co_return` | 코루틴의 최종 결과를 저장하고 종료 |
| `co_await` | awaiter 규약을 통해 비동기 대기 지점을 표현 |
| lazy evaluation | 값이 필요할 때만 코루틴을 재개하는 실행 방식 |

## 핵심 문법

```cpp
Generator<int> count(int n) {
    for (int i = 1; i <= n; ++i) {
        co_yield i;
    }
}

Task<int> calculate_score(int base) {
    co_return base * 2;
}
```

## 컴파일 명령어

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic homework.cpp  -o hw
g++ -std=c++20 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
g++ -std=c++20 -Wall -Wextra -Wpedantic game1.cpp     -o game
```
