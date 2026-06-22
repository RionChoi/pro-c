# 세션 22 — Coroutines (C++20)

## 학습 목표
- `co_yield`로 값을 지연 생성하는 Generator를 구현한다.
- `co_return`으로 결과를 반환하는 Task를 구현한다.
- `co_await`로 대기 가능한(Awaitable) 객체를 사용한다.
- promise_type / coroutine_handle 구조를 이해한다.

## 핵심 개념

### 코루틴 키워드
| 키워드 | 역할 |
|--------|------|
| `co_yield v` | 값을 호출자에게 전달하고 일시 중단 |
| `co_return v` | 최종 값을 반환하고 종료 |
| `co_await expr` | Awaitable을 기다리며 일시 중단 |

### Generator 구조
```cpp
template <typename T>
struct Generator {
    struct promise_type {
        T value;
        Generator   get_return_object();
        auto        initial_suspend() { return std::suspend_always{}; }
        auto        final_suspend()   noexcept { return std::suspend_always{}; }
        auto        yield_value(T v)  { value = v; return std::suspend_always{}; }
        void        return_void() {}
        void        unhandled_exception() { std::terminate(); }
    };
    // next() → resume → co_yield → suspend
};
```

### co_yield 사용
```cpp
Generator<int> fibonacci() {
    int a = 0, b = 1;
    while (true) {
        co_yield a;
        auto tmp = a + b;
        a = b;
        b = tmp;
    }
}
```

## 과제
| 파일 | 내용 |
|------|------|
| `homework.cpp` | Generator — co_yield로 지연 시퀀스 생성 |
| `homework2.cpp` | Task — co_return / co_await 비동기 패턴 |
| `game1.cpp` | 던전 탐험 게임 — Generator로 방 생성 |

## 컴파일
```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic homework.cpp  -o hw
g++ -std=c++20 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
g++ -std=c++20 -Wall -Wextra -Wpedantic game1.cpp     -o game
```
