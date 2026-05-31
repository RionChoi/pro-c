#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>

struct Item {
    std::string name;
    int quantity;
    double price;
};

struct Inventory {
    std::string owner;
    int level;
    double gold;
    std::vector<Item> items;
};

bool saveInventory(const Inventory& inv) {
    std::ofstream file("inventory.dat", std::ios::binary);
    if (!file.is_open()) return false;

    uint32_t owner_len = inv.owner.length();
    file.write(reinterpret_cast<const char*>(&owner_len), sizeof(owner_len));
    file.write(inv.owner.c_str(), owner_len);
    file.write(reinterpret_cast<const char*>(&inv.level), sizeof(inv.level));
    file.write(reinterpret_cast<const char*>(&inv.gold), sizeof(inv.gold));

    uint32_t item_count = inv.items.size();
    file.write(reinterpret_cast<const char*>(&item_count), sizeof(item_count));

    for (const auto& item : inv.items) {
        uint32_t name_len = item.name.length();
        file.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
        file.write(item.name.c_str(), name_len);
        file.write(reinterpret_cast<const char*>(&item.quantity), sizeof(item.quantity));
        file.write(reinterpret_cast<const char*>(&item.price), sizeof(item.price));
    }

    file.close();
    return true;
}

bool loadInventory(Inventory& inv) {
    std::ifstream file("inventory.dat", std::ios::binary);
    if (!file.is_open()) return false;

    uint32_t owner_len;
    file.read(reinterpret_cast<char*>(&owner_len), sizeof(owner_len));
    char owner_buffer[256];
    file.read(owner_buffer, owner_len);
    owner_buffer[owner_len] = '\0';
    inv.owner = owner_buffer;

    file.read(reinterpret_cast<char*>(&inv.level), sizeof(inv.level));
    file.read(reinterpret_cast<char*>(&inv.gold), sizeof(inv.gold));

    uint32_t item_count;
    file.read(reinterpret_cast<char*>(&item_count), sizeof(item_count));
    inv.items.clear();

    for (uint32_t i = 0; i < item_count; ++i) {
        uint32_t name_len;
        file.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));
        char name_buffer[256];
        file.read(name_buffer, name_len);
        name_buffer[name_len] = '\0';

        int quantity;
        double price;
        file.read(reinterpret_cast<char*>(&quantity), sizeof(quantity));
        file.read(reinterpret_cast<char*>(&price), sizeof(price));

        inv.items.push_back({std::string(name_buffer), quantity, price});
    }

    file.close();
    return true;
}

void printInventory(const Inventory& inv) {
    std::cout << "\n=== 인벤토리 ===\n";
    std::cout << "플레이어: " << inv.owner << "\n";
    std::cout << "레벨: " << inv.level << "\n";
    std::cout << "골드: " << inv.gold << "\n";
    std::cout << "아이템 (" << inv.items.size() << "개):\n";
    for (const auto& item : inv.items) {
        std::cout << "  - " << item.name << " x" << item.quantity
                  << " (단가: " << item.price << ")\n";
    }
    std::cout << "================\n";
}

int main() {
    std::cout << "=== RPG 인벤토리 저장/로드 게임 ===\n\n";

    Inventory player{"Hero", 10, 5000.0};
    player.items.push_back({"철검", 1, 500.0});
    player.items.push_back({"포션", 5, 100.0});
    player.items.push_back({"방어 부츠", 1, 800.0});

    std::cout << "플레이어 데이터 생성:\n";
    printInventory(player);

    std::cout << "인벤토리 저장 중...\n";
    if (saveInventory(player)) {
        std::cout << "저장 완료!\n\n";
    } else {
        std::cerr << "저장 실패!\n";
        return 1;
    }

    std::cout << "게임 재시작 시뮬레이션...\n";
    std::cout << "인벤토리 로드 중...\n";

    Inventory loaded;
    if (loadInventory(loaded)) {
        std::cout << "로드 완료!\n";
        printInventory(loaded);

        std::cout << "검증:\n";
        bool valid = (player.owner == loaded.owner &&
                      player.level == loaded.level &&
                      player.gold == loaded.gold &&
                      player.items.size() == loaded.items.size());

        if (valid) {
            for (size_t i = 0; i < player.items.size(); ++i) {
                if (player.items[i].name != loaded.items[i].name ||
                    player.items[i].quantity != loaded.items[i].quantity ||
                    player.items[i].price != loaded.items[i].price) {
                    valid = false;
                    break;
                }
            }
        }

        std::cout << "  데이터 무결성: " << (valid ? "✓ 일치" : "✗ 불일치") << "\n";

        if (valid) {
            loaded.gold += 1000.0;
            loaded.items[1].quantity += 3;
            std::cout << "\n인벤토리 업데이트 (골드 +1000, 포션 +3):\n";
            printInventory(loaded);

            if (saveInventory(loaded)) {
                std::cout << "업데이트 저장 완료!\n";
            }
        }
    } else {
        std::cerr << "로드 실패!\n";
        return 1;
    }

    return 0;
}
