"""
Python-01 game1.py
성적 분석 도구 — comprehension, 내장 함수, f-string, type hint 종합 활용
"""
from typing import TypedDict


class Student(TypedDict):
    name: str
    scores: list[int]  # 수학, 영어, 과학, 국어, 역사


SUBJECTS = ["수학", "영어", "과학", "국어", "역사"]


def average(scores: list[int]) -> float:
    return sum(scores) / len(scores)


def to_grade(avg: float) -> str:
    if avg >= 90: return "A"
    if avg >= 80: return "B"
    if avg >= 70: return "C"
    if avg >= 60: return "D"
    return "F"


def analyze(students: list[Student]) -> None:
    # 학생별 평균/등급 계산 — dict comprehension
    stats = {
        s["name"]: {
            "avg":   (avg := average(s["scores"])),
            "grade": to_grade(avg),
            "best":  SUBJECTS[s["scores"].index(max(s["scores"]))],
            "worst": SUBJECTS[s["scores"].index(min(s["scores"]))],
        }
        for s in students
    }

    # 석차 정렬 — sorted + key
    ranked = sorted(stats.items(), key=lambda t: t[1]["avg"], reverse=True)

    print("=" * 62)
    print(f"{'석차':^4} {'이름':^6} {'평균':^6} {'등급':^4} {'최고과목':^8} {'최저과목':^8}")
    print("-" * 62)
    for rank, (name, info) in enumerate(ranked, 1):
        print(f"{rank:^4} {name:^6} {info['avg']:^6.1f} {info['grade']:^4}"
              f" {info['best']:^8} {info['worst']:^8}")
    print("=" * 62)

    # 과목별 통계 — zip + comprehension
    print("\n[과목별 통계]")
    subject_scores: dict[str, list[int]] = {
        subj: [s["scores"][i] for s in students]
        for i, subj in enumerate(SUBJECTS)
    }
    for subj, sc in subject_scores.items():
        avg_s = average(sc)
        bar   = "▓" * int(avg_s // 5) + "░" * (20 - int(avg_s // 5))
        print(f"  {subj:<4} {avg_s:5.1f}점  {bar}  최고:{max(sc)} 최저:{min(sc)}")

    # 전체 통계
    all_avgs = [info["avg"] for info in stats.values()]
    print(f"\n  전체 평균: {average(all_avgs):.1f}점")
    print(f"  90점 이상: {sum(1 for a in all_avgs if a >= 90)}명")
    print(f"  60점 미만: {sum(1 for a in all_avgs if a < 60)}명")

    # 개인 상세 리포트
    print("\n[개인 점수 상세]")
    for s in students:
        row = "  ".join(f"{subj}:{sc:3}" for subj, sc in zip(SUBJECTS, s["scores"]))
        print(f"  {s['name']}: {row}")


def main() -> None:
    students: list[Student] = [
        {"name": "김철수", "scores": [88, 75, 92, 85, 79]},
        {"name": "이영희", "scores": [95, 98, 91, 96, 93]},
        {"name": "박민준", "scores": [72, 68, 80, 75, 71]},
        {"name": "최지수", "scores": [60, 55, 63, 70, 58]},
        {"name": "정하늘", "scores": [84, 90, 87, 82, 88]},
    ]

    print("📊 성적 분석 도구 — Python 핵심 특성 종합")
    print()
    analyze(students)


if __name__ == "__main__":
    main()
