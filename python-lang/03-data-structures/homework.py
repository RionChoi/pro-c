"""
Python-03 homework.py
주제: dict 고급 활용, set 연산, collections (defaultdict, Counter, deque, namedtuple)
"""
from collections import defaultdict, Counter, deque, namedtuple
from typing import Any


# ── 1. dict 고급 활용 ─────────────────────────────────────────────────────
def dict_advanced() -> None:
    print("=== 1. dict 고급 활용 ===")

    # setdefault: 키 없을 때만 삽입 (있으면 기존 값 유지)
    scores: dict[str, list[int]] = {}
    for name, score in [("철수", 85), ("영희", 92), ("철수", 78), ("영희", 88)]:
        scores.setdefault(name, []).append(score)
    print(f"  setdefault: {scores}")

    # get with default
    config = {"host": "localhost", "port": 8080}
    print(f"  get: {config.get('timeout', 30)}s (기본값)")

    # dict 병합 (Python 3.9+: | 연산자)
    defaults = {"debug": False, "timeout": 30, "host": "localhost"}
    overrides = {"debug": True, "host": "prod.server.com"}
    merged = defaults | overrides          # overrides가 우선
    print(f"  병합: {merged}")

    # dict comprehension으로 변환
    inverted = {v: k for k, v in {"a": 1, "b": 2, "c": 3}.items()}
    print(f"  역전: {inverted}")

    # 조건부 dict 생성
    data = {"name": "리온", "age": 25, "score": None, "city": ""}
    clean = {k: v for k, v in data.items() if v}   # falsy 값 제거
    print(f"  falsy 제거: {clean}")


# ── 2. set 연산 ───────────────────────────────────────────────────────────
def set_operations() -> None:
    print("\n=== 2. set 연산 ===")

    python_devs = {"철수", "영희", "민준", "지수"}
    go_devs     = {"영희", "지수", "하늘", "도현"}

    print(f"  교집합 (둘 다):  {python_devs & go_devs}")
    print(f"  합집합 (하나라도): {python_devs | go_devs}")
    print(f"  차집합 (Python만): {python_devs - go_devs}")
    print(f"  대칭차 (한쪽만): {python_devs ^ go_devs}")
    subset = {"영희", "지수"}
    print(f"  부분집합?: {subset <= python_devs}")

    # frozenset: 불변 set (dict 키나 set 원소로 사용 가능)
    tags1 = frozenset(["python", "backend"])
    tags2 = frozenset(["python", "ml"])
    tag_groups = {tags1: "백엔드 팀", tags2: "ML 팀"}
    print(f"  frozenset 키: {tag_groups[tags1]}")


# ── 3. defaultdict ────────────────────────────────────────────────────────
def defaultdict_demo() -> None:
    print("\n=== 3. defaultdict ===")

    # 그래프 인접 리스트 — KeyError 없이 바로 append
    graph: dict[str, list[str]] = defaultdict(list)
    edges = [("A", "B"), ("A", "C"), ("B", "D"), ("C", "D"), ("D", "E")]
    for src, dst in edges:
        graph[src].append(dst)
    print(f"  그래프: {dict(graph)}")

    # 그룹핑 — SQL의 GROUP BY와 동일
    employees = [
        ("개발팀", "철수"), ("마케팅", "영희"), ("개발팀", "민준"),
        ("기획팀", "지수"), ("마케팅", "하늘"), ("개발팀", "도현"),
    ]
    by_dept: dict[str, list[str]] = defaultdict(list)
    for dept, name in employees:
        by_dept[dept].append(name)
    for dept, members in sorted(by_dept.items()):
        print(f"  {dept}: {members}")

    # int factory: 빈도 집계 (Counter 전 단계)
    word_count: dict[str, int] = defaultdict(int)
    for word in "the quick brown fox jumps over the lazy dog the".split():
        word_count[word] += 1
    top3 = sorted(word_count.items(), key=lambda t: -t[1])[:3]
    print(f"  상위 3개: {top3}")


# ── 4. Counter ────────────────────────────────────────────────────────────
def counter_demo() -> None:
    print("\n=== 4. Counter ===")

    text = "python is great and python is fast and python rocks"
    cnt = Counter(text.split())

    print(f"  most_common(3): {cnt.most_common(3)}")
    print(f"  'python' 빈도: {cnt['python']}")
    print(f"  없는 키: {cnt['java']} (KeyError 없음)")

    # Counter 산술 연산
    a = Counter({"사과": 3, "바나나": 2, "체리": 1})
    b = Counter({"바나나": 1, "체리": 2, "포도": 4})
    print(f"  합산: {dict(a + b)}")
    print(f"  교집합(최솟값): {dict(a & b)}")
    print(f"  합집합(최댓값): {dict(a | b)}")

    # 문자 빈도 분석
    char_cnt = Counter("mississippi")
    print(f"  문자 빈도: {char_cnt.most_common()}")


# ── 5. deque ──────────────────────────────────────────────────────────────
def deque_demo() -> None:
    print("\n=== 5. deque ===")

    # 양방향 O(1) push/pop — list.insert(0, x)는 O(n)
    dq: deque[int] = deque([3, 4, 5])
    dq.appendleft(2)
    dq.appendleft(1)
    dq.append(6)
    print(f"  deque: {list(dq)}")
    print(f"  popleft: {dq.popleft()}  남은: {list(dq)}")

    # maxlen: 슬라이딩 윈도우 / 최근 N개 유지
    recent: deque[str] = deque(maxlen=3)
    for page in ["홈", "소개", "제품", "블로그", "문의"]:
        recent.append(page)
    print(f"  최근 3페이지: {list(recent)}")

    # rotate: 원형 버퍼처럼 회전
    jobs: deque[str] = deque(["작업A", "작업B", "작업C", "작업D"])
    jobs.rotate(-1)    # 왼쪽으로 회전 (앞 원소를 뒤로)
    print(f"  rotate(-1): {list(jobs)}")


# ── 6. namedtuple ─────────────────────────────────────────────────────────
def namedtuple_demo() -> None:
    print("\n=== 6. namedtuple ===")

    # 불변 레코드 — tuple보다 가독성, dict보다 메모리 효율
    Point3D = namedtuple("Point3D", ["x", "y", "z"])
    Student = namedtuple("Student", ["name", "score", "grade"])

    p = Point3D(1.0, 2.5, -3.0)
    print(f"  점: {p}  x={p.x}  인덱스: {p[1]}")
    print(f"  _asdict: {p._asdict()}")

    # _replace: 일부 필드만 바꾼 새 인스턴스 (불변이므로 복사)
    p2 = p._replace(z=0.0)
    print(f"  _replace: {p2}")

    students = [
        Student("영희", 92, "A"),
        Student("철수", 85, "B"),
        Student("민준", 78, "C"),
    ]
    ranked = sorted(students, key=lambda s: s.score, reverse=True)
    for i, s in enumerate(ranked, 1):
        print(f"  {i}위 {s.name}: {s.score}점 ({s.grade})")


# ── main ─────────────────────────────────────────────────────────────────
def main() -> None:
    dict_advanced()
    set_operations()
    defaultdict_demo()
    counter_demo()
    deque_demo()
    namedtuple_demo()


if __name__ == "__main__":
    main()
