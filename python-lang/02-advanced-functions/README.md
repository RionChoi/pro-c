# Python-02: 함수 고급 패턴

## 전제 조건
Python-01 완료. C/C++의 함수 포인터, 콜백, 템플릿 개념을 알고 있다.
이 세션은 Python 함수가 **일급 객체(first-class object)**라는 점에서 출발하는
고급 패턴들을 다룬다.

---

## C/C++과의 비교

| 패턴 | C/C++ | Python |
|------|-------|--------|
| 가변 인자 | `va_list`, `...` | `*args`, `**kwargs` |
| 함수 래퍼 | 수동 래퍼 함수 작성 | decorator (`@`) |
| 지연 계산 | 직접 구현 필요 | `generator` + `yield` |
| 부분 적용 | 수동 구현 / `std::bind` | `functools.partial` |
| 메모이제이션 | 직접 구현 | `functools.lru_cache` |

---

## 1. *args / **kwargs

함수가 임의 개수의 위치 인자(`*args`)와 키워드 인자(`**kwargs`)를 받을 수 있다.

```python
def log(level: str, *args, sep: str = " ", **kwargs) -> None:
    msg = sep.join(str(a) for a in args)
    print(f"[{level}] {msg}", **kwargs)

log("INFO", "서버", "시작", "완료")
log("DEBUG", "x=", 42, sep="", end="\n\n")
```

## 2. Decorator

함수를 인자로 받아 **새로운 함수를 반환**하는 패턴. `@` 문법 설탕(syntactic sugar).

```python
import functools

def timer(func):
    @functools.wraps(func)   # 원본 함수 메타데이터 보존
    def wrapper(*args, **kwargs):
        import time
        start = time.perf_counter()
        result = func(*args, **kwargs)
        elapsed = time.perf_counter() - start
        print(f"{func.__name__} 실행 시간: {elapsed:.4f}s")
        return result
    return wrapper

@timer
def heavy_task(n: int) -> int:
    return sum(range(n))
```

### 인자를 받는 decorator (decorator factory)

```python
def repeat(n: int):
    def decorator(func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            for _ in range(n):
                result = func(*args, **kwargs)
            return result
        return wrapper
    return decorator

@repeat(3)
def greet(name: str) -> None:
    print(f"안녕하세요, {name}!")
```

## 3. Generator / yield

`yield`는 함수 실행을 **일시 중지**하고 값을 반환한다. 재개 시 중단 지점부터 계속.

```python
def countdown(n: int):
    while n > 0:
        yield n        # 여기서 일시 중지, 값 반환
        n -= 1         # next() 호출 시 여기서 재개

for v in countdown(5):
    print(v, end=" ")  # 5 4 3 2 1
```

`yield from`으로 다른 제너레이터에 위임할 수 있다.

## 4. functools

| 함수 | 역할 |
|------|------|
| `functools.wraps` | decorator 작성 시 원본 함수 메타데이터 보존 |
| `functools.lru_cache` | 자동 메모이제이션 (LRU 캐시) |
| `functools.partial` | 인자를 미리 고정한 새 함수 생성 |
| `functools.reduce` | 시퀀스를 누적 계산으로 단일 값으로 축약 |

---

## 과제 목표

| 파일 | 내용 |
|------|------|
| `homework.py` | `*args/**kwargs` + decorator (timer, retry, validate) |
| `homework2.py` | generator 심화 + functools (lru_cache, partial, reduce) |
| `game1.py` | decorator 파이프라인으로 만드는 데이터 처리 시스템 |
