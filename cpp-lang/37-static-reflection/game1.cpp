#include <iostream>
#include <string>
#include <random>
#include <algorithm>

// C++20 Concepts 정의

template <typename T>
concept Damageable = requires(T obj, int damage) {
    { obj.take_damage(damage) } -> std::convertible_to<void>;
    { obj.is_alive() } -> std::convertible_to<bool>;
};

template <typename T>
concept Attackable = requires(T obj) {
    { obj.attack() } -> std::convertible_to<int>;
};

template <typename T>
concept Fighter = Damageable<T> && Attackable<T>;

// Player 클래스
class Player {
private:
    std::string name;
    int hp;
    int max_hp;
    int level;
    int exp;
    std::mt19937 gen{std::random_device{}()};

public:
    Player(const std::string& n = "Hero", int h = 100)
        : name(n), hp(h), max_hp(h), level(1), exp(0) {}

    void take_damage(int damage) {
        int actual_damage = std::max(1, damage - level);
        hp = std::max(0, hp - actual_damage);
        std::cout << "[" << name << "] " << actual_damage << "의 피해를 입었습니다!" << std::endl;
        std::cout << "  현재 HP: " << hp << "/" << max_hp << std::endl;
    }

    bool is_alive() const {
        return hp > 0;
    }

    int attack() const {
        std::uniform_int_distribution<> dis(5, 15);
        int damage = dis(const_cast<std::mt19937&>(gen)) + level * 2;
        std::cout << "[" << name << "]의 공격! (" << damage << " 데미지)" << std::endl;
        return damage;
    }

    void heal(int amount) {
        int old_hp = hp;
        hp = std::min(max_hp, hp + amount);
        int healed = hp - old_hp;
        std::cout << "[" << name << "] " << healed << "만큼 회복되었습니다!" << std::endl;
        std::cout << "  현재 HP: " << hp << "/" << max_hp << std::endl;
    }

    void gain_exp(int amount) {
        exp += amount;
        std::cout << "[" << name << "] " << amount << " 경험치를 획득했습니다!" << std::endl;

        if (exp >= level * 100) {
            level++;
            max_hp += 20;
            hp = max_hp;
            exp = 0;
            std::cout << "*** [" << name << "] 레벨 " << level << "로 상승! HP 증가! ***" << std::endl;
        }
    }

    void display_status() const {
        std::cout << "\n=== [" << name << "] 상태 ===" << std::endl;
        std::cout << "  레벨: " << level << std::endl;
        std::cout << "  HP: " << hp << "/" << max_hp << std::endl;
        std::cout << "  경험치: " << exp << "/" << (level * 100) << std::endl;
    }

    const std::string& get_name() const { return name; }
};

// Monster 클래스
class Monster {
private:
    std::string name;
    int hp;
    int max_hp;
    int level;
    std::mt19937 gen{std::random_device{}()};

public:
    Monster(const std::string& n = "Goblin", int h = 30, int lv = 1)
        : name(n), hp(h), max_hp(h), level(lv) {}

    void take_damage(int damage) {
        hp = std::max(0, hp - damage);
        std::cout << "[" << name << "] " << damage << "의 피해를 입었습니다!" << std::endl;
        std::cout << "  현재 HP: " << hp << "/" << max_hp << std::endl;
    }

    bool is_alive() const {
        return hp > 0;
    }

    int attack() const {
        std::uniform_int_distribution<> dis(3, 10);
        int damage = dis(const_cast<std::mt19937&>(gen)) + level;
        std::cout << "[" << name << "]의 공격! (" << damage << " 데미지)" << std::endl;
        return damage;
    }

    const std::string& get_name() const { return name; }
    int get_exp_reward() const { return level * 20; }
};

// Concept 기반 함수

template <Attackable A, Damageable D>
void battle(A& attacker, D& defender) {
    int damage = attacker.attack();
    defender.take_damage(damage);
}

template <Damageable D>
void heal(D& target, int amount) {
    if constexpr (requires { target.heal(amount); }) {
        target.heal(amount);
    }
}

int main() {
    std::cout << "=== 던전 레이드 게임 (Concepts 기반) ===" << std::endl << std::endl;

    // 1. 캐릭터 생성
    std::cout << "[1단계] 캐릭터 생성" << std::endl;
    Player player("용사", 100);
    player.display_status();
    std::cout << std::endl;

    // 2. 약한 몬스터와 전투
    std::cout << "[2단계] 약한 몬스터와 전투" << std::endl;
    Monster goblin("고블린", 25, 1);
    while (player.is_alive() && goblin.is_alive()) {
        battle(player, goblin);
        if (!goblin.is_alive()) {
            std::cout << "*** " << goblin.get_name() << "를 격파했습니다! ***" << std::endl;
            player.gain_exp(goblin.get_exp_reward());
            break;
        }
        battle(goblin, player);
    }
    std::cout << std::endl;

    // 3. 회복
    std::cout << "[3단계] 회복" << std::endl;
    heal(player, 50);
    player.display_status();
    std::cout << std::endl;

    // 4. 강한 몬스터와 전투
    std::cout << "[4단계] 중간 몬스터와 전투" << std::endl;
    Monster orc("오크", 40, 2);
    while (player.is_alive() && orc.is_alive()) {
        battle(player, orc);
        if (!orc.is_alive()) {
            std::cout << "*** " << orc.get_name() << "를 격파했습니다! ***" << std::endl;
            player.gain_exp(orc.get_exp_reward());
            break;
        }
        battle(orc, player);
        if (!player.is_alive()) {
            std::cout << "*** 플레이어가 쓰러졌습니다! ***" << std::endl;
            break;
        }
    }
    std::cout << std::endl;

    // 5. 다시 회복
    if (player.is_alive()) {
        std::cout << "[5단계] 회복" << std::endl;
        heal(player, 50);
        std::cout << std::endl;
    }

    // 6. 보스전
    if (player.is_alive()) {
        std::cout << "[6단계] 보스와 전투!" << std::endl;
        Monster boss("검은 기사", 80, 3);
        while (player.is_alive() && boss.is_alive()) {
            battle(player, boss);
            if (!boss.is_alive()) {
                std::cout << "*** " << boss.get_name() << "를 격파했습니다! ***" << std::endl;
                std::cout << "*** 던전 레이드 성공! ***" << std::endl;
                player.gain_exp(boss.get_exp_reward());
                break;
            }
            battle(boss, player);
            if (!player.is_alive()) {
                std::cout << "*** 플레이어가 쓰러졌습니다! ***" << std::endl;
                break;
            }
        }
    }

    // 7. 최종 상태
    std::cout << std::endl;
    player.display_status();
    std::cout << "\n[게임 종료]" << std::endl;

    return 0;
}
