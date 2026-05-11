"""
Python-03 homework2.py
주제: itertools 전체 — chain, islice, groupby, product, combinations,
      permutations, accumulate, cycle, repeat, takewhile, dropwhile
"""
import itertools
import operator
from typing import Iterator


# ── 1. 연결 & 슬라이싱 ───────────────────────────────────────────────────
def chain_islice_demo() -> None:
    print("=== 1. chain & islice ===")

    # chain: 여러 이터러블을 하나로 연결 (메모리 복사 없음)
    nums = list(itertools.chain([1, 2], [3, 4], range(5, 8)))
    print(f"  chain: {nums}")

    # chain.from_iterable: 중첩 리스트 평탄화
    nested = [[1, 2], [3, 4], [5, 6]]
    flat = list(itertools.chain.from_iterable(nested))
    print(f"  chain.from_iterable: {flat}")

    # islice: lazy 슬라이싱 — 무한 이터레이터에서 필요한 만큼만 추출
    def naturals() -> Iterator[int]:
        n = 1
        while True:
            yield n
            n += 1

    page2 = list(itertools.islice(naturals(), 10, 20))  # 11~20
    print(f"  islice(10, 20): {page2}")


# ── 2. groupby ────────────────────────────────────────────────────────────
def groupby_demo() -> None:
    print("\n=== 2. groupby ===")

    # groupby는 연속으로 같은 키가 나올 때 그룹화 → 정렬 후 사용해야 함
    employees = [
        {"name": "철수", "dept": "개발팀", "score": 88},
        {"name": "민준", "dept": "개발팀", "score": 75},
        {"name": "영희", "dept": "기획팀", "score": 92},
        {"name": "하늘", "dept": "마케팅", "score": 81},
        {"name": "도현", "dept": "마케팅", "score": 69},
        {"name": "지수", "dept": "기획팀", "score": 95},
    ]

    # 부서별 그룹화 (정렬 필수)
    sorted_emp = sorted(employees, key=lambda e: e["dept"])
    print("  부서별 평균 점수:")
    for dept, members in itertools.groupby(sorted_emp, key=lambda e: e["dept"]):
        scores = [m["score"] for m in members]
        print(f"    {dept}: {scores}  평균={sum(scores)/len(scores):.1f}")


# ── 3. product / combinations / permutations ─────────────────────────────
def combinatorics_demo() -> None:
    print("\n=== 3. 조합론 ===")

    # product: 데카르트 곱 (중첩 for 루프 대체)
    sizes   = ["S", "M", "L"]
    colors  = ["빨강", "파랑"]
    items   = list(itertools.product(sizes, colors))
    print(f"  product (재고 조합): {len(items)}개")
    for item in items:
        print(f"    {item}")

    # 동전 던지기 3번 — repeat 활용
    tosses = list(itertools.product("HT", repeat=3))
    print(f"\n  동전 3회 경우의 수: {len(tosses)}가지")

    # combinations: 순서 없는 선택
    team = ["A", "B", "C", "D", "E"]
    pairs = list(itertools.combinations(team, 2))
    print(f"\n  combinations(5명 중 2명): {len(pairs)}쌍")
    print(f"    {pairs}")

    # combinations_with_replacement: 중복 허용
    dice = list(itertools.combinations_with_replacement(range(1, 4), 2))
    print(f"\n  주사위 2개 (중복 허용): {dice}")

    # permutations: 순서 있는 선택
    podium = list(itertools.permutations(["금", "은", "동"], 2))
    print(f"\n  permutations(메달 2개 배치): {len(podium)}가지")


# ── 4. accumulate ─────────────────────────────────────────────────────────
def accumulate_demo() -> None:
    print("\n=== 4. accumulate ===")

    sales = [120, 85, 200, 150, 95, 180, 210]

    # 누적 합 (기본)
    cumsum = list(itertools.accumulate(sales))
    print(f"  일별 매출: {sales}")
    print(f"  누적 합:   {cumsum}")

    # 누적 최대값
    cummax = list(itertools.accumulate(sales, max))
    print(f"  누적 최대: {cummax}")

    # 누적 곱 (팩토리얼)
    factorial_10 = list(itertools.accumulate(range(1, 11), operator.mul))
    print(f"  1!~10!:    {factorial_10}")

    # initial 값 설정 (Python 3.8+)
    running = list(itertools.accumulate(sales, operator.add, initial=0))
    print(f"  initial=0: {running}")


# ── 5. takewhile / dropwhile / filterfalse ───────────────────────────────
def filtering_demo() -> None:
    print("\n=== 5. takewhile / dropwhile / filterfalse ===")

    temps = [22, 24, 27, 31, 35, 38, 36, 30, 25, 20]

    # takewhile: 조건이 False가 되는 순간 중단
    cool = list(itertools.takewhile(lambda t: t < 30, temps))
    print(f"  30도 미만 연속: {cool}")

    # dropwhile: 조건이 False가 되는 순간부터 시작
    hot_season = list(itertools.dropwhile(lambda t: t < 30, temps))
    print(f"  30도 이상 시작: {hot_season}")

    # filterfalse: filter 반대 (조건 False인 것만)
    odds = list(itertools.filterfalse(lambda x: x % 2 == 0, range(10)))
    print(f"  홀수: {odds}")


# ── 6. cycle / repeat / starmap ──────────────────────────────────────────
def misc_demo() -> None:
    print("\n=== 6. cycle / repeat / starmap ===")

    # cycle: 무한 순환 (islice로 제한)
    colors = list(itertools.islice(itertools.cycle(["빨강", "초록", "파랑"]), 8))
    print(f"  cycle(8개): {colors}")

    # repeat: 같은 값 N번 반복
    zeros = list(itertools.repeat(0, 5))
    print(f"  repeat(0, 5): {zeros}")

    # starmap: map의 다중 인자 버전
    pairs = [(2, 3), (3, 4), (4, 5)]
    powered = list(itertools.starmap(pow, pairs))
    print(f"  starmap(pow): {powered}")  # [8, 81, 1024]


# ── main ─────────────────────────────────────────────────────────────────
def main() -> None:
    chain_islice_demo()
    groupby_demo()
    combinatorics_demo()
    accumulate_demo()
    filtering_demo()
    misc_demo()


if __name__ == "__main__":
    main()
