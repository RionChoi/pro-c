// Strategy 패턴: 전사 캐릭터 전투 게임
// 전투 전략(공격/방어/도주)을 런타임에 교체하며 몬스터를 처치한다.
#include <iostream>
#include <string>
#include <memory>
#include <cstdlib>
#include <ctime>

// 전투 전략 인터페이스
class CombatStrategy {
public:
    virtual ~CombatStrategy() = default;
    // 공격력, 피해 감소율을 반환한다.
    virtual int  attack()       const = 0;
    virtual int  defense()      const = 0;
    virtual std::string label() const = 0;
};

// 전략 1: 공격적 — 높은 데미지, 낮은 방어
class AggressiveStrategy : public CombatStrategy {
public:
    int  attack()       const override { return 25; }
    int  defense()      const override { return 5;  }
    std::string label() const override { return "공격적 (Aggressive)"; }
};

// 전략 2: 방어적 — 낮은 데미지, 높은 방어
class DefensiveStrategy : public CombatStrategy {
public:
    int  attack()       const override { return 8;  }
    int  defense()      const override { return 20; }
    std::string label() const override { return "방어적 (Defensive)"; }
};

// 전략 3: 균형 — 중간 데미지, 중간 방어
class BalancedStrategy : public CombatStrategy {
public:
    int  attack()       const override { return 15; }
    int  defense()      const override { return 12; }
    std::string label() const override { return "균형 (Balanced)"; }
};

// 캐릭터 — 전략을 보유하고 런타임에 교체할 수 있다.
class Warrior {
public:
    Warrior(const std::string& name, int hp)
        : name_(name), hp_(hp), strategy_(std::make_unique<BalancedStrategy>()) {}

    void setStrategy(std::unique_ptr<CombatStrategy> strategy) {
        strategy_ = std::move(strategy);
        std::cout << name_ << "의 전략 변경 → " << strategy_->label() << "\n";
    }

    int  hp()   const { return hp_; }
    bool alive()const { return hp_ > 0; }

    int performAttack() const {
        int dmg = strategy_->attack() + (std::rand() % 6);
        std::cout << name_ << " 공격! 데미지: " << dmg << "\n";
        return dmg;
    }

    void takeDamage(int incoming) {
        int actual = std::max(0, incoming - strategy_->defense());
        hp_ -= actual;
        if (hp_ < 0) hp_ = 0;
        std::cout << name_ << " 피해 " << actual
                  << " (방어 " << strategy_->defense() << " 흡수)"
                  << " → HP: " << hp_ << "\n";
    }

    const std::string& name() const { return name_; }

private:
    std::string                      name_;
    int                              hp_;
    std::unique_ptr<CombatStrategy>  strategy_;
};

// 몬스터 (고정 능력치)
struct Monster {
    std::string name;
    int hp;
    int attackPower;
};

void printSeparator() {
    std::cout << "----------------------------------------\n";
}

void choosingStrategy(Warrior& warrior) {
    std::cout << "\n전략을 선택하세요:\n"
              << "  1. 공격적 — 공격 25, 방어  5\n"
              << "  2. 방어적 — 공격  8, 방어 20\n"
              << "  3. 균형   — 공격 15, 방어 12\n"
              << "> ";
    int choice;
    if (!(std::cin >> choice)) {
        std::cin.clear();
        std::cin.ignore(256, '\n');
        choice = 3;
    }
    switch (choice) {
        case 1: warrior.setStrategy(std::make_unique<AggressiveStrategy>()); break;
        case 2: warrior.setStrategy(std::make_unique<DefensiveStrategy>());  break;
        default: warrior.setStrategy(std::make_unique<BalancedStrategy>());  break;
    }
}

void battle(Warrior& warrior, Monster& monster) {
    printSeparator();
    std::cout << "⚔️  전투 시작: " << warrior.name()
              << " (HP:" << warrior.hp() << ") vs "
              << monster.name << " (HP:" << monster.hp << ")\n";
    printSeparator();

    choosingStrategy(warrior);

    int round = 1;
    while (warrior.alive() && monster.hp > 0) {
        std::cout << "\n[라운드 " << round++ << "]\n";

        // 플레이어 공격
        int dmg = warrior.performAttack();
        monster.hp -= dmg;
        if (monster.hp < 0) monster.hp = 0;
        std::cout << monster.name << " HP: " << monster.hp << "\n";

        if (monster.hp == 0) break;

        // 몬스터 공격
        int mDmg = monster.attackPower + (std::rand() % 5);
        std::cout << monster.name << " 반격! 데미지: " << mDmg << "\n";
        warrior.takeDamage(mDmg);
    }

    printSeparator();
    if (warrior.alive()) {
        std::cout << "✅ " << warrior.name() << " 승리! "
                  << monster.name << " 처치\n";
    } else {
        std::cout << "💀 " << warrior.name() << " 패배...\n";
    }
    printSeparator();
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    std::cout << "=== Strategy 패턴 전투 게임 ===\n";
    std::cout << "각 전투 전 전략을 선택하세요. 전략에 따라 공격/방어 성능이 달라집니다.\n\n";

    Warrior hero("용사 리온", 100);

    Monster goblin  {"고블린",   40, 12};
    Monster orc     {"오크",     70, 18};
    Monster dragon  {"드래곤", 120, 28};

    battle(hero, goblin);
    if (!hero.alive()) { std::cout << "\n게임 오버\n"; return 0; }

    battle(hero, orc);
    if (!hero.alive()) { std::cout << "\n게임 오버\n"; return 0; }

    battle(hero, dragon);
    if (!hero.alive()) {
        std::cout << "\n게임 오버\n";
    } else {
        std::cout << "\n🎉 축하합니다! 모든 몬스터를 처치했습니다!\n";
        std::cout << "최종 HP: " << hero.hp() << "\n";
    }

    return 0;
}
