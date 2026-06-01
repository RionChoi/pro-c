#include <iostream>
#include <type_traits>
#include <array>
#include <string>

template<typename T>
struct CharacterTraits {
    static constexpr bool isPhysical = std::is_same_v<T, int>;
    static constexpr bool isMagical = std::is_same_v<T, double>;
};

constexpr int calculateDamage(int baseAttack, int level) {
    return baseAttack + level * 5;
}

constexpr double calculateMagicDamage(double baseSpell, int level) {
    return baseSpell + level * 0.5;
}

constexpr int calculateHealth(int level) {
    return 100 + level * 20;
}

constexpr int levelUpExperience(int level) {
    return level * level * 100;
}

template<typename DamageType>
struct Character {
    std::string name;
    int level;
    int experience;
    int health;
    DamageType damage;

    void displayInfo() const {
        std::cout << "\n=== " << name << " ===\n";
        std::cout << "레벨: " << level << "\n";
        std::cout << "체력: " << health << "\n";

        if constexpr (CharacterTraits<DamageType>::isPhysical) {
            std::cout << "공격력(물리): " << damage << "\n";
            std::cout << "타입: 전사\n";
        } else if constexpr (CharacterTraits<DamageType>::isMagical) {
            std::cout << "공격력(마법): " << damage << "\n";
            std::cout << "타입: 마법사\n";
        }

        std::cout << "경험치: " << experience << "/" << levelUpExperience(level) << "\n";
    }

    void gainExperience(int exp) {
        experience += exp;
        std::cout << exp << " 경험치 획득!\n";

        while (experience >= levelUpExperience(level)) {
            experience -= levelUpExperience(level);
            level++;
            health = calculateHealth(level);

            if constexpr (CharacterTraits<DamageType>::isPhysical) {
                damage = calculateDamage(damage, 1);
            } else if constexpr (CharacterTraits<DamageType>::isMagical) {
                damage = calculateMagicDamage(damage, 1);
            }

            std::cout << "레벨 업! -> Lv" << level << "\n";
        }
    }

    int takeDamage(int dmg) {
        health -= dmg;
        if (health < 0) health = 0;
        return health;
    }
};

template<typename T>
constexpr int getDamageValue(T value) {
    if constexpr (std::is_same_v<T, int>) {
        return static_cast<int>(value);
    } else if constexpr (std::is_floating_point_v<T>) {
        return static_cast<int>(value);
    }
    return 0;
}

int main() {
    Character<int> warrior{"전사", 1, 0, calculateHealth(1), calculateDamage(10, 1)};
    Character<double> mage{"마법사", 1, 0, calculateHealth(1), calculateMagicDamage(8.0, 1)};

    std::cout << "=== 메타프로그래밍 RPG 게임 ===\n";
    std::cout << "명령어: attack, magic, rest, status, switch, quit\n\n";

    bool isWarrior = true;

    while (true) {
        if (isWarrior) {
            std::cout << "[Lv" << warrior.level << "] > ";
        } else {
            std::cout << "[Lv" << mage.level << "] > ";
        }

        std::string command;
        std::cin >> command;

        if (command == "attack") {
            if (isWarrior) {
                int actualDamage = calculateDamage(warrior.damage, warrior.level);
                std::cout << warrior.name << "의 공격! " << actualDamage << " 피해\n";
                warrior.gainExperience(50);
            } else {
                std::cout << "전사의 공격이 아닙니다!\n";
            }
        } else if (command == "magic") {
            if (!isWarrior) {
                double actualDamage = calculateMagicDamage(mage.damage, mage.level);
                std::cout << mage.name << "의 마법 공격! " << actualDamage << " 피해\n";
                mage.gainExperience(60);
            } else {
                std::cout << "마법을 사용할 수 없습니다!\n";
            }
        } else if (command == "rest") {
            int maxHealth = calculateHealth(isWarrior ? warrior.level : mage.level);
            if (isWarrior) {
                warrior.health = maxHealth;
            } else {
                mage.health = maxHealth;
            }
            std::cout << "휴식을 취했습니다. 체력이 회복되었습니다.\n";
        } else if (command == "status") {
            warrior.displayInfo();
            mage.displayInfo();
        } else if (command == "switch") {
            isWarrior = !isWarrior;
            if (isWarrior) {
                std::cout << "전사로 전환했습니다.\n";
            } else {
                std::cout << "마법사로 전환했습니다.\n";
            }
        } else if (command == "quit") {
            std::cout << "게임 종료\n";
            break;
        } else {
            std::cout << "알 수 없는 명령어\n";
        }
    }

    return 0;
}
