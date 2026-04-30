#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Monster {
protected:
    std::string name;
    int hp;

public:
    Monster(std::string n, int health) : name(std::move(n)), hp(health) {}
    virtual ~Monster() = default;

    virtual int attack() const = 0;

    void takeDamage(int damage) {
        hp -= damage;
        if (hp < 0) {
            hp = 0;
        }
    }

    bool alive() const {
        return hp > 0;
    }

    const std::string& getName() const {
        return name;
    }

    int getHp() const {
        return hp;
    }
};

class Slime : public Monster {
public:
    Slime() : Monster("Slime", 30) {}

    int attack() const override {
        return 5;
    }
};

class Dragon : public Monster {
public:
    Dragon() : Monster("Dragon", 80) {}

    int attack() const override {
        return 14;
    }
};

int main() {
    std::vector<std::unique_ptr<Monster>> monsters;
    monsters.push_back(std::make_unique<Slime>());
    monsters.push_back(std::make_unique<Dragon>());

    int playerHp = 100;

    for (const auto& monster : monsters) {
        std::cout << "A wild " << monster->getName() << " appeared!\n";
        while (monster->alive() && playerHp > 0) {
            monster->takeDamage(12);
            if (!monster->alive()) {
                std::cout << monster->getName() << " defeated!\n";
                break;
            }

            playerHp -= monster->attack();
            std::cout << monster->getName() << " attacks! Player HP: " << playerHp
                      << ", Monster HP: " << monster->getHp() << '\n';
        }

        if (playerHp <= 0) {
            std::cout << "Player was defeated...\n";
            return 0;
        }
    }

    std::cout << "All monsters defeated!\n";
    return 0;
}
