#include "contract.hpp"

#include <iostream>
#include <string>
#include <vector>

class Player {
public:
    Player(int health, int energy, int potions)
        : health_(health),
          energy_(energy),
          potions_(potions)
    {
        CONTRACT_PRE(health > 0);
        CONTRACT_PRE(energy >= 0);
        CONTRACT_PRE(potions >= 0);
        assertInvariant();
    }

    void attack(int monsterPower)
    {
        CONTRACT_PRE_MSG(alive(), "dead player cannot attack");
        CONTRACT_PRE_MSG(monsterPower > 0, "monster power must be positive");
        CONTRACT_PRE_MSG(energy_ >= attackCost, "not enough energy to attack");
        const int beforeEnergy = energy_;

        energy_ -= attackCost;
        health_ -= monsterPower / 2;
        if (health_ < 0) {
            health_ = 0;
        }

        CONTRACT_POST(energy_ == beforeEnergy - attackCost);
        assertInvariant();
    }

    void rest()
    {
        CONTRACT_PRE_MSG(alive(), "dead player cannot rest");
        const int beforeEnergy = energy_;

        energy_ += restGain;
        if (energy_ > maxEnergy) {
            energy_ = maxEnergy;
        }

        CONTRACT_POST(energy_ >= beforeEnergy);
        assertInvariant();
    }

    void drinkPotion()
    {
        CONTRACT_PRE_MSG(alive(), "dead player cannot use potion");
        CONTRACT_PRE_MSG(potions_ > 0, "no potion left");
        const int beforeHealth = health_;
        const int beforePotions = potions_;

        --potions_;
        health_ += potionHeal;
        if (health_ > maxHealth) {
            health_ = maxHealth;
        }

        CONTRACT_POST(potions_ == beforePotions - 1);
        CONTRACT_POST(health_ >= beforeHealth);
        assertInvariant();
    }

    bool alive() const
    {
        return health_ > 0;
    }

    void print() const
    {
        std::cout << "HP=" << health_ << " EN=" << energy_ << " POT=" << potions_ << '\n';
    }

private:
    void assertInvariant() const
    {
        CONTRACT_ASSERT(health_ >= 0 && health_ <= maxHealth);
        CONTRACT_ASSERT(energy_ >= 0 && energy_ <= maxEnergy);
        CONTRACT_ASSERT(potions_ >= 0);
    }

    static constexpr int maxHealth = 100;
    static constexpr int maxEnergy = 40;
    static constexpr int attackCost = 12;
    static constexpr int restGain = 9;
    static constexpr int potionHeal = 25;

    int health_;
    int energy_;
    int potions_;
};

struct Turn {
    std::string action;
    int monsterPower;
};

void runTurn(Player& player, const Turn& turn)
{
    std::cout << "action: " << turn.action << '\n';

    if (turn.action == "attack") {
        player.attack(turn.monsterPower);
    } else if (turn.action == "rest") {
        player.rest();
    } else if (turn.action == "potion") {
        player.drinkPotion();
    } else {
        CONTRACT_PRE_MSG(false, "unknown action");
    }

    player.print();
}

int main()
{
    Player player(70, 24, 1);
    const std::vector<Turn> script{
        {"attack", 18},
        {"rest", 0},
        {"potion", 0},
        {"potion", 0},
    };

    player.print();
    for (const Turn& turn : script) {
        try {
            runTurn(player, turn);
        } catch (const lesson_contracts::ContractViolation& error) {
            const lesson_contracts::ViolationInfo& info = error.info();
            std::cout << "blocked by " << lesson_contracts::kindName(info.kind) << ": "
                      << info.message << '\n';
            player.print();
        }
    }

    return 0;
}
