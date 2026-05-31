#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <ctime>

const uint32_t MAGIC_NUMBER = 0x47414D45; // "GAME"
const uint16_t VERSION = 1;

struct Item {
    uint16_t id;
    char name[32];
    uint16_t quantity;
};

struct GameState {
    char player_name[64];
    uint16_t level;
    uint32_t experience;
    uint16_t health;
    uint16_t max_health;
    std::vector<Item> inventory;
};

bool save_game(const std::string& filename, const GameState& state) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "게임 저장 실패" << std::endl;
        return false;
    }

    file.write(reinterpret_cast<const char*>(&MAGIC_NUMBER), sizeof(MAGIC_NUMBER));
    file.write(reinterpret_cast<const char*>(&VERSION), sizeof(VERSION));

    file.write(state.player_name, sizeof(state.player_name));
    file.write(reinterpret_cast<const char*>(&state.level), sizeof(state.level));
    file.write(reinterpret_cast<const char*>(&state.experience), sizeof(state.experience));
    file.write(reinterpret_cast<const char*>(&state.health), sizeof(state.health));
    file.write(reinterpret_cast<const char*>(&state.max_health), sizeof(state.max_health));

    uint32_t item_count = static_cast<uint32_t>(state.inventory.size());
    file.write(reinterpret_cast<const char*>(&item_count), sizeof(item_count));

    for (const auto& item : state.inventory) {
        file.write(reinterpret_cast<const char*>(&item.id), sizeof(item.id));
        file.write(item.name, sizeof(item.name));
        file.write(reinterpret_cast<const char*>(&item.quantity), sizeof(item.quantity));
    }

    file.close();
    return true;
}

bool load_game(const std::string& filename, GameState& state) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "게임 로드 실패" << std::endl;
        return false;
    }

    uint32_t magic;
    uint16_t version;

    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    if (magic != MAGIC_NUMBER) {
        std::cerr << "손상된 게임 파일" << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (version != VERSION) {
        std::cerr << "호환되지 않는 게임 버전" << std::endl;
        return false;
    }

    file.read(state.player_name, sizeof(state.player_name));
    file.read(reinterpret_cast<char*>(&state.level), sizeof(state.level));
    file.read(reinterpret_cast<char*>(&state.experience), sizeof(state.experience));
    file.read(reinterpret_cast<char*>(&state.health), sizeof(state.health));
    file.read(reinterpret_cast<char*>(&state.max_health), sizeof(state.max_health));

    uint32_t item_count;
    file.read(reinterpret_cast<char*>(&item_count), sizeof(item_count));

    state.inventory.clear();
    for (uint32_t i = 0; i < item_count; ++i) {
        Item item;
        file.read(reinterpret_cast<char*>(&item.id), sizeof(item.id));
        file.read(item.name, sizeof(item.name));
        file.read(reinterpret_cast<char*>(&item.quantity), sizeof(item.quantity));
        state.inventory.push_back(item);
    }

    file.close();
    return true;
}

void display_state(const GameState& state) {
    std::cout << "\n==== 게임 상태 ====" << std::endl;
    std::cout << "플레이어: " << state.player_name << std::endl;
    std::cout << "레벨: " << state.level << std::endl;
    std::cout << "경험치: " << state.experience << std::endl;
    std::cout << "체력: " << state.health << "/" << state.max_health << std::endl;
    std::cout << "인벤토리 (" << state.inventory.size() << " 개):" << std::endl;
    for (const auto& item : state.inventory) {
        std::cout << "  [" << item.id << "] " << item.name << " x" << item.quantity << std::endl;
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    GameState game_state;
    std::strcpy(game_state.player_name, "용사");
    game_state.level = 1;
    game_state.experience = 0;
    game_state.health = 100;
    game_state.max_health = 100;

    std::cout << "=== RPG 게임 (저장/로드 시스템) ===" << std::endl;

    while (true) {
        std::cout << "\n1. 게임 진행" << std::endl;
        std::cout << "2. 상태 확인" << std::endl;
        std::cout << "3. 게임 저장" << std::endl;
        std::cout << "4. 게임 로드" << std::endl;
        std::cout << "5. 종료" << std::endl;
        std::cout << "선택: ";

        int choice;
        if (std::cin >> choice) {
            switch (choice) {
                case 1: {
                    int damage = rand() % 20 + 5;
                    game_state.health = (game_state.health > damage) ? game_state.health - damage : 0;
                    int reward = rand() % 50 + 10;
                    game_state.experience += reward;
                    game_state.level = 1 + game_state.experience / 100;

                    std::cout << "몬스터와 전투! 피해: " << damage << ", 경험치: " << reward << std::endl;

                    if (game_state.health == 0) {
                        std::cout << "게임 오버..." << std::endl;
                        return 0;
                    }
                    break;
                }
                case 2: {
                    display_state(game_state);
                    break;
                }
                case 3: {
                    game_state.inventory.clear();
                    game_state.inventory.push_back({1, "검", 1});
                    game_state.inventory.push_back({2, "포션", 5});
                    game_state.inventory.push_back({3, "금화", 100});

                    if (save_game("savegame.bin", game_state)) {
                        std::cout << "게임을 저장했습니다." << std::endl;
                    }
                    break;
                }
                case 4: {
                    if (load_game("savegame.bin", game_state)) {
                        std::cout << "게임을 로드했습니다." << std::endl;
                        display_state(game_state);
                    }
                    break;
                }
                case 5: {
                    std::cout << "게임을 종료합니다." << std::endl;
                    return 0;
                }
                default:
                    std::cout << "잘못된 선택입니다." << std::endl;
            }
        }
    }

    return 0;
}
