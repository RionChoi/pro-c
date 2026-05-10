"""
Python-02 game1.py
데이터 처리 파이프라인 — decorator + generator + functools 종합
실제 ETL(Extract-Transform-Load) 패턴을 간단히 구현한다.
"""
import functools
import time
from typing import Callable, Iterator, Any


# ── Decorator: 파이프라인 단계를 꾸미는 도구들 ────────────────────────────

def stage(name: str) -> Callable:
    """파이프라인 단계에 이름을 붙이고 처리 건수를 출력하는 decorator."""
    def decorator(func: Callable) -> Callable:
        @functools.wraps(func)
        def wrapper(data: Iterator, *args: Any, **kwargs: Any) -> Iterator:
            print(f"  ▶ [{name}] 시작")
            count = 0
            for item in func(data, *args, **kwargs):
                count += 1
                yield item
            print(f"  ✓ [{name}] 완료 → {count}건 출력")
        return wrapper
    return decorator


def timed(func: Callable) -> Callable:
    """전체 파이프라인 실행 시간을 측정한다."""
    @functools.wraps(func)
    def wrapper(*args: Any, **kwargs: Any) -> Any:
        start = time.perf_counter()
        result = func(*args, **kwargs)
        elapsed = time.perf_counter() - start
        print(f"\n  ⏱  총 실행 시간: {elapsed*1000:.1f}ms")
        return result
    return wrapper


# ── Generator 기반 파이프라인 단계들 ─────────────────────────────────────

@stage("Extract")
def extract(source: Iterator) -> Iterator[dict]:
    """원시 데이터를 dict 레코드로 변환한다."""
    for raw in source:
        yield {"raw": raw, "processed": False}


@stage("Parse")
def parse(records: Iterator[dict]) -> Iterator[dict]:
    """문자열 레코드에서 필드를 파싱한다. (이름,점수,부서 형식)"""
    for rec in records:
        parts = rec["raw"].split(",")
        if len(parts) != 3:
            continue                   # 잘못된 형식 건너뜀
        name, score_str, dept = (p.strip() for p in parts)
        try:
            score = int(score_str)
        except ValueError:
            continue
        yield {"name": name, "score": score, "dept": dept}


@stage("Filter")
def filter_valid(records: Iterator[dict], min_score: int = 0) -> Iterator[dict]:
    """점수 범위 밖의 레코드를 제거한다."""
    for rec in records:
        if 0 <= rec["score"] <= 100 and rec["score"] >= min_score:
            yield rec


@stage("Transform")
def transform(records: Iterator[dict]) -> Iterator[dict]:
    """등급 계산 및 보너스 포인트를 추가한다."""
    grade_map = [(90, "A"), (80, "B"), (70, "C"), (60, "D")]

    for rec in records:
        s = rec["score"]
        grade = next((g for threshold, g in grade_map if s >= threshold), "F")
        yield {**rec, "grade": grade, "bonus": round(s * 0.1, 1)}


@stage("Aggregate")
def aggregate(records: Iterator[dict]) -> Iterator[dict]:
    """부서별로 집계한다. (전체를 소비한 뒤 결과를 yield)"""
    dept_data: dict[str, list[int]] = {}
    all_records = list(records)          # generator를 소비해 메모리에 적재

    for rec in all_records:
        dept_data.setdefault(rec["dept"], []).append(rec["score"])

    for dept, scores in sorted(dept_data.items()):
        avg = sum(scores) / len(scores)
        yield {
            "dept": dept,
            "count": len(scores),
            "avg": round(avg, 1),
            "max": max(scores),
            "min": min(scores),
        }


# ── 파이프라인 실행 ────────────────────────────────────────────────────────

@timed
def run_pipeline(raw_data: list[str], min_score: int = 60) -> list[dict]:
    """
    Extract → Parse → Filter → Transform → Aggregate
    generator 체이닝: 각 단계는 lazy하게 처리된다.
    """
    pipeline = aggregate(
        transform(
            filter_valid(
                parse(
                    extract(iter(raw_data))
                ),
                min_score=min_score,
            )
        )
    )
    return list(pipeline)


# ── 결과 출력 ─────────────────────────────────────────────────────────────

def print_report(results: list[dict]) -> None:
    print("\n📊 부서별 성적 집계 리포트")
    print("=" * 48)
    print(f"{'부서':<10} {'인원':>4} {'평균':>6} {'최고':>4} {'최저':>4}")
    print("-" * 48)
    for r in results:
        bar = "█" * int(r["avg"] // 10)
        print(f"{r['dept']:<10} {r['count']:>4}명  {r['avg']:>5.1f}점  "
              f"{r['max']:>4}  {r['min']:>4}  {bar}")
    print("=" * 48)


# ── main ─────────────────────────────────────────────────────────────────

def main() -> None:
    # 원시 CSV 형식 데이터 (일부 불량 데이터 포함)
    raw_data = [
        "김철수, 88, 개발팀",
        "이영희, 95, 기획팀",
        "박민준, 72, 개발팀",
        "최지수, 61, 마케팅",
        "정하늘, 84, 개발팀",
        "한소희, 101, 개발팀",   # 범위 초과 → 필터링
        "오류레코드",              # 형식 오류 → 파싱 제외
        "강지훈, abc, 기획팀",    # 점수 파싱 오류 → 제외
        "윤서연, 78, 마케팅",
        "임도현, 55, 기획팀",     # 60점 미만 → min_score 필터링
        "신예린, 91, 마케팅",
        "조민서, 67, 개발팀",
    ]

    print("🔧 데이터 처리 파이프라인 (ETL 패턴)")
    print(f"  입력: {len(raw_data)}건  |  최소 점수: 60점\n")

    results = run_pipeline(raw_data, min_score=60)
    print_report(results)


if __name__ == "__main__":
    main()
