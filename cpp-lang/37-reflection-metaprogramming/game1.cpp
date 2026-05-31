#include <iostream>
#include <tuple>
#include <string>
#include <type_traits>

class Sword {
public:
    int damage = 10;
    void use() const { std::cout << "휘두르다! 데미지: " << damage << '\n'; }
};

class Shield {
public:
    int defense = 5;
    void use() const { std::cout << "방어하다! 방어력: " << defense << '\n'; }
};

class Potion {
public:
    int healing = 30;
    void use() const { std::cout << "약을 마시다! 회복: " << healing << '\n'; }
};

template<typename Item>
struct item_name {
    static constexpr const char* value = "Unknown";
};

template<>
struct item_name<Sword> {
    static constexpr const char* value = "Sword";
};

template<>
struct item_name<Shield> {
    static constexpr const char* value = "Shield";
};

template<>
struct item_name<Potion> {
    static constexpr const char* value = "Potion";
};

template<typename T>
inline constexpr const char* item_name_v = item_name<T>::value;

template<typename... Items>
class Inventory {
    std::tuple<Items...> items;

public:
    Inventory() = default;

    void print_inventory() const {
        std::cout << "인벤토리 (" << tuple_size_v << " 아이템):\n";
        print_items(std::index_sequence_for<Items...>{});
    }

    template<typename Item>
    void use_item() {
        use_item_impl<Item>(std::index_sequence_for<Items...>{});
    }

    static constexpr size_t tuple_size_v = sizeof...(Items);

private:
    template<typename Item, size_t... I>
    void use_item_impl(std::index_sequence<I...>) {
        bool found = false;
        ([this, &found](auto idx) {
            if (!found && std::is_same_v<Item, std::tuple_element_t<idx, std::tuple<Items...>>>) {
                std::cout << "사용: " << item_name_v<Item> << " -> ";
                std::get<idx>(items).use();
                found = true;
            }
        }(std::integral_constant<size_t, I>{}), ...);

        if (!found) {
            std::cout << "인벤토리에 없는 아이템입니다.\n";
        }
    }

    template<size_t... I>
    void print_items(std::index_sequence<I...>) const {
        ([this](auto idx) {
            std::cout << "  [" << (idx + 1) << "] " << get_item_name(idx) << '\n';
        }(std::integral_constant<size_t, I>{}), ...);
    }

    template<size_t Index>
    const char* get_item_name(std::integral_constant<size_t, Index>) const {
        using ItemType = std::tuple_element_t<Index, std::tuple<Items...>>;
        return item_name_v<ItemType>;
    }
};

template<typename... Items>
void play_adventure(Inventory<Items...>& inv) {
    std::cout << "\n=== 어드벤처 게임 ===\n";
    inv.print_inventory();

    std::cout << "\n몬스터가 나타났다!\n";
    std::cout << "1. 칼로 공격 2. 방어 3. 약을 마시기\n선택: ";
    int choice;
    std::cin >> choice;

    switch (choice) {
        case 1:
            inv.template use_item<Sword>();
            break;
        case 2:
            inv.template use_item<Shield>();
            break;
        case 3:
            inv.template use_item<Potion>();
            break;
        default:
            std::cout << "잘못된 선택\n";
    }
}

int main() {
    std::cout << "=== 타입 안전 인벤토리 시스템 ===\n";

    Inventory<Sword, Shield, Potion> inv1;
    play_adventure(inv1);

    std::cout << "\n---\n";

    Inventory<Potion, Potion, Sword> inv2;
    play_adventure(inv2);

    return 0;
}
