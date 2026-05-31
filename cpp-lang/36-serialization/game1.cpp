#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <cstdlib>

struct GameState {
    static const uint32_t MAGIC = 0x47414D45; // "GAME"
    static const uint8_t VERSION = 1;

    uint32_t magic;
    uint8_t version;
    uint32_t level;
    uint32_t score;
    uint32_t health;
    uint32_t coins;
    char playerName[64];
    uint32_t playtime;
};

bool saveGame(const char* filename, const GameState& state) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "저장 실패: 파일 열기 오류\n";
        return false;
    }

    file.write(reinterpret_cast<const char*>(&state.magic), sizeof(state.magic));
    file.write(reinterpret_cast<const char*>(&state.version), sizeof(state.version));
    file.write(reinterpret_cast<const char*>(&state.level), sizeof(state.level));
    file.write(reinterpret_cast<const char*>(&state.score), sizeof(state.score));
    file.write(reinterpret_cast<const char*>(&state.health), sizeof(state.health));
    file.write(reinterpret_cast<const char*>(&state.coins), sizeof(state.coins));
    file.write(state.playerName, sizeof(state.playerName));
    file.write(reinterpret_cast<const char*>(&state.playtime), sizeof(state.playtime));

    file.close();
    return true;
}

bool loadGame(const char* filename, GameState& state) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "로드 실패: 파일 없음\n";
        return false;
    }

    file.read(reinterpret_cast<char*>(&state.magic), sizeof(state.magic));
    if (!file || state.magic != GameState::MAGIC) {
        std::cerr << "오류: 유효하지 않은 파일\n";
        return false;
    }

    file.read(reinterpret_cast<char*>(&state.version), sizeof(state.version));
    if (!file || state.version != GameState::VERSION) {
        std::cerr << "오류: 버전 불일치\n";
        return false;
    }

    file.read(reinterpret_cast<char*>(&state.level), sizeof(state.level));
    file.read(reinterpret_cast<char*>(&state.score), sizeof(state.score));
    file.read(reinterpret_cast<char*>(&state.health), sizeof(state.health));
    file.read(reinterpret_cast<char*>(&state.coins), sizeof(state.coins));
    file.read(state.playerName, sizeof(state.playerName));
    file.read(reinterpret_cast<char*>(&state.playtime), sizeof(state.playtime));

    file.close();
    return file.gcount() > 0;
}

void displayState(const GameState& state) {
    std::cout << "플레이어: " << state.playerName << "\n";
    std::cout << "레벨: " << state.level << ", 점수: " << state.score << "\n";
    std::cout << "체력: " << state.health << ", 코인: " << state.coins << "\n";
    std::cout << "플레이 시간: " << state.playtime << "초\n";
}

int main() {
    const char* savefile = "gamesave.dat";

    GameState state;
    state.magic = GameState::MAGIC;
    state.version = GameState::VERSION;
    state.level = 3;
    state.score = 15000;
    state.health = 80;
    state.coins = 250;
    std::strncpy(state.playerName, "Hero", sizeof(state.playerName) - 1);
    state.playerName[sizeof(state.playerName) - 1] = '\0';
    state.playtime = 3600;

    // 저장
    std::cout << "게임 상태 저장 중...\n";
    if (!saveGame(savefile, state)) {
        return 1;
    }
    std::cout << "저장 완료!\n\n";

    // 상태 초기화
    std::memset(&state, 0, sizeof(state));

    // 로드
    std::cout << "게임 상태 로드 중...\n";
    if (!loadGame(savefile, state)) {
        return 1;
    }
    std::cout << "로드 완료!\n\n";

    // 표시
    std::cout << "저장된 게임 상태:\n";
    displayState(state);

    return 0;
}
