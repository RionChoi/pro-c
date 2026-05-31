#include <iostream>
#include <tuple>
#include <string>
#include <vector>
#include <variant>
#include <cstdlib>
#include <ctime>

struct Warrior {
    std::string name;
    int hp;
    int attack;
    int defense;
};

struct Mage {
    std::string name;
    int hp;
    int magic_power;
    int mana;
};

struct Rogue {
    std::string name;
    int hp;
    int agility;
    int evasion;
};

using Character = std::variant<Warrior, Mage, Rogue>;

// 리플렉션 기반 캐릭터 정보 출력
template <typename T>
void print_character_info(const T& ch) {
    if constexpr (std::is_same_v<T, Warrior>) {
        std::cout << "  클래스: 전사\n";
        std::cout << "  이름: " << ch.name << "\n";
        std::cout << "  체력: " << ch.hp << "\n";
        std::cout << "  공격력: " << ch.attack << "\n";
        std::cout << "  방어력: " << ch.defense << "\n";
    } else if constexpr (std::is_same_v<T, Mage>) {
        std::cout << "  클래스: 마법사\n";
        std::cout << "  이름: " << ch.name << "\n";
        std::cout << "  체력: " << ch.hp << "\n";
        std::cout << "  마법력: " << ch.magic_power << "\n";
        std::cout << "  마나: " << ch.mana << "\n";
    } else if constexpr (std::is_same_v<T, Rogue>) {
        std::cout << "  클래스: 도적\n";
        std::cout << "  이름: " << ch.name << "\n";
        std::cout << "  체력: " << ch.hp << "\n";
        std::cout << "  민첩성: " << ch.agility << "\n";
        std::cout << "  회피율: " << ch.evasion << "%\n";
    }
}

// 리플렉션 기반 공격력 계산
template <typename T>
int get_attack_power(const T& ch) {
    if constexpr (std::is_same_v<T, Warrior>) {
        return ch.attack;
    } else if constexpr (std::is_same_v<T, Mage>) {
        return ch.magic_power;
    } else if constexpr (std::is_same_v<T, Rogue>) {
        return ch.agility;
    }
    return 0;
}

// 리플렉션 기반 방어력 계산
template <typename T>
int get_defense(const T& ch) {
    if constexpr (std::is_same_v<T, Warrior>) {
        return ch.defense;
    } else if constexpr (std::is_same_v<T, Mage>) {
        return 5;
    } else if constexpr (std::is_same_v<T, Rogue>) {
        return ch.evasion / 2;
    }
    return 0;
}

// 방문자 패턴으로 Character 동작 처리
struct CharacterVisitor {
    void operator()(Warrior& ch) const {
        std::cout << "전사의 강력한 검 공격!\n";
        ch.hp = std::min(100, ch.hp + 5);
    }

    void operator()(Mage& ch) const {
        if (ch.mana >= 20) {
            std::cout << "마법사의 마법 공격!\n";
            ch.mana -= 20;
        } else {
            std::cout << "마나 부족!\n";
        }
    }

    void operator()(Rogue& ch) const {
        std::cout << "도적의 민첩한 공격!\n";
        ch.hp = std::max(0, ch.hp - 10);
    }
};

int main() {
    srand(time(nullptr));

    std::cout << "=== 리플렉션 기반 RPG 게임 ===\n\n";

    Warrior warrior{"강철 검사", 100, 25, 15};
    Mage mage{"신비한 마법사", 60, 30, 100};
    Rogue rogue{"그림자 도적", 70, 20, 40};

    std::vector<Character> party = {warrior, mage, rogue};

    std::cout << "=== 파티 구성 ===\n";
    int idx = 1;
    for (const auto& ch : party) {
        std::cout << "[" << idx << "]\n";
        std::visit([](const auto& c) { print_character_info(c); }, ch);
        std::cout << "\n";
        idx++;
    }

    // 전투 시뮬레이션
    std::cout << "=== 전투 ===\n";
    int enemy_hp = 200;
    int turn = 1;

    while (enemy_hp > 0 && turn <= 10) {
        std::cout << "\n[ 턴 " << turn << " ]\n";
        std::cout << "적의 체력: " << enemy_hp << "\n";

        for (auto& ch : party) {
            int damage = std::visit(
                [](auto& c) { return get_attack_power(c) + (rand() % 10); },
                ch);
            enemy_hp -= damage;
            std::cout << "공격력: " << damage << "\n";
        }

        turn++;
    }

    if (enemy_hp <= 0) {
        std::cout << "\n전투에서 승리했습니다!\n";
    } else {
        std::cout << "\n전투에서 패배했습니다!\n";
    }

    return 0;
}
