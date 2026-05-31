#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>

struct GameState {
    int level;
    int experience;
    int health;
    int gold;
    char player_name[32];
    int playtime_minutes;
};

bool save_game(const char* filename, const GameState& state) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "게임 저장 실패\n";
        return false;
    }

    uint32_t magic = 0xDEADBEEF;
    uint16_t version = 1;

    file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));
    file.write(reinterpret_cast<const char*>(&state), sizeof(state));

    file.close();
    std::cout << "[저장] " << filename << "에 게임이 저장되었습니다.\n";
    return true;
}

bool load_game(const char* filename, GameState& state) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "게임 로드 실패\n";
        return false;
    }

    uint32_t magic;
    uint16_t version;

    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    if (magic != 0xDEADBEEF) {
        std::cerr << "잘못된 세이브 파일입니다.\n";
        file.close();
        return false;
    }

    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (version != 1) {
        std::cerr << "지원하지 않는 버전입니다: " << version << '\n';
        file.close();
        return false;
    }

    file.read(reinterpret_cast<char*>(&state), sizeof(state));
    file.close();

    std::cout << "[로드] " << filename << "에서 게임을 로드했습니다.\n";
    return true;
}

void display_state(const GameState& state) {
    std::cout << "\n=== 게임 상태 ===\n"
              << "플레이어: " << state.player_name << '\n'
              << "레벨: " << state.level << '\n'
              << "경험치: " << state.experience << '\n'
              << "체력: " << state.health << '\n'
              << "골드: " << state.gold << '\n'
              << "플레이 시간: " << state.playtime_minutes << "분\n";
}

int main() {
    GameState state1{5, 2500, 100, 5000, "용사", 120};

    save_game("savegame.bin", state1);

    GameState state2{};
    load_game("savegame.bin", state2);
    display_state(state2);

    state2.level = 6;
    state2.experience = 3200;
    state2.gold = 7500;
    state2.playtime_minutes = 180;
    save_game("savegame.bin", state2);

    GameState state3{};
    load_game("savegame.bin", state3);
    display_state(state3);

    return 0;
}
