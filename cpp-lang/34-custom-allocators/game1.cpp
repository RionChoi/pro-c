#include <iostream>
#include <vector>
#include <memory_resource>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cstddef>
#include <array>

// 게임 아이템 구조체
struct Item {
    std::string name;
    int rarity;  // 1: 일반, 2: 희귀, 3: 전설

    Item() : rarity(0) {}
    Item(const std::string& n, int r) : name(n), rarity(r) {}
};

// 아이템 풀 관리자
class ItemPool {
    std::pmr::vector<Item> pool;
    std::pmr::memory_resource* resource;

public:
    ItemPool(std::pmr::memory_resource* res = std::pmr::new_delete_resource())
        : pool(res), resource(res) {
        // 미리 할당된 풀로 초기화
        for (int i = 0; i < 50; ++i) {
            pool.emplace_back("", 0);
        }
    }

    Item* allocate(const std::string& name, int rarity) {
        if (!pool.empty()) {
            Item& item = pool.back();
            item.name = name;
            item.rarity = rarity;
            pool.pop_back();
            return &item;
        }
        return nullptr;
    }

    void deallocate(Item* item) {
        if (item != nullptr) {
            pool.emplace_back("", 0);
        }
    }

    void print_stats() const {
        std::cout << "  풀 사용 가능: " << pool.size() << "/50 아이템\n";
    }
};

int main() {
    std::cout << "=== 메모리 풀 기반 게임 ===\n";
    std::cout << "숫자를 입력하여 인벤토리 관리 (1-3: 아이템 획득, 4: 초기화, 0: 종료)\n\n";

    std::array<std::byte, 4096> buffer;
    std::pmr::monotonic_buffer_resource buffer_res(buffer.data(), buffer.size());

    std::pmr::vector<Item> inventory(&buffer_res);
    ItemPool pool(&buffer_res);

    srand(static_cast<unsigned>(time(nullptr)));

    while (true) {
        pool.print_stats();
        std::cout << "\n현재 인벤토리 (" << inventory.size() << "개):\n";

        for (size_t i = 0; i < inventory.size(); ++i) {
            const Item& item = inventory[i];
            std::string rarity_name;
            switch (item.rarity) {
                case 1: rarity_name = "일반"; break;
                case 2: rarity_name = "희귀"; break;
                case 3: rarity_name = "전설"; break;
                default: rarity_name = "불명"; break;
            }
            std::cout << "  [" << i << "] " << item.name << " (" << rarity_name << ")\n";
        }

        std::cout << "\n명령 입력 (1-3: 아이템 획득, 4: 초기화, 0: 종료): ";
        int cmd;
        if (std::cin.peek() == '\n') {
            std::cin.get();
            continue;
        }
        if (!(std::cin >> cmd)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }
        std::cin.ignore(10000, '\n');

        if (cmd == 0) {
            std::cout << "게임 종료!\n";
            break;
        } else if (cmd >= 1 && cmd <= 3) {
            if (inventory.size() >= 20) {
                std::cout << "인벤토리가 가득 찼습니다!\n\n";
                continue;
            }

            const char* names[] = {"검", "방패", "지팡이", "반지", "책", "보옥"};
            std::string name = names[rand() % 6];
            name += std::to_string(rand() % 1000);

            inventory.emplace_back(name, cmd);
            std::cout << "획득! " << name << " (희귀도: " << cmd << ")\n\n";
        } else if (cmd == 4) {
            if (!inventory.empty()) {
                std::cout << "인벤토리를 초기화했습니다!\n";
                inventory.clear();
            } else {
                std::cout << "이미 비어있습니다.\n";
            }
            std::cout << "\n";
        } else {
            std::cout << "잘못된 명령입니다.\n\n";
        }
    }

    return 0;
}
