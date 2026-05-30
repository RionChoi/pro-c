#include <iostream>
#include <vector>
#include <memory_resource>
#include <cstdlib>
#include <ctime>

class GameObject {
private:
    std::string name;
    int health;
    int damage;

public:
    GameObject(const std::string& n, int h, int d)
        : name(n), health(h), damage(d) {}

    void display() const {
        std::cout << "이름: " << name << " | HP: " << health
                  << " | 데미지: " << damage << "\n";
    }

    int get_health() const { return health; }
    int get_damage() const { return damage; }
    void take_damage(int dmg) { health -= dmg; }
};

int main() {
    std::cout << "=== 메모리 할당자 게임: 몬스터 배틀 ===\n\n";

    constexpr std::size_t POOL_SIZE = 8192;
    std::byte pool[POOL_SIZE];

    std::pmr::monotonic_buffer_resource mbr(pool, POOL_SIZE);
    std::pmr::polymorphic_allocator<GameObject> alloc(&mbr);

    std::pmr::vector<GameObject> enemies(alloc);

    std::cout << "초기 메모리 풀 크기: " << POOL_SIZE << " 바이트\n\n";

    std::cout << "3개의 몬스터 생성\n";
    enemies.emplace_back("고블린", 30, 5);
    enemies.emplace_back("오크", 50, 8);
    enemies.emplace_back("드래곤", 100, 15);

    std::cout << "\n몬스터 목록:\n";
    for (std::size_t i = 0; i < enemies.size(); i++) {
        std::cout << i + 1 << ". ";
        enemies[i].display();
    }

    int player_hp = 100;
    std::cout << "\n플레이어 HP: " << player_hp << "\n\n";

    srand(static_cast<unsigned>(time(nullptr)));

    int round = 1;
    while (round <= 3) {
        std::cout << "=== 라운드 " << round << " ===\n";

        int enemy_idx = rand() % enemies.size();
        int player_dmg = 10 + rand() % 5;

        std::cout << "플레이어의 공격! " << player_dmg << " 데미지\n";
        enemies[enemy_idx].take_damage(player_dmg);
        std::cout << "몬스터 " << enemy_idx + 1 << " HP: " << enemies[enemy_idx].get_health() << "\n";

        if (enemies[enemy_idx].get_health() > 0) {
            int enemy_dmg = enemies[enemy_idx].get_damage();
            std::cout << "몬스터의 반격! " << enemy_dmg << " 데미지\n";
            player_hp -= enemy_dmg;
            std::cout << "플레이어 HP: " << player_hp << "\n\n";

            if (player_hp <= 0) {
                std::cout << "플레이어 패배!\n";
                return 0;
            }
        } else {
            std::cout << "몬스터 " << enemy_idx + 1 << " 격파!\n\n";
        }

        round++;
    }

    std::cout << "\n=== 게임 끝 ===\n";
    std::cout << "플레이어 최종 HP: " << player_hp << "\n";
    std::cout << "승리!\n";

    std::cout << "\n(메모리 풀이 자동으로 해제됩니다)\n";

    return 0;
}
