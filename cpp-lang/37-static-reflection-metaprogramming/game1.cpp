#include <iostream>
#include <tuple>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>

template <typename T, typename U>
struct Member {
    std::string name;
    U T::*ptr;

    Member(const std::string& n, U T::*p) : name(n), ptr(p) {}
};

template <typename T>
struct ClassInfo;

struct Player {
    std::string name;
    int hp;
    int level;
};

template <>
struct ClassInfo<Player> {
    using Type = Player;
    using Members = std::tuple<
        Member<Player, std::string>,
        Member<Player, int>,
        Member<Player, int>
    >;

    static Members get_members() {
        return std::make_tuple(
            Member<Player, std::string>("name", &Player::name),
            Member<Player, int>("hp", &Player::hp),
            Member<Player, int>("level", &Player::level)
        );
    }
};

struct Monster {
    std::string name;
    int attack;
    int defense;
};

template <>
struct ClassInfo<Monster> {
    using Type = Monster;
    using Members = std::tuple<
        Member<Monster, std::string>,
        Member<Monster, int>,
        Member<Monster, int>
    >;

    static Members get_members() {
        return std::make_tuple(
            Member<Monster, std::string>("name", &Monster::name),
            Member<Monster, int>("attack", &Monster::attack),
            Member<Monster, int>("defense", &Monster::defense)
        );
    }
};

template <typename T>
T clone_object(const T& obj) {
    T copy = obj;
    return copy;
}

template <typename T>
bool objects_equal(const T& a, const T& b) {
    if (a.name != b.name) return false;
    if (a.hp != b.hp && a.attack != b.attack) return false;
    if (a.level != b.level && a.defense != b.defense) return false;
    return true;
}

class EntityPool {
public:
    EntityPool() : players_active(0), monsters_active(0) {}

    void add_player(const Player& p) {
        if (players_active < players.size()) {
            players[players_active] = p;
        } else {
            players.push_back(p);
        }
        players_active++;
    }

    void add_monster(const Monster& m) {
        if (monsters_active < monsters.size()) {
            monsters[monsters_active] = m;
        } else {
            monsters.push_back(m);
        }
        monsters_active++;
    }

    Player& get_player(size_t idx) {
        return players[idx];
    }

    Monster& get_monster(size_t idx) {
        return monsters[idx];
    }

    size_t player_count() const { return players_active; }
    size_t monster_count() const { return monsters_active; }

    void reset() {
        players_active = 0;
        monsters_active = 0;
    }

    Player save_player_state(size_t idx) {
        return clone_object(players[idx]);
    }

    Monster save_monster_state(size_t idx) {
        return clone_object(monsters[idx]);
    }

private:
    std::vector<Player> players;
    std::vector<Monster> monsters;
    size_t players_active;
    size_t monsters_active;
};

void print_player(const Player& p) {
    std::cout << "플레이어: " << p.name << " | HP: " << p.hp << " | 레벨: " << p.level;
}

void print_monster(const Monster& m) {
    std::cout << "몬스터: " << m.name << " | 공격력: " << m.attack << " | 방어력: " << m.defense;
}

void battle_simulation(EntityPool& pool) {
    std::cout << "\n=== 전투 시뮬레이션 ===\n\n";

    if (pool.player_count() == 0 || pool.monster_count() == 0) {
        std::cout << "전투를 위한 플레이어 또는 몬스터가 부족합니다.\n";
        return;
    }

    Player player = pool.save_player_state(0);
    Monster monster = pool.save_monster_state(0);

    std::cout << "전투 시작!\n";
    print_player(player);
    std::cout << "\n";
    print_monster(monster);
    std::cout << "\n\n";

    int rounds = 0;
    while (player.hp > 0 && monster.attack > 0 && rounds < 10) {
        rounds++;
        std::cout << "라운드 " << rounds << ":\n";

        int damage_to_monster = (player.level * 10 + 5) - monster.defense;
        if (damage_to_monster < 1) damage_to_monster = 1;
        monster.attack -= damage_to_monster;

        std::cout << "  플레이어가 " << damage_to_monster << " 데미지를 입혔습니다.\n";

        if (monster.attack <= 0) {
            monster.attack = 0;
            break;
        }

        int damage_to_player = monster.attack - (player.level * 2);
        if (damage_to_player < 1) damage_to_player = 1;
        player.hp -= damage_to_player;

        std::cout << "  몬스터가 " << damage_to_player << " 데미지를 입혔습니다.\n";
        std::cout << "  현재 플레이어 HP: " << player.hp << "\n\n";

        if (player.hp <= 0) {
            player.hp = 0;
            break;
        }
    }

    std::cout << "\n전투 결과:\n";
    print_player(player);
    std::cout << "\n";
    print_monster(monster);
    std::cout << "\n\n";

    if (player.hp > 0 && monster.attack <= 0) {
        std::cout << "✓ 플레이어 승리!\n";
        player.level++;
        pool.get_player(0) = player;
    } else if (player.hp <= 0) {
        std::cout << "✗ 플레이어 패배...\n";
    } else {
        std::cout << "△ 무승부!\n";
    }
}

void state_save_restore(EntityPool& pool) {
    std::cout << "\n=== 상태 저장 및 복원 ===\n\n";

    if (pool.player_count() == 0) {
        std::cout << "저장할 플레이어가 없습니다.\n";
        return;
    }

    Player original = pool.save_player_state(0);
    std::cout << "저장된 상태:\n";
    print_player(original);
    std::cout << "\n\n";

    Player& current = pool.get_player(0);
    current.hp = 10;
    current.level = 5;

    std::cout << "현재 상태 (변경됨):\n";
    print_player(current);
    std::cout << "\n\n";

    pool.get_player(0) = original;

    std::cout << "복원된 상태:\n";
    print_player(pool.get_player(0));
    std::cout << "\n";
}

int main() {
    EntityPool pool;

    std::cout << "=== 리플렉션 기반 게임 엔티티 관리 ===\n\n";

    std::cout << "플레이어 추가:\n";
    Player p1{"용사", 100, 1};
    Player p2{"마법사", 50, 3};
    pool.add_player(p1);
    pool.add_player(p2);

    print_player(pool.get_player(0));
    std::cout << "\n";
    print_player(pool.get_player(1));
    std::cout << "\n\n";

    std::cout << "몬스터 추가:\n";
    Monster m1{"고블린", 15, 5};
    Monster m2{"오크", 25, 10};
    pool.add_monster(m1);
    pool.add_monster(m2);

    print_monster(pool.get_monster(0));
    std::cout << "\n";
    print_monster(pool.get_monster(1));
    std::cout << "\n\n";

    std::cout << "현재 풀 상태: 플레이어 " << pool.player_count() << "명, "
              << "몬스터 " << pool.monster_count() << "마리\n";

    battle_simulation(pool);
    state_save_restore(pool);

    return 0;
}
