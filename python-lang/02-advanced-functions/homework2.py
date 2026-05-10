"""
Python-02 homework2.py
주제: generator 심화, yield from, functools (lru_cache, partial, reduce)
"""
import functools
import operator
from typing import Generator, Iterator


# ── 1. Generator 심화 ─────────────────────────────────────────────────────
def generator_demo() -> None:
    print("=== 1. Generator 심화 ===")

    # 무한 수열 — list로 만들면 메모리 폭발, generator는 O(1)
    def naturals(start: int = 1) -> Iterator[int]:
        n = start
        while True:
            yield n
            n += 1

    def take(n: int, it: Iterator[int]) -> list[int]:
        return [next(it) for _ in range(n)]

    print(f"  자연수 10개: {take(10, naturals())}")

    # 파이프라인: generator를 generator에 연결
    def evens(it: Iterator[int]) -> Iterator[int]:
        for x in it:
            if x % 2 == 0:
                yield x

    def squares(it: Iterator[int]) -> Iterator[int]:
        for x in it:
            yield x * x

    pipeline = squares(evens(naturals()))
    print(f"  짝수의 제곱 8개: {take(8, pipeline)}")


# ── 2. yield from ─────────────────────────────────────────────────────────
def yield_from_demo() -> None:
    print("\n=== 2. yield from ===")

    def flatten(nested: list) -> Generator:
        """중첩 리스트를 재귀적으로 평탄화한다."""
        for item in nested:
            if isinstance(item, list):
                yield from flatten(item)   # 재귀 generator 위임
            else:
                yield item

    data = [1, [2, 3], [4, [5, 6]], [[[7]], 8]]
    print(f"  원본: {data}")
    print(f"  평탄화: {list(flatten(data))}")

    # 여러 이터러블 연결
    def chain(*iterables):
        for it in iterables:
            yield from it

    result = list(chain([1, 2], "ab", range(3)))
    print(f"  chain: {result}")


# ── 3. send() — 양방향 generator ─────────────────────────────────────────
def send_demo() -> None:
    print("\n=== 3. send() — 양방향 Generator ===")

    def running_average() -> Generator[float, float, str]:
        """send()로 값을 주입받아 누적 평균을 yield한다."""
        total, count = 0.0, 0
        while True:
            value = yield total / count if count else 0.0
            if value is None:
                return "완료"
            total += value
            count += 1

    avg = running_average()
    next(avg)                          # generator 초기화 (첫 yield까지 진행)
    for score in [80, 90, 70, 100, 60]:
        current = avg.send(score)
        print(f"  {score} 입력 → 누적 평균: {current:.1f}")


# ── 4. functools.lru_cache ────────────────────────────────────────────────
def lru_cache_demo() -> None:
    print("\n=== 4. functools.lru_cache ===")

    # 캐시 없는 피보나치: O(2^n) 호출
    call_count = 0

    @functools.lru_cache(maxsize=None)   # 무제한 캐시
    def fib(n: int) -> int:
        nonlocal call_count
        call_count += 1
        if n < 2:
            return n
        return fib(n - 1) + fib(n - 2)

    result = fib(35)
    print(f"  fib(35) = {result}")
    print(f"  실제 호출 횟수: {call_count}회 (캐시 없으면 수천만 회)")
    print(f"  캐시 정보: {fib.cache_info()}")


# ── 5. functools.partial ──────────────────────────────────────────────────
def partial_demo() -> None:
    print("\n=== 5. functools.partial ===")

    def power(base: float, exp: float) -> float:
        return base ** exp

    # 인자를 미리 고정 — C++의 std::bind와 유사
    square = functools.partial(power, exp=2)
    cube   = functools.partial(power, exp=3)
    double = functools.partial(operator.mul, 2)

    nums = [1, 2, 3, 4, 5]
    print(f"  제곱: {list(map(square, nums))}")
    print(f"  세제곱: {list(map(cube, nums))}")
    print(f"  두배: {list(map(double, nums))}")

    # 로그 함수에 레벨 고정
    def log(level: str, msg: str) -> str:
        return f"[{level}] {msg}"

    info  = functools.partial(log, "INFO")
    error = functools.partial(log, "ERROR")
    print(f"\n  {info('서버 시작')}")
    print(f"  {error('연결 실패')}")


# ── 6. functools.reduce ───────────────────────────────────────────────────
def reduce_demo() -> None:
    print("\n=== 6. functools.reduce ===")

    nums = [1, 2, 3, 4, 5]

    total   = functools.reduce(operator.add, nums)
    product = functools.reduce(operator.mul, nums)
    maximum = functools.reduce(lambda a, b: a if a > b else b, nums)

    print(f"  합계: {total}")
    print(f"  곱:   {product}")
    print(f"  최대: {maximum}")

    # 중첩 dict 병합
    configs = [{"host": "localhost"}, {"port": 8080}, {"debug": True}]
    merged: dict = functools.reduce(lambda a, b: {**a, **b}, configs)
    print(f"  병합된 설정: {merged}")


# ── main ─────────────────────────────────────────────────────────────────
def main() -> None:
    generator_demo()
    yield_from_demo()
    send_demo()
    lru_cache_demo()
    partial_demo()
    reduce_demo()


if __name__ == "__main__":
    main()
