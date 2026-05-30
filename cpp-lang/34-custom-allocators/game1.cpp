#include <iostream>
#include <memory_resource>
#include <vector>
#include <string>
#include <algorithm>
#include <random>

struct Item {
    std::pmr::string name;
    int health;
    int value;

    Item(const std::string& n, int h, int v, std::pmr::polymorphic_allocator<char> alloc)
        : name(n, alloc), health(h), value(v) {}

    void display() const {
        std::cout << "  [" << name << "] 체력: " << health << ", 가치: " << value << "\n";
    }
};

int main() {
    std::cout << "=== 메모리 풀 관리 게임 ===\n";
    std::cout << "제한된 메모리 내에서 몬스터를 생성하세요!\n";
    std::cout << "메모리 풀 크기: 512 바이트\n\n";

    char buffer[512];
    std::pmr::monotonic_buffer_resource pool(buffer, sizeof(buffer));
    std::pmr::polymorphic_allocator<Item> item_alloc(&pool);
    std::pmr::polymorphic_allocator<char> char_alloc(&pool);

    std::pmr::vector<Item> items(item_alloc);
    int score = 0;
    int turn = 0;

    std::mt19937 rng(42);
    std::uniform_int_distribution<> health_dist(20, 100);
    std::uniform_int_distribution<> value_dist(10, 50);

    const char* monster_names[] = {
        "슬라임", "고블린", "늑대", "오우거", "용",
        "골렘", "뱀파이어", "좀비", "유령", "악마"
    };

    while (true) {
        ++turn;
        std::cout << "=== 턴 " << turn << " ===\n";
        std::cout << "현재 메모리 사용: " << items.size() * sizeof(Item) + 100
                  << " / 512 바이트\n";
        std::cout << "현재 점수: " << score << "\n\n";

        std::cout << "명령을 선택하세요:\n";
        std::cout << "1. 몬스터 생성\n";
        std::cout << "2. 가장 약한 몬스터 제거\n";
        std::cout << "3. 모든 몬스터 보기\n";
        std::cout << "4. 게임 종료\n";
        std::cout << "> ";

        int cmd;
        if (!(std::cin >> cmd)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (cmd == 1) {
            try {
                int idx = static_cast<int>(rng() % 10);
                int h = health_dist(rng);
                int v = value_dist(rng);

                items.emplace_back(monster_names[idx], h, v, char_alloc);
                score += v;
                std::cout << "✓ '" << monster_names[idx] << "' 생성!\n\n";
            } catch (const std::bad_alloc&) {
                std::cout << "✗ 메모리 부족! 몬스터를 제거해야 합니다.\n\n";
            }
        } else if (cmd == 2) {
            if (items.empty()) {
                std::cout << "✗ 제거할 몬스터가 없습니다.\n\n";
            } else {
                auto it = std::min_element(items.begin(), items.end(),
                    [](const Item& a, const Item& b) { return a.health < b.health; });
                std::cout << "✓ '" << it->name << "' 제거!\n";
                items.erase(it);
                std::cout << "  (메모리 확보됨)\n\n";
            }
        } else if (cmd == 3) {
            if (items.empty()) {
                std::cout << "몬스터가 없습니다.\n\n";
            } else {
                std::cout << "현재 몬스터 목록:\n";
                for (const auto& item : items) {
                    item.display();
                }
                std::cout << "\n";
            }
        } else if (cmd == 4) {
            std::cout << "\n=== 게임 종료 ===\n";
            std::cout << "최종 점수: " << score << "\n";
            std::cout << "생성한 몬스터: " << items.size() << "개\n";
            std::cout << "평균 체력: ";
            if (items.empty()) {
                std::cout << "0\n";
            } else {
                int total_health = 0;
                for (const auto& item : items) {
                    total_health += item.health;
                }
                std::cout << (total_health / static_cast<double>(items.size())) << "\n";
            }
            break;
        } else {
            std::cout << "✗ 잘못된 명령\n\n";
        }
    }

    return 0;
}
