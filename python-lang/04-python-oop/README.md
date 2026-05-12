# Python-04: Python OOP 고유 특성

## 전제 조건
C++-07~10에서 클래스, 생성자/소멸자, 상속, 다형성, 연산자 오버로딩을 이미 학습했다.
이 세션은 **Python에만 있는** OOP 메커니즘에만 집중한다.

---

## C++에서 Python으로 — 달라지는 것들

| C++ | Python 대응 | 차이점 |
|-----|------------|--------|
| `struct` / 멤버 변수 선언 | `@dataclass` | 타입 힌트만으로 자동 `__init__` 생성 |
| `operator==`, `operator<` | `__eq__`, `__lt__` | dunder 메서드로 모든 연산자 제어 |
| `private:` / getter/setter | `@property` | 데코레이터로 접근 제어, 지연 계산 |
| 순수 가상 함수 (`= 0`) | `ABC` + `@abstractmethod` | 추상 클래스 강제 |
| `sizeof` 최적화 | `__slots__` | 인스턴스 메모리 절약 |

---

## 1. @dataclass

```python
from dataclasses import dataclass, field

@dataclass(order=True, frozen=True)   # order: 비교 연산자 자동 생성, frozen: 불변
class Point:
    x: float
    y: float
    label: str = field(default="", compare=False)  # 비교에서 제외

p1 = Point(1.0, 2.0)
p2 = Point(3.0, 4.0)
print(p1 < p2)   # order=True 덕분에 자동 동작
```

## 2. Dunder 메서드

`__repr__`, `__str__`, `__len__`, `__getitem__`, `__contains__`, `__iter__`, `__enter__`/`__exit__` 등으로 Python 프로토콜에 통합된다.

```python
class Stack:
    def __init__(self): self._data = []
    def __len__(self):  return len(self._data)       # len(stack)
    def __repr__(self): return f"Stack({self._data})" # repr()
    def __iter__(self): return iter(reversed(self._data))  # for x in stack
    def __contains__(self, item): return item in self._data  # x in stack
```

## 3. @property

```python
class Circle:
    def __init__(self, radius: float):
        self._radius = radius

    @property
    def radius(self) -> float:        # getter
        return self._radius

    @radius.setter
    def radius(self, value: float):   # setter — 유효성 검사 포함
        if value < 0:
            raise ValueError("반지름은 0 이상이어야 합니다.")
        self._radius = value

    @property
    def area(self) -> float:          # 읽기 전용 계산 프로퍼티
        import math
        return math.pi * self._radius ** 2
```

## 4. ABC (추상 기본 클래스)

```python
from abc import ABC, abstractmethod

class Shape(ABC):
    @abstractmethod
    def area(self) -> float: ...      # 반드시 구현해야 함

    @abstractmethod
    def perimeter(self) -> float: ...

class Circle(Shape):
    def area(self): ...
    def perimeter(self): ...
```

## 5. __slots__

인스턴스 `__dict__` 대신 고정 슬롯을 사용해 메모리를 절약한다.

```python
class Point:
    __slots__ = ("x", "y")   # 허용된 속성만 존재, ~40% 메모리 절약
    def __init__(self, x, y):
        self.x, self.y = x, y
```

---

## 과제 목표

| 파일 | 내용 |
|------|------|
| `homework.py` | `@dataclass` 고급 옵션 + dunder 메서드 완전 실습 |
| `homework2.py` | `ABC` + `Protocol` + `__slots__` + 믹스인 패턴 |
| `game1.py` | RPG 아이템 시스템 — dataclass + ABC + dunder 종합 |
