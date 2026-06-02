#include <iostream>
#include <string>

class Character {
    std::string name;
    int level = 1;
    std::string characterClass;
    std::string race;
    int health = 100;
    int mana = 50;

public:
    Character& withName(const std::string& n) {
        name = n;
        return *this;
    }

    Character& withLevel(int l) {
        level = l;
        health = 100 + (l - 1) * 20;
        mana = 50 + (l - 1) * 10;
        return *this;
    }

    Character& withClass(const std::string& c) {
        characterClass = c;
        if (c == "Warrior") {
            health += 50;
        } else if (c == "Mage") {
            mana += 100;
        } else if (c == "Rogue") {
            health += 20;
        }
        return *this;
    }

    Character& withRace(const std::string& r) {
        race = r;
        if (r == "Dwarf") {
            health += 30;
        } else if (r == "Elf") {
            mana += 30;
        } else if (r == "Human") {
            level += 1;
        }
        return *this;
    }

    void display() const {
        std::cout << "[ 캐릭터: " << name << " ]" << std::endl;
        std::cout << "  레벨: " << level << std::endl;
        std::cout << "  직업: " << characterClass << std::endl;
        std::cout << "  종족: " << race << std::endl;
        std::cout << "  체력: " << health << " HP" << std::endl;
        std::cout << "  마나: " << mana << " MP" << std::endl;
    }
};

int main() {
    std::cout << "=== 게임 캐릭터 생성 DSL ===" << std::endl;

    std::cout << "\n캐릭터 1: 기본 워리어" << std::endl;
    Character hero1;
    hero1
        .withName("Dragon Slayer")
        .withLevel(5)
        .withClass("Warrior")
        .withRace("Human")
        .display();

    std::cout << "\n캐릭터 2: 마법사" << std::endl;
    Character hero2;
    hero2
        .withName("Archmage")
        .withLevel(3)
        .withClass("Mage")
        .withRace("Elf")
        .display();

    std::cout << "\n캐릭터 3: 암살자" << std::endl;
    Character hero3;
    hero3
        .withName("Shadow")
        .withLevel(4)
        .withClass("Rogue")
        .withRace("Dwarf")
        .display();

    std::cout << "\n✓ DSL을 사용해 게임 캐릭터를 유연하게 생성했습니다!" << std::endl;

    return 0;
}
