#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <ctime>

struct GameState {
    int player_hp;
    int player_level;
    int enemy_hp;
    int gold;
    char player_name[64];

    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> buffer;

        uint8_t *hp_ptr = reinterpret_cast<uint8_t*>(const_cast<int*>(&player_hp));
        buffer.insert(buffer.end(), hp_ptr, hp_ptr + sizeof(int));

        uint8_t *level_ptr = reinterpret_cast<uint8_t*>(const_cast<int*>(&player_level));
        buffer.insert(buffer.end(), level_ptr, level_ptr + sizeof(int));

        uint8_t *enemy_ptr = reinterpret_cast<uint8_t*>(const_cast<int*>(&enemy_hp));
        buffer.insert(buffer.end(), enemy_ptr, enemy_ptr + sizeof(int));

        uint8_t *gold_ptr = reinterpret_cast<uint8_t*>(const_cast<int*>(&gold));
        buffer.insert(buffer.end(), gold_ptr, gold_ptr + sizeof(int));

        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(player_name),
                      reinterpret_cast<const uint8_t*>(player_name) + sizeof(player_name));

        return buffer;
    }

    void deserialize(const std::vector<uint8_t>& buffer) {
        if (buffer.size() < sizeof(int) * 4 + sizeof(player_name)) {
            std::cerr << "버퍼 크기 부족\n";
            return;
        }

        size_t offset = 0;
        std::memcpy(&player_hp, buffer.data() + offset, sizeof(int));
        offset += sizeof(int);

        std::memcpy(&player_level, buffer.data() + offset, sizeof(int));
        offset += sizeof(int);

        std::memcpy(&enemy_hp, buffer.data() + offset, sizeof(int));
        offset += sizeof(int);

        std::memcpy(&gold, buffer.data() + offset, sizeof(int));
        offset += sizeof(int);

        std::memcpy(player_name, buffer.data() + offset, sizeof(player_name));
    }
};

void display_state(const GameState& state) {
    std::cout << "\n=== 게임 상태 ===\n";
    std::cout << "플레이어: " << state.player_name << "\n";
    std::cout << "체력: " << state.player_hp << " HP\n";
    std::cout << "레벨: " << state.player_level << "\n";
    std::cout << "적 체력: " << state.enemy_hp << " HP\n";
    std::cout << "골드: " << state.gold << " G\n";
}

int main() {
    srand(time(nullptr));

    GameState state{};
    std::cout << "플레이어 이름을 입력: ";
    std::cin.getline(state.player_name, sizeof(state.player_name));

    state.player_hp = 100;
    state.player_level = 1;
    state.enemy_hp = 50;
    state.gold = 0;

    bool playing = true;
    while (playing) {
        display_state(state);

        std::cout << "\n[1] 전투 [2] 휴식 [3] 저장 [4] 로드 [5] 종료: ";
        int choice;
        std::cin >> choice;

        if (choice == 1) {
            int damage = (rand() % 15) + 5;
            state.enemy_hp -= damage;
            std::cout << "플레이어가 " << damage << " 데미지를 입혔습니다!\n";

            if (state.enemy_hp <= 0) {
                std::cout << "적을 격퇴했습니다! 50 골드 획득!\n";
                state.gold += 50;
                state.player_level++;
                state.enemy_hp = 30 + (state.player_level * 10);
            } else {
                int counter = (rand() % 12) + 3;
                state.player_hp -= counter;
                std::cout << "적이 " << counter << " 데미지를 입혔습니다!\n";
            }

            if (state.player_hp <= 0) {
                std::cout << "패배했습니다!\n";
                playing = false;
            }
        } else if (choice == 2) {
            state.player_hp = std::min(100, state.player_hp + 30);
            std::cout << "휴식하여 30 HP를 회복했습니다.\n";
        } else if (choice == 3) {
            std::vector<uint8_t> data = state.serialize();
            std::ofstream file("savegame.bin", std::ios::binary);
            file.write(reinterpret_cast<const char*>(data.data()), data.size());
            file.close();
            std::cout << "게임을 저장했습니다!\n";
        } else if (choice == 4) {
            std::ifstream file("savegame.bin", std::ios::binary);
            if (!file) {
                std::cout << "저장 파일이 없습니다.\n";
            } else {
                std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)),
                                          std::istreambuf_iterator<char>());
                file.close();
                state.deserialize(data);
                std::cout << "게임을 로드했습니다!\n";
            }
        } else if (choice == 5) {
            playing = false;
            std::cout << "게임을 종료합니다.\n";
        } else {
            std::cout << "잘못된 선택입니다.\n";
        }
    }

    return 0;
}
