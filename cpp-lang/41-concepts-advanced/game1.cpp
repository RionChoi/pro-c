// Session 41: C++20 Concepts 심화 — RPG 아이템 시스템 (Concept 기반 타입 안전)
#include <concepts>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>
#include <random>
#include <limits>
#include <functional>

// ─────────────────────────────────────────────
// Concept 정의: RPG 아이템 인터페이스
// ─────────────────────────────────────────────

template <typename T>
concept Item = requires(T t) {
    { t.name()     } -> std::same_as<std::string_view>;
    { t.weight()   } -> std::floating_point;
    { t.value()    } -> std::integral;
    { t.describe() } -> std::same_as<std::string>;
};

template <typename T>
concept Equippable = Item<T> && requires(T t) {
    { t.slot()      } -> std::same_as<std::string_view>;
    { t.stat_bonus()} -> std::integral;
};

template <typename T>
concept Consumable = Item<T> && requires(T t) {
    { t.use()       } -> std::same_as<std::string>;
    { t.charges()   } -> std::integral;
};

template <typename T>
concept Stackable = Consumable<T> && requires(T& t, int n) {
    { t.add_stack(n) } -> std::same_as<void>;
    { t.max_stack()  } -> std::integral;
};

// ─────────────────────────────────────────────
// 아이템 구현
// ─────────────────────────────────────────────

struct Sword {
    std::string name_{"철제 검"};
    float  weight_{3.5f};
    int    value_{120};
    int    attack_{25};

    std::string_view name()     const { return name_; }
    float            weight()   const { return weight_; }
    int              value()    const { return value_; }
    std::string_view slot()     const { return "무기"; }
    int              stat_bonus()const{ return attack_; }
    std::string      describe() const {
        return std::string(name_) + " (공격력 +" + std::to_string(attack_) + ")";
    }
};

struct Shield {
    std::string name_{"나무 방패"};
    float  weight_{2.0f};
    int    value_{80};
    int    defense_{15};

    std::string_view name()     const { return name_; }
    float            weight()   const { return weight_; }
    int              value()    const { return value_; }
    std::string_view slot()     const { return "방어구"; }
    int              stat_bonus()const{ return defense_; }
    std::string      describe() const {
        return std::string(name_) + " (방어력 +" + std::to_string(defense_) + ")";
    }
};

struct Potion {
    std::string name_{"HP 포션"};
    float  weight_{0.3f};
    int    value_{50};
    int    charges_{3};
    int    heal_{40};

    std::string_view name()    const { return name_; }
    float            weight()  const { return weight_; }
    int              value()   const { return value_; }
    int              charges() const { return charges_; }
    int              max_stack()const{ return 10; }
    void             add_stack(int n){ charges_ = std::min(charges_ + n, max_stack()); }
    std::string      use() {
        if (charges_ <= 0) return "포션이 소진되었습니다.";
        --charges_;
        return "HP가 " + std::to_string(heal_) + " 회복되었습니다. (남은 횟수: "
               + std::to_string(charges_) + ")";
    }
    std::string describe() const {
        return std::string(name_) + " (x" + std::to_string(charges_) + ")";
    }
};

struct Bomb {
    std::string name_{"화염 폭탄"};
    float  weight_{0.8f};
    int    value_{70};
    int    charges_{2};
    int    damage_{60};

    std::string_view name()    const { return name_; }
    float            weight()  const { return weight_; }
    int              value()   const { return value_; }
    int              charges() const { return charges_; }
    std::string      use() {
        if (charges_ <= 0) return "폭탄이 없습니다.";
        --charges_;
        return "폭발! 적에게 " + std::to_string(damage_) + " 피해. (남은: "
               + std::to_string(charges_) + ")";
    }
    std::string describe() const {
        return std::string(name_) + " (x" + std::to_string(charges_) + ")";
    }
};

// ─────────────────────────────────────────────
// Concept 기반 인벤토리 관리자
// ─────────────────────────────────────────────

class Inventory {
    struct Slot {
        std::string name;
        double weight;
        int    value;
        std::string description;
        std::function<std::string()> use_fn;  // nullable
    };

    std::vector<Slot> slots_;
    double weight_limit_{20.0};

public:
    template <Item T>
    bool add(T&& item) {
        double total = current_weight() + static_cast<double>(item.weight());
        if (total > weight_limit_) {
            std::cout << "  [무게 초과] " << item.name() << " 추가 불가\n";
            return false;
        }
        std::function<std::string()> use_fn;
        if constexpr (Consumable<T>) {
            use_fn = [item_copy = std::forward<T>(item)]() mutable {
                return item_copy.use();
            };
        }
        slots_.push_back({
            std::string(item.name()),
            static_cast<double>(item.weight()),
            item.value(),
            item.describe(),
            std::move(use_fn)
        });
        return true;
    }

    double current_weight() const {
        double w = 0.0;
        for (auto& s : slots_) w += s.weight;
        return w;
    }

    void show() const {
        std::cout << "  ─── 인벤토리 (" << slots_.size() << "칸, "
                  << current_weight() << "/" << weight_limit_ << "kg) ───\n";
        for (std::size_t i = 0; i < slots_.size(); ++i) {
            auto& s = slots_[i];
            std::cout << "    [" << (i+1) << "] " << s.description
                      << " | " << s.weight << "kg | " << s.value << "G";
            if (s.use_fn) std::cout << " [사용 가능]";
            std::cout << '\n';
        }
    }

    std::string use_item(std::size_t index) {
        if (index >= slots_.size()) return "잘못된 슬롯";
        auto& s = slots_[index];
        if (!s.use_fn) return s.name + "은 사용할 수 없는 아이템입니다.";
        return s.use_fn();
    }

    int total_value() const {
        int sum = 0;
        for (auto& s : slots_) sum += s.value;
        return sum;
    }
};

// ─────────────────────────────────────────────
// Concept 제약 검증 출력
// ─────────────────────────────────────────────

void print_item_traits() {
    std::cout << "=== 아이템 Concept 검증 ===\n";
    std::cout << "Item<Sword>:       " << Item<Sword>       << '\n';
    std::cout << "Item<Potion>:      " << Item<Potion>       << '\n';
    std::cout << "Equippable<Sword>: " << Equippable<Sword> << '\n';
    std::cout << "Equippable<Potion>:" << Equippable<Potion> << '\n';
    std::cout << "Consumable<Potion>:" << Consumable<Potion> << '\n';
    std::cout << "Consumable<Sword>: " << Consumable<Sword>  << '\n';
    std::cout << "Stackable<Potion>: " << Stackable<Potion>  << '\n';
    std::cout << "Stackable<Bomb>:   " << Stackable<Bomb>    << '\n';
    std::cout << '\n';
}

// ─────────────────────────────────────────────
// main 게임 루프
// ─────────────────────────────────────────────

int main() {
    print_item_traits();

    std::cout << "=== RPG 아이템 시뮬레이션 ===\n\n";

    Inventory inv;

    // 아이템 추가
    std::cout << "[ 아이템 획득 ]\n";
    inv.add(Sword{});
    inv.add(Shield{});
    inv.add(Potion{});
    inv.add(Potion{"고급 HP 포션", 0.3f, 150, 5, 100});
    inv.add(Bomb{});
    std::cout << '\n';

    inv.show();
    std::cout << "  총 가치: " << inv.total_value() << "G\n\n";

    // 아이템 사용
    std::cout << "[ 아이템 사용 ]\n";
    std::cout << "  3번 슬롯: " << inv.use_item(2) << '\n';
    std::cout << "  3번 슬롯: " << inv.use_item(2) << '\n';
    std::cout << "  5번 슬롯: " << inv.use_item(4) << '\n';
    std::cout << "  1번 슬롯: " << inv.use_item(0) << '\n';  // 검은 사용 불가
    std::cout << '\n';

    // 포션 스택 추가
    std::cout << "[ Stackable 포션에 스택 추가 ]\n";
    Potion stackable{"회복 포션", 0.3f, 50, 1, 40};
    std::cout << "  현재 충전: " << stackable.charges() << '\n';
    stackable.add_stack(4);
    std::cout << "  +4 추가 후: " << stackable.charges() << '\n';
    stackable.add_stack(20);  // max_stack=10 초과
    std::cout << "  +20 시도 후: " << stackable.charges() << " (최대 " << stackable.max_stack() << ")\n";
    std::cout << '\n';

    std::cout << "=== 게임 종료 ===\n";
    return 0;
}
