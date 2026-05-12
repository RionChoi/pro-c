"""
Python-04 homework.py
주제: @dataclass 고급 옵션 + dunder 메서드 (Python 고유 OOP)
전제: C++의 클래스/상속/연산자 오버로딩은 이미 알고 있음
"""
from __future__ import annotations
from dataclasses import dataclass, field, fields, astuple, asdict
from typing import Iterator
import math


# ── 1. @dataclass 기본 → 고급 옵션 ──────────────────────────────────────

@dataclass
class Vector2D:
    """2D 벡터 — dataclass + dunder 메서드 조합."""
    x: float
    y: float

    # dunder: 산술 연산자
    def __add__(self, other: Vector2D) -> Vector2D:
        return Vector2D(self.x + other.x, self.y + other.y)

    def __sub__(self, other: Vector2D) -> Vector2D:
        return Vector2D(self.x - other.x, self.y - other.y)

    def __mul__(self, scalar: float) -> Vector2D:
        return Vector2D(self.x * scalar, self.y * scalar)

    def __rmul__(self, scalar: float) -> Vector2D:   # 3.0 * v 도 동작
        return self.__mul__(scalar)

    def __neg__(self) -> Vector2D:
        return Vector2D(-self.x, -self.y)

    def __abs__(self) -> float:                       # abs(v) → 크기
        return math.hypot(self.x, self.y)

    def __bool__(self) -> bool:                       # if v:
        return abs(self) != 0.0

    # dunder: 문자열 표현
    def __repr__(self) -> str:
        return f"Vector2D({self.x}, {self.y})"

    def __str__(self) -> str:
        return f"({self.x:.2f}, {self.y:.2f})"

    def dot(self, other: Vector2D) -> float:
        return self.x * other.x + self.y * other.y


def vector_demo() -> None:
    print("=== 1. @dataclass + dunder 산술 연산 ===")
    v1 = Vector2D(3.0, 4.0)
    v2 = Vector2D(1.0, 2.0)

    print(f"  v1 = {v1}  |v1| = {abs(v1):.2f}")
    print(f"  v1 + v2 = {v1 + v2}")
    print(f"  v1 - v2 = {v1 - v2}")
    print(f"  2.5 * v1 = {2.5 * v1}")
    print(f"  -v1 = {-v1}")
    print(f"  v1 · v2 = {v1.dot(v2):.2f}")
    print(f"  bool(v1) = {bool(v1)}  bool(Vector2D(0,0)) = {bool(Vector2D(0,0))}")


# ── 2. @dataclass 고급 옵션 ──────────────────────────────────────────────

@dataclass(order=True)
class Student:
    """order=True: __lt__, __le__, __gt__, __ge__ 자동 생성."""
    # sort_index를 첫 필드로 두어 정렬 기준으로 사용
    sort_index: float = field(init=False, repr=False)
    name: str
    score: float
    grade: str = field(default="", compare=False)   # 비교/정렬에서 제외

    def __post_init__(self) -> None:
        # init 직후 자동 호출 — 파생 필드 계산 (C++ 생성자 본문과 동일 역할)
        self.sort_index = -self.score               # 점수 내림차순 정렬
        if not self.grade:
            self.grade = self._calc_grade()

    def _calc_grade(self) -> str:
        if self.score >= 90: return "A"
        if self.score >= 80: return "B"
        if self.score >= 70: return "C"
        return "D"


def dataclass_advanced_demo() -> None:
    print("\n=== 2. @dataclass 고급 (order, field, __post_init__) ===")

    students = [
        Student("영희", 92.0),
        Student("철수", 85.5),
        Student("민준", 78.0),
        Student("지수", 95.0),
    ]

    for s in sorted(students):        # order=True 덕분에 sorted() 바로 사용
        print(f"  {s.name}: {s.score}점 ({s.grade})")

    # asdict / astuple — JSON 직렬화, DB 저장에 유용
    s = students[0]
    print(f"\n  asdict:  {asdict(s)}")
    print(f"  astuple: {astuple(s)}")
    print(f"  fields:  {[f.name for f in fields(s)]}")


# ── 3. @property — 지연 계산 & 유효성 검사 ──────────────────────────────

class Circle:
    def __init__(self, radius: float) -> None:
        self.radius = radius           # setter를 통해 검증

    @property
    def radius(self) -> float:
        return self._radius

    @radius.setter
    def radius(self, value: float) -> None:
        if value < 0:
            raise ValueError(f"반지름은 0 이상이어야 합니다: {value}")
        self._radius = value

    @property
    def diameter(self) -> float:       # 읽기 전용 계산 프로퍼티
        return self._radius * 2

    @property
    def area(self) -> float:
        return math.pi * self._radius ** 2

    @property
    def circumference(self) -> float:
        return 2 * math.pi * self._radius

    def __repr__(self) -> str:
        return f"Circle(r={self._radius:.2f})"


def property_demo() -> None:
    print("\n=== 3. @property ===")

    c = Circle(5.0)
    print(f"  {c}")
    print(f"  지름: {c.diameter:.2f}  넓이: {c.area:.2f}  둘레: {c.circumference:.2f}")

    c.radius = 10.0
    print(f"  반지름 변경 후 넓이: {c.area:.2f}")

    try:
        c.radius = -1.0
    except ValueError as e:
        print(f"  ValueError: {e}")


# ── 4. 컨테이너 프로토콜 dunder ──────────────────────────────────────────

class NumberSet:
    """__len__, __getitem__, __contains__, __iter__, __reversed__ 구현."""

    def __init__(self, *nums: int) -> None:
        self._data: list[int] = sorted(set(nums))

    def __len__(self) -> int:           return len(self._data)
    def __getitem__(self, i: int) -> int: return self._data[i]
    def __contains__(self, item: object) -> bool: return item in self._data
    def __iter__(self) -> Iterator[int]:  return iter(self._data)
    def __reversed__(self) -> Iterator[int]: return reversed(self._data)
    def __repr__(self) -> str:          return f"NumberSet({self._data})"

    def __or__(self, other: NumberSet) -> NumberSet:   # 합집합
        return NumberSet(*self._data, *other._data)

    def __and__(self, other: NumberSet) -> NumberSet:  # 교집합
        return NumberSet(*[x for x in self._data if x in other])


def container_dunder_demo() -> None:
    print("\n=== 4. 컨테이너 dunder ===")

    ns = NumberSet(3, 1, 4, 1, 5, 9, 2, 6, 5)
    print(f"  {ns}")
    print(f"  len={len(ns)}  ns[0]={ns[0]}  5 in ns: {5 in ns}")
    print(f"  list(ns): {list(ns)}")
    print(f"  reversed: {list(reversed(ns))}")

    ns2 = NumberSet(4, 5, 6, 7, 8)
    print(f"  합집합: {ns | ns2}")
    print(f"  교집합: {ns & ns2}")


# ── main ─────────────────────────────────────────────────────────────────

def main() -> None:
    vector_demo()
    dataclass_advanced_demo()
    property_demo()
    container_dunder_demo()


if __name__ == "__main__":
    main()
