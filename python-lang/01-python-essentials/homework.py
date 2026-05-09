"""
Python-01 homework.py
주제: 동적 타입, f-string, type hint, list/dict comprehension
전제: C/C++ 기본 문법은 이미 알고 있음 — Python 고유 특성에만 집중
"""

# ── 1. 동적 타입과 type hint ──────────────────────────────────────────────
def type_demo() -> None:
    print("=== 1. 동적 타입 & Type Hint ===")

    # 동적 타입: 런타임에 타입 결정
    value = 42
    print(f"  value = {value!r}  type = {type(value).__name__}")

    value = "이제 문자열"
    print(f"  value = {value!r}  type = {type(value).__name__}")

    value = [1, 2, 3]
    print(f"  value = {value!r}  type = {type(value).__name__}")

    # type hint: 실행에는 영향 없지만 IDE·협업에 필수
    name: str   = "리온"
    score: float = 95.7
    passed: bool = score >= 60.0
    print(f"\n  name={name}, score={score}, passed={passed}")


# ── 2. f-string 고급 ─────────────────────────────────────────────────────
def fstring_demo() -> None:
    print("\n=== 2. f-string ===")

    pi = 3.14159265
    items = ["사과", "바나나", "체리"]
    score = 87.5

    # 소수점 자리수
    print(f"  pi = {pi:.2f}")

    # 너비 정렬
    for i, item in enumerate(items, 1):
        print(f"  {i:2}. {item:<8} → {score + i:>6.1f}점")

    # 표현식 직접 삽입 (C printf와의 차이)
    n = 7
    print(f"  {n} × {n} = {n * n}")

    # !r / !s 변환 플래그
    text = "  공백 포함  "
    print(f"  repr : {text!r}")
    print(f"  strip: {text.strip()}")


# ── 3. List & Dict Comprehension ─────────────────────────────────────────
def comprehension_demo() -> None:
    print("\n=== 3. Comprehension ===")

    # list comprehension
    squares = [x ** 2 for x in range(1, 6)]
    print(f"  제곱: {squares}")

    # 조건 필터
    evens = [x for x in range(20) if x % 2 == 0]
    print(f"  짝수: {evens}")

    # 중첩 comprehension (2×3 행렬)
    matrix = [[r * c for c in range(1, 4)] for r in range(1, 3)]
    print(f"  행렬: {matrix}")

    # dict comprehension
    words = ["python", "go", "rust", "docker"]
    word_len = {w: len(w) for w in words}
    print(f"  단어 길이: {word_len}")

    # set comprehension (중복 제거)
    nums = [1, 2, 2, 3, 3, 3, 4]
    unique_squares = {x ** 2 for x in nums}
    print(f"  중복 없는 제곱: {unique_squares}")


# ── 4. 언패킹 & 다중 할당 ────────────────────────────────────────────────
def unpacking_demo() -> None:
    print("\n=== 4. 언패킹 ===")

    # 다중 할당
    a, b = 10, 20
    print(f"  a={a}, b={b}")

    # swap — C에서는 임시 변수 필요
    a, b = b, a
    print(f"  swap 후: a={a}, b={b}")

    # 스타 언패킹
    first, *middle, last = [1, 2, 3, 4, 5]
    print(f"  first={first}, middle={middle}, last={last}")

    # 중첩 언패킹
    point = (3, 4)
    x, y = point
    distance = (x ** 2 + y ** 2) ** 0.5
    print(f"  점 {point} 원점 거리: {distance:.2f}")


# ── main ─────────────────────────────────────────────────────────────────
def main() -> None:
    type_demo()
    fstring_demo()
    comprehension_demo()
    unpacking_demo()


if __name__ == "__main__":
    main()
