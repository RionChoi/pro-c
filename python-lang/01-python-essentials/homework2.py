"""
Python-01 homework2.py
주제: enumerate, zip, map, filter, sorted, any/all, walrus operator
"""
from typing import Iterator


# ── 1. enumerate & zip ────────────────────────────────────────────────────
def builtin_demo() -> None:
    print("=== 1. enumerate & zip ===")

    subjects = ["수학", "영어", "과학", "국어", "역사"]
    scores   = [88, 92, 75, 95, 81]

    # enumerate: 인덱스 + 값
    print("  성적표:")
    for rank, (subj, score) in enumerate(
            sorted(zip(subjects, scores), key=lambda t: t[1], reverse=True), 1):
        bar = "█" * (score // 10)
        print(f"    {rank}위  {subj:<4}  {score:3}점  {bar}")

    # zip으로 두 리스트 병렬 처리
    before = [70, 65, 80]
    after  = [85, 72, 90]
    diffs  = [a - b for b, a in zip(before, after)]
    print(f"\n  향상도: {diffs}  (평균 {sum(diffs)/len(diffs):.1f}점)")


# ── 2. map & filter ───────────────────────────────────────────────────────
def map_filter_demo() -> None:
    print("\n=== 2. map & filter ===")

    raw_scores = ["85", "92", "78", "61", "99", "45"]

    # map: 문자열 리스트 → 정수 리스트 (C의 atoi 루프 대체)
    scores: list[int] = list(map(int, raw_scores))
    print(f"  변환: {scores}")

    # filter: 합격 점수만 추출
    passed = list(filter(lambda s: s >= 60, scores))
    print(f"  합격: {passed}")

    # map + filter 체이닝 → comprehension과 비교
    # comprehension 방식 (더 Pythonic)
    result = [s * 1.1 for s in scores if s >= 60]
    print(f"  합격자 10% 보정: {[round(r, 1) for r in result]}")


# ── 3. sorted, any, all ──────────────────────────────────────────────────
def sorting_demo() -> None:
    print("\n=== 3. sorted / any / all ===")

    students = [
        {"name": "철수", "score": 85, "grade": "B"},
        {"name": "영희", "score": 92, "grade": "A"},
        {"name": "민준", "score": 78, "grade": "C"},
        {"name": "지수", "score": 92, "grade": "A"},
    ]

    # key 함수로 다중 기준 정렬 (점수 내림차순, 이름 오름차순)
    ranked = sorted(students, key=lambda s: (-s["score"], s["name"]))
    print("  점수 순 (동점은 이름순):")
    for s in ranked:
        print(f"    {s['name']}: {s['score']}점 ({s['grade']})")

    scores = [s["score"] for s in students]

    # any / all
    print(f"\n  90점 이상 존재? {any(s >= 90 for s in scores)}")
    print(f"  전원 60점 이상? {all(s >= 60 for s in scores)}")
    print(f"  최고: {max(scores)}  최저: {min(scores)}  합계: {sum(scores)}")


# ── 4. Generator & Walrus Operator (:=) ──────────────────────────────────
def generator_walrus_demo() -> None:
    print("\n=== 4. Generator & Walrus Operator ===")

    # generator: 메모리 효율적인 시퀀스 (lazy evaluation)
    def fibonacci() -> Iterator[int]:
        a, b = 0, 1
        while True:
            yield a
            a, b = b, a + b

    fib = fibonacci()
    first_ten = [next(fib) for _ in range(10)]
    print(f"  피보나치 10개: {first_ten}")

    # generator expression (list comp와 비교)
    total = sum(x ** 2 for x in range(1_000_000))  # 메모리: O(1) vs list O(n)
    print(f"  1~999999 제곱합: {total}")

    # walrus operator (:=) — Python 3.8+
    # C의 while ((c = getchar()) != EOF) 패턴과 유사
    data = [1, -2, 3, -4, 5, -6, 7, 8, -9, 10]
    positives = [y for x in data if (y := x * 2) > 0]
    print(f"  양수 × 2: {positives}")


# ── main ─────────────────────────────────────────────────────────────────
def main() -> None:
    builtin_demo()
    map_filter_demo()
    sorting_demo()
    generator_walrus_demo()


if __name__ == "__main__":
    main()
