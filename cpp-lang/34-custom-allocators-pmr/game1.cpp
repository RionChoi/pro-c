#include <iostream>
#include <memory_resource>
#include <vector>
#include <string>
#include <array>
#include <cstddef>
#include <random>
#include <algorithm>

class GameItem {
public:
    std::pmr::string name;
    int power;

    GameItem(const char* n, int p, std::pmr::memory_resource* mr)
        : name(n, mr), power(p) {}

    void display() const {
        std::cout << "  [" << name << "] 공격력=" << power << "\n";
    }
};

int main() {
    std::cout << "=== 메모리 제한 게임: 보물 수집 ===\n\n";

    // 고정 크기 버퍼 메모리 (제한된 환경)
    std::array<std::byte, 2048> buffer;
    std::pmr::monotonic_buffer_resource game_memory(buffer.data(), buffer.size());

    std::pmr::vector<GameItem> inventory(&game_memory);

    std::mt19937 gen(42);
    std::uniform_int_distribution<> power_dist(10, 100);

    std::cout << "1. 보물 수집 (메모리 제한 환경):\n";
    const char* treasures[] = {"검", "활", "방패", "투구", "부츠"};

    for (int i = 0; i < 5; ++i) {
        try {
            inventory.emplace_back(treasures[i], power_dist(gen), &game_memory);
            std::cout << "   보물 획득: " << treasures[i] << "\n";
        } catch (...) {
            std::cout << "   메모리 부족! 더 이상 보물을 수집할 수 없습니다.\n";
            break;
        }
    }

    std::cout << "\n2. 수집된 보물 목록:\n";
    int total_power = 0;
    for (const auto& item : inventory) {
        item.display();
        total_power += item.power;
    }

    std::cout << "\n3. 게임 통계:\n";
    std::cout << "   수집한 보물 수: " << inventory.size() << "\n";
    std::cout << "   총 공격력: " << total_power << "\n";
    std::cout << "   평균 공격력: " << total_power / (int)inventory.size() << "\n";

    std::cout << "\n4. 보물 정렬 (공격력 높은 순):\n";
    std::sort(inventory.begin(), inventory.end(),
        [](const GameItem& a, const GameItem& b) {
            return a.power > b.power;
        });

    for (const auto& item : inventory) {
        item.display();
    }

    std::cout << "\n게임 완료!\n";
    return 0;
}
