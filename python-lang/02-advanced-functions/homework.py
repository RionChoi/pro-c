"""
Python-02 homework.py
주제: *args/**kwargs, decorator (timer, retry, validate)
"""
import functools
import time
import random
from typing import Callable, Any


# ── 1. *args / **kwargs ───────────────────────────────────────────────────
def args_kwargs_demo() -> None:
    print("=== 1. *args / **kwargs ===")

    # level을 키워드 전용 인자로 선언 (*messages 뒤에 오므로 반드시 키워드로 전달)
    def log(*messages: Any, level: str = "INFO", sep: str = " ", prefix: str = "") -> str:
        """가변 메시지를 받아 포맷된 로그 문자열을 반환한다."""
        body = sep.join(str(m) for m in messages)
        line = f"[{level}]{' ' + prefix if prefix else ''} {body}"
        print(line)
        return line

    log("서버", "시작", "완료")
    log("x=", 42, level="DEBUG", sep="", prefix="main.py:10")
    log("디스크 사용량", 87, "%", level="WARN", sep="")

    # ** 언패킹으로 dict를 키워드 인자로 전달
    config = {"level": "ERROR", "prefix": "db.py:55"}
    log("연결 실패", "재시도 중", **config)


# ── 2. Decorator: timer ───────────────────────────────────────────────────
def timer(func: Callable) -> Callable:
    """함수 실행 시간을 측정하는 decorator."""
    @functools.wraps(func)
    def wrapper(*args: Any, **kwargs: Any) -> Any:
        start = time.perf_counter()
        result = func(*args, **kwargs)
        elapsed = time.perf_counter() - start
        print(f"  ⏱  {func.__name__}() → {elapsed*1000:.2f}ms")
        return result
    return wrapper


# ── 3. Decorator: retry (decorator factory — 인자를 받는 decorator) ───────
def retry(max_attempts: int = 3, delay: float = 0.1) -> Callable:
    """실패 시 최대 max_attempts번 재시도하는 decorator."""
    def decorator(func: Callable) -> Callable:
        @functools.wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            last_exc: Exception = RuntimeError("알 수 없는 오류")
            for attempt in range(1, max_attempts + 1):
                try:
                    return func(*args, **kwargs)
                except Exception as exc:
                    last_exc = exc
                    print(f"  ↻  {func.__name__}() 시도 {attempt}/{max_attempts} 실패: {exc}")
                    if attempt < max_attempts:
                        time.sleep(delay)
            raise last_exc
        return wrapper
    return decorator


# ── 4. Decorator: validate_types ──────────────────────────────────────────
def validate_types(func: Callable) -> Callable:
    """type hint와 실제 인자 타입이 일치하는지 런타임에 검사한다."""
    hints = func.__annotations__

    @functools.wraps(func)
    def wrapper(*args: Any, **kwargs: Any) -> Any:
        params = list(func.__code__.co_varnames[:func.__code__.co_argcount])
        all_args = dict(zip(params, args))
        all_args.update(kwargs)
        for param, value in all_args.items():
            expected = hints.get(param)
            if expected and not isinstance(value, expected):
                raise TypeError(
                    f"{func.__name__}(): '{param}' 인자는 "
                    f"{expected.__name__} 이어야 하는데 "
                    f"{type(value).__name__} 이 전달됨"
                )
        return func(*args, **kwargs)
    return wrapper


# ── decorator 적용 예시 ───────────────────────────────────────────────────
@timer
def slow_sum(n: int) -> int:
    return sum(range(n))


_call_count = 0

@retry(max_attempts=3, delay=0.05)
def flaky_api(success_rate: float = 0.4) -> str:
    global _call_count
    _call_count += 1
    if random.random() > success_rate:
        raise ConnectionError("API 타임아웃")
    return "응답 성공"


@validate_types
def create_user(name: str, age: int, score: float) -> dict:
    return {"name": name, "age": age, "score": score}


# ── 5. decorator 스택 (여러 개 중첩) ─────────────────────────────────────
def decorator_stack_demo() -> None:
    print("\n=== 5. Decorator 스택 ===")

    def bold(func: Callable) -> Callable:
        @functools.wraps(func)
        def wrapper(*a: Any, **kw: Any) -> str:
            return f"**{func(*a, **kw)}**"
        return wrapper

    def upper(func: Callable) -> Callable:
        @functools.wraps(func)
        def wrapper(*a: Any, **kw: Any) -> str:
            return func(*a, **kw).upper()
        return wrapper

    @bold
    @upper            # 아래부터 적용: upper 먼저, bold 나중
    def greet(name: str) -> str:
        return f"hello, {name}"

    print(f"  {greet('python')}")   # **HELLO, PYTHON**
    print(f"  함수명 보존: {greet.__name__}")  # functools.wraps 덕분에 'greet'


# ── main ─────────────────────────────────────────────────────────────────
def main() -> None:
    args_kwargs_demo()

    print("\n=== 2. timer decorator ===")
    result = slow_sum(1_000_000)
    print(f"  결과: {result:,}")

    print("\n=== 3. retry decorator ===")
    random.seed(42)
    try:
        msg = flaky_api(success_rate=0.4)
        print(f"  최종: {msg} (총 {_call_count}회 시도)")
    except ConnectionError as e:
        print(f"  최종 실패: {e}")

    print("\n=== 4. validate_types decorator ===")
    user = create_user("리온", 25, 98.5)
    print(f"  생성됨: {user}")
    try:
        create_user("리온", "스물다섯", 98.5)  # age가 str — TypeError 발생
    except TypeError as e:
        print(f"  타입 오류 잡힘: {e}")

    decorator_stack_demo()


if __name__ == "__main__":
    main()
