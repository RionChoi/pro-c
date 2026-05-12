"""
Python-04 game1.py
RPG 아이템 시스템 — dataclass + ABC + Protocol + dunder 종합
"""
from __future__ import annotations
from abc import ABC, abstractmethod
from dataclasses import dataclass, field
from typing import Protocol, runtime_checkable
import math


# ── 아이템 추상 기반 ──────────────────────────────────────────────────────

class Item(ABC):
    """모든 아이템의 추상 기반 클래스."""

    @property
    @abstractmethod
    def name(self) -> str: ...

    @property
    @abstractmethod
    def value(self) -> int: ...

    @abstractmethod
    def description(self) -> str: ...

    def __str__(self) -> str:
        return f"[{self.name}] {self.description()} (가격: {self.value}골드)"


# ── 장비 아이템 ───────────────────────────────────────────────────────────

@dataclass(order=True)
class Weapon(Item):
    _name: str
    damage: int
    speed: float           # 공격 속도 (0.5 ~ 2.0)
    durability: int = 100
    _value: int = field(init=False, repr=False)

    def __post_init__(self) -> None:
        self._value = int(self.damage * self.speed * 10)

    @property
    def name(self) -> str:   return self._name
    @property
    def value(self) -> int:  return self._value

    @property
    def dps(self) -> float:
        return self.damage * self.speed

    def description(self) -> str:
        return f"공격력 {self.damage}, 속도 {self.speed:.1f}, DPS {self.dps:.1f}"

    # 사용 시 내구도 감소
    def use(self) -> str:
        if self.durability <= 0:
            return f"{self._name}이(가) 파손되었습니다."
        self.durability -= 10
        return f"{self._name} 사용 (내구도: {self.durability})"


@dataclass
class Armor(Item):
    _name: str
    defense: int
    weight: float
    _value: int = field(init=False, repr=False)

    def __post_init__(self) -> None:
        self._value = self.defense * 8

    @property
    def name(self) -> str:   return self._name
    @property
    def value(self) -> int:  return self._value

    def description(self) -> str:
        return f"방어력 {self.defense}, 무게 {self.weight:.1f}kg"


@dataclass
class Potion(Item):
    _name: str
    heal_amount: int
    quantity: int = 1

    @property
    def name(self) -> str:   return self._name
    @property
    def value(self) -> int:  return self.heal_amount * 2

    def description(self) -> str:
        return f"HP +{self.heal_amount} (보유: {self.quantity}개)"

    def drink(self) -> str:
        if self.quantity <= 0:
            return "포션이 없습니다."
        self.quantity -= 1
        return f"{self._name} 사용: HP +{self.heal_amount}"


# ── 인벤토리 (컨테이너 dunder) ────────────────────────────────────────────

class Inventory:
    """__len__, __iter__, __contains__, __getitem__ 구현."""

    MAX_SLOTS = 20

    def __init__(self, owner: str) -> None:
        self._owner  = owner
        self._items: list[Item] = []
        self._gold   = 0

    @property
    def gold(self) -> int: return self._gold

    def earn(self, amount: int) -> None:
        self._gold += amount

    def add(self, item: Item) -> bool:
        if len(self._items) >= self.MAX_SLOTS:
            print(f"  인벤토리 가득 참 ({self.MAX_SLOTS}칸)")
            return False
        self._items.append(item)
        return True

    def remove(self, item: Item) -> bool:
        try:
            self._items.remove(item)
            return True
        except ValueError:
            return False

    def sell(self, item: Item) -> str:
        if self.remove(item):
            self._gold += item.value
            return f"  판매: {item.name} → +{item.value}골드 (보유: {self._gold}골드)"
        return f"  {item.name}을(를) 소지하지 않습니다."

    # ── 컨테이너 dunder ──
    def __len__(self)             -> int:  return len(self._items)
    def __iter__(self):                    return iter(self._items)
    def __contains__(self, item)  -> bool: return item in self._items
    def __getitem__(self, idx: int) -> Item: return self._items[idx]

    def __repr__(self) -> str:
        return (f"Inventory({self._owner}, "
                f"{len(self._items)}/{self.MAX_SLOTS}칸, {self._gold}골드)")

    def display(self) -> None:
        print(f"\n  ── {self._owner}의 인벤토리 ({len(self)}칸, {self._gold}골드) ──")
        if not self._items:
            print("    (비어 있음)")
            return
        for i, item in enumerate(self._items, 1):
            print(f"    {i:2}. {item}")

    def find_by_type(self, item_type: type) -> list[Item]:
        return [i for i in self._items if isinstance(i, item_type)]

    def total_value(self) -> int:
        return sum(i.value for i in self._items)


# ── Sellable Protocol ─────────────────────────────────────────────────────

@runtime_checkable
class Sellable(Protocol):
    @property
    def value(self) -> int: ...
    @property
    def name(self) -> str: ...


# ── 상점 ─────────────────────────────────────────────────────────────────

class Shop:
    def __init__(self) -> None:
        self._stock: list[Item] = [
            Weapon("불꽃 검", damage=45, speed=1.2),
            Weapon("단검",   damage=25, speed=1.8),
            Armor("철갑옷",  defense=30, weight=8.5),
            Armor("가죽 갑옷", defense=15, weight=3.0),
            Potion("체력 포션", heal_amount=50, quantity=5),
            Potion("대형 포션", heal_amount=150, quantity=2),
        ]

    def display(self) -> None:
        print("\n  ── 상점 목록 ──")
        weapons  = sorted([i for i in self._stock if isinstance(i, Weapon)],
                          key=lambda w: w.dps, reverse=True)
        others   = [i for i in self._stock if not isinstance(i, Weapon)]
        for item in weapons + others:
            print(f"    {item}")

    def buy(self, inv: Inventory, item_name: str) -> str:
        item = next((i for i in self._stock if i.name == item_name), None)
        if not item:
            return f"  '{item_name}'을(를) 찾을 수 없습니다."
        if inv.gold < item.value:
            return f"  골드 부족 ({inv.gold} < {item.value})"
        inv.earn(-item.value)
        inv.add(item)
        self._stock.remove(item)
        return f"  구매: {item.name} (-{item.value}골드)"


# ── main ─────────────────────────────────────────────────────────────────

def main() -> None:
    print("⚔️  RPG 아이템 시스템\n")

    inv  = Inventory("리온")
    shop = Shop()

    inv.earn(3000)

    shop.display()

    print("\n--- 구매 ---")
    print(shop.buy(inv, "불꽃 검"))
    print(shop.buy(inv, "체력 포션"))
    print(shop.buy(inv, "철갑옷"))

    inv.display()

    print("\n--- 아이템 사용 ---")
    sword = inv.find_by_type(Weapon)[0]
    print(f"  {sword.use()}")
    potion = inv.find_by_type(Potion)[0]
    print(f"  {potion.drink()}")

    print("\n--- 판매 ---")
    print(inv.sell(sword))

    inv.display()

    # 컨테이너 dunder 확인
    print(f"\n--- 인벤토리 통계 ---")
    print(f"  총 아이템: {len(inv)}개")
    print(f"  총 가치:   {inv.total_value()}골드")
    print(f"  Sellable 체크: {all(isinstance(i, Sellable) for i in inv)}")

    # dataclass order 비교
    print(f"\n--- 무기 DPS 비교 ---")
    w1 = Weapon("검A", 40, 1.0)
    w2 = Weapon("검B", 30, 1.5)
    print(f"  {w1.name} DPS={w1.dps:.1f} vs {w2.name} DPS={w2.dps:.1f}")
    print(f"  검A > 검B (가치 기준): {w1 > w2}")


if __name__ == "__main__":
    main()
