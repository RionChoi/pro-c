#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

struct GameState {
    char playerName[64];
    int level;
    int experience;
    std::vector<std::string> inventory;
};

const uint32_t GAME_VERSION = 1;

void saveGame(const std::string& filename, const GameState& state) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "저장 실패\n";
        return;
    }

    uint32_t version = GAME_VERSION;
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));

    file.write(state.playerName, sizeof(state.playerName));
    file.write(reinterpret_cast<const char*>(&state.level), sizeof(state.level));
    file.write(reinterpret_cast<const char*>(&state.experience), sizeof(state.experience));

    size_t itemCount = state.inventory.size();
    file.write(reinterpret_cast<const char*>(&itemCount), sizeof(itemCount));

    for (const auto& item : state.inventory) {
        size_t itemLen = item.length();
        file.write(reinterpret_cast<const char*>(&itemLen), sizeof(itemLen));
        file.write(item.c_str(), itemLen);
    }

    file.close();
}

bool loadGame(const std::string& filename, GameState& state) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "파일 없음\n";
        return false;
    }

    uint32_t version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (version != GAME_VERSION) {
        std::cerr << "버전 불일치\n";
        return false;
    }

    file.read(state.playerName, sizeof(state.playerName));
    file.read(reinterpret_cast<char*>(&state.level), sizeof(state.level));
    file.read(reinterpret_cast<char*>(&state.experience), sizeof(state.experience));

    size_t itemCount;
    file.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));

    state.inventory.clear();
    for (size_t i = 0; i < itemCount; ++i) {
        size_t itemLen;
        file.read(reinterpret_cast<char*>(&itemLen), sizeof(itemLen));

        std::string item(itemLen, '\0');
        file.read(&item[0], itemLen);
        state.inventory.push_back(item);
    }

    file.close();
    return true;
}

void displayStatus(const GameState& state) {
    std::cout << "\n=== 플레이어 상태 ===\n";
    std::cout << "이름: " << state.playerName << "\n";
    std::cout << "레벨: " << state.level << "\n";
    std::cout << "경험치: " << state.experience << "\n";
    std::cout << "아이템 (" << state.inventory.size() << "개): ";
    for (const auto& item : state.inventory) {
        std::cout << item << " ";
    }
    std::cout << "\n\n";
}

int main() {
    GameState game{"Player", 1, 0, {}};
    std::string command;

    std::cout << "=== 간단한 RPG 게임 ===\n";
    std::cout << "명령어: fight, find, status, save, load, quit\n\n";

    while (true) {
        std::cout << "[Lv" << game.level << "] > ";
        std::cin >> command;

        if (command == "fight") {
            game.experience += 50;
            std::cout << "전투! +50 경험치\n";
            if (game.experience >= 100) {
                game.level++;
                game.experience = 0;
                std::cout << "레벨 업! -> Lv" << game.level << "\n";
            }
        } else if (command == "find") {
            static int itemCount = 0;
            game.inventory.push_back("Item" + std::to_string(++itemCount));
            std::cout << "아이템 발견! -> " << game.inventory.back() << "\n";
        } else if (command == "status") {
            displayStatus(game);
        } else if (command == "save") {
            saveGame("gamesave.dat", game);
            std::cout << "저장 완료\n";
        } else if (command == "load") {
            if (loadGame("gamesave.dat", game)) {
                std::cout << "불러오기 완료\n";
                displayStatus(game);
            }
        } else if (command == "quit") {
            std::cout << "게임 종료\n";
            break;
        } else {
            std::cout << "알 수 없는 명령어\n";
        }
    }

    return 0;
}
