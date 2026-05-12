"""
Python-04 homework2.py
주제: ABC, Protocol (구조적 서브타이핑), __slots__, 믹스인 패턴
"""
from __future__ import annotations
from abc import ABC, abstractmethod
from typing import Protocol, runtime_checkable
import sys


# ── 1. ABC — 추상 기본 클래스 (C++ 순수 가상 함수와 대응) ───────────────

class Shape(ABC):
    """모든 도형의 추상 기반 클래스."""

    @abstractmethod
    def area(self) -> float: ...

    @abstractmethod
    def perimeter(self) -> float: ...

    # ABC에도 구체 메서드를 넣을 수 있다
    def describe(self) -> str:
        return (f"{type(self).__name__}: "
                f"넓이={self.area():.2f}, 둘레={self.perimeter():.2f}")


class Rectangle(Shape):
    def __init__(self, w: float, h: float) -> None:
        self.w, self.h = w, h

    def area(self) -> float:      return self.w * self.h
    def perimeter(self) -> float: return 2 * (self.w + self.h)


class Circle(Shape):
    import math as _math
    def __init__(self, r: float) -> None:
        self.r = r

    def area(self) -> float:
        import math; return math.pi * self.r ** 2

    def perimeter(self) -> float:
        import math; return 2 * math.pi * self.r


def abc_demo() -> None:
    print("=== 1. ABC (추상 기본 클래스) ===")

    shapes: list[Shape] = [Rectangle(4, 6), Circle(5)]
    for s in shapes:
        print(f"  {s.describe()}")

    # ABC 인스턴스화 시도 → TypeError
    try:
        Shape()
    except TypeError as e:
        print(f"  Shape() 직접 생성 불가: {e}")

    # isinstance로 추상 타입 체크
    print(f"  isinstance(Circle(1), Shape): {isinstance(Circle(1), Shape)}")


# ── 2. Protocol — 구조적 서브타이핑 (덕 타이핑의 정적 버전) ─────────────
# C++의 concept (C++20) / Go의 interface와 유사
# 상속 없이 메서드만 맞으면 타입 호환

@runtime_checkable
class Drawable(Protocol):
    def draw(self) -> str: ...
    def bounding_box(self) -> tuple[float, float, float, float]: ...


class Sprite:
    """Shape를 상속하지 않지만 Drawable 프로토콜을 만족한다."""
    def __init__(self, x: float, y: float, w: float, h: float) -> None:
        self.x, self.y, self.w, self.h = x, y, w, h

    def draw(self) -> str:
        return f"Sprite at ({self.x},{self.y}) size {self.w}×{self.h}"

    def bounding_box(self) -> tuple[float, float, float, float]:
        return self.x, self.y, self.x + self.w, self.y + self.h


class UIButton:
    def draw(self) -> str:
        return "Button[OK]"

    def bounding_box(self) -> tuple[float, float, float, float]:
        return 0.0, 0.0, 80.0, 30.0


def render(obj: Drawable) -> None:
    print(f"  렌더: {obj.draw()}  bbox={obj.bounding_box()}")


def protocol_demo() -> None:
    print("\n=== 2. Protocol (구조적 서브타이핑) ===")

    items: list[Drawable] = [Sprite(10, 20, 50, 50), UIButton()]
    for item in items:
        render(item)

    # runtime_checkable 덕분에 isinstance 사용 가능
    print(f"  isinstance(Sprite, Drawable): {isinstance(Sprite(0,0,1,1), Drawable)}")
    print(f"  isinstance(int, Drawable):    {isinstance(42, Drawable)}")


# ── 3. __slots__ — 메모리 최적화 ────────────────────────────────────────

class PointNormal:
    def __init__(self, x: float, y: float) -> None:
        self.x, self.y = x, y


class PointSlotted:
    __slots__ = ("x", "y")

    def __init__(self, x: float, y: float) -> None:
        self.x, self.y = x, y


def slots_demo() -> None:
    print("\n=== 3. __slots__ 메모리 비교 ===")

    n = PointNormal(1.0, 2.0)
    s = PointSlotted(1.0, 2.0)

    size_n = sys.getsizeof(n) + sys.getsizeof(n.__dict__)
    size_s = sys.getsizeof(s)

    print(f"  일반 인스턴스: {size_n} bytes (객체 + __dict__)")
    print(f"  __slots__ 인스턴스: {size_s} bytes")
    print(f"  절약: {size_n - size_s} bytes ({(size_n-size_s)/size_n*100:.0f}%)")

    # __slots__는 동적 속성 추가 불가
    try:
        s.z = 3.0
    except AttributeError as e:
        print(f"  동적 속성 추가 불가: {e}")


# ── 4. 믹스인 패턴 — 다중 상속으로 기능 조합 ────────────────────────────

class JsonMixin:
    """dict 변환 기능을 어떤 클래스에도 붙일 수 있는 믹스인."""
    def to_dict(self) -> dict:
        return {k: v for k, v in self.__dict__.items() if not k.startswith("_")}

    def to_json_str(self) -> str:
        import json
        return json.dumps(self.to_dict(), ensure_ascii=False)


class ValidateMixin:
    """필드 검증 기능 믹스인."""
    def validate(self) -> list[str]:
        errors: list[str] = []
        for attr, value in self.__dict__.items():
            if value is None:
                errors.append(f"{attr} 값이 없습니다.")
        return errors


class Product(JsonMixin, ValidateMixin):
    def __init__(self, name: str, price: float, stock: int) -> None:
        self.name  = name
        self.price = price
        self.stock = stock

    def __repr__(self) -> str:
        return f"Product({self.name}, {self.price}원, 재고:{self.stock})"


def mixin_demo() -> None:
    print("\n=== 4. 믹스인 패턴 ===")

    p = Product("맥북", 2_500_000.0, 5)
    print(f"  {p}")
    print(f"  to_dict:     {p.to_dict()}")
    print(f"  to_json_str: {p.to_json_str()}")
    print(f"  validate:    {p.validate() or '오류 없음'}")

    # MRO(Method Resolution Order) 확인
    print(f"  MRO: {[c.__name__ for c in Product.__mro__]}")


# ── main ─────────────────────────────────────────────────────────────────

def main() -> None:
    abc_demo()
    protocol_demo()
    slots_demo()
    mixin_demo()


if __name__ == "__main__":
    main()
