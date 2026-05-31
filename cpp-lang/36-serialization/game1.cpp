#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <ctime>

struct GameState {
    int level;
    int experience;
    int health;
    int maxHealth;
    char playerName[32];
    int coinsCollected;

    void serialize(std::ofstream& out) const {
        out.write(reinterpret_cast<const char*>(&level), sizeof(level));
        out.write(reinterpret_cast<const char*>(&experience), sizeof(experience));
        out.write(reinterpret_cast<const char*>(&health), sizeof(health));
        out.write(reinterpret_cast<const char*>(&maxHealth), sizeof(maxHealth));
        out.write(playerName, sizeof(playerName));
        out.write(reinterpret_cast<const char*>(&coinsCollected), sizeof(coinsCollected));
    }

    void deserialize(std::ifstream& in) {
        in.read(reinterpret_cast<char*>(&level), sizeof(level));
        in.read(reinterpret_cast<char*>(&experience), sizeof(experience));
        in.read(reinterpret_cast<char*>(&health), sizeof(health));
        in.read(reinterpret_cast<char*>(&maxHealth), sizeof(maxHealth));
        in.read(playerName, sizeof(playerName));
        in.read(reinterpret_cast<char*>(&coinsCollected), sizeof(coinsCollected));
    }

    void display() const {
        std::cout << "Player: " << playerName << '\n'
                  << "Level: " << level << '\n'
                  << "Experience: " << experience << '\n'
                  << "Health: " << health << "/" << maxHealth << '\n'
                  << "Coins: " << coinsCollected << '\n';
    }
};

void play_game(GameState& state) {
    std::cout << "\n=== 게임 진행 ===\n";
    std::cout << "레벨: " << state.level << ", 코인: " << state.coinsCollected << '\n';

    int action;
    std::cout << "1. 경험치 획득 2. 코인 수집 3. 데미지 받음\n선택: ";
    std::cin >> action;

    switch (action) {
        case 1:
            state.experience += 10;
            std::cout << "경험치 +10\n";
            if (state.experience >= 100) {
                state.level++;
                state.experience = 0;
                state.maxHealth += 10;
                state.health = state.maxHealth;
                std::cout << "레벨 업! 새 레벨: " << state.level << '\n';
            }
            break;
        case 2:
            state.coinsCollected += 5 + (std::rand() % 10);
            std::cout << "코인 수집!\n";
            break;
        case 3:
            state.health -= 10;
            if (state.health < 0) state.health = 0;
            std::cout << "데미지 -10\n";
            break;
        default:
            std::cout << "잘못된 선택\n";
    }

    state.display();
}

int main() {
    std::srand(std::time(nullptr));

    GameState state = {1, 0, 100, 100, "Hero", 0};

    int choice;
    std::cout << "=== 어드벤처 게임 세이브 시스템 ===\n";
    std::cout << "1. 새 게임 2. 저장된 게임 로드\n선택: ";
    std::cin >> choice;
    std::cin.ignore();

    if (choice == 2) {
        std::ifstream in("savegame.bin", std::ios::binary);
        if (in) {
            state.deserialize(in);
            in.close();
            std::cout << "\n저장된 게임을 로드했습니다.\n";
            state.display();
        } else {
            std::cout << "저장된 게임이 없습니다.\n";
        }
    } else {
        std::cout << "플레이어 이름: ";
        std::cin.getline(state.playerName, sizeof(state.playerName));
        std::cout << "새 게임을 시작합니다!\n";
        state.display();
    }

    play_game(state);

    int save_choice;
    std::cout << "\n게임을 저장하시겠습니까? (1. 예 2. 아니오): ";
    std::cin >> save_choice;

    if (save_choice == 1) {
        std::ofstream out("savegame.bin", std::ios::binary);
        if (out) {
            state.serialize(out);
            out.close();
            std::cout << "게임을 저장했습니다.\n";
        } else {
            std::cerr << "저장 실패\n";
            return 1;
        }
    }

    std::cout << "\n게임을 종료합니다.\n";
    return 0;
}
