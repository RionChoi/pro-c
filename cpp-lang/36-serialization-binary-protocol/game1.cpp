#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <algorithm>

struct GameSession {
    uint32_t session_id;
    uint32_t score;
    uint16_t duration_seconds;
    uint8_t difficulty;  // 1=Easy, 2=Normal, 3=Hard
    char player_name[32];
};

class GameSessionManager {
public:
    void save_to_binary(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "저장 실패\n";
            return;
        }

        uint32_t count = sessions.size();
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (const auto& s : sessions) {
            file.write(reinterpret_cast<const char*>(&s.session_id), sizeof(s.session_id));
            file.write(reinterpret_cast<const char*>(&s.score), sizeof(s.score));
            file.write(reinterpret_cast<const char*>(&s.duration_seconds), sizeof(s.duration_seconds));
            file.write(reinterpret_cast<const char*>(&s.difficulty), sizeof(s.difficulty));
            file.write(s.player_name, sizeof(s.player_name));
        }

        file.close();
        std::cout << "✓ " << count << "개 세션을 이진 형식으로 저장\n";
    }

    bool load_from_binary(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) return false;

        uint32_t count = 0;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));

        sessions.clear();
        for (uint32_t i = 0; i < count; ++i) {
            GameSession s;
            file.read(reinterpret_cast<char*>(&s.session_id), sizeof(s.session_id));
            file.read(reinterpret_cast<char*>(&s.score), sizeof(s.score));
            file.read(reinterpret_cast<char*>(&s.duration_seconds), sizeof(s.duration_seconds));
            file.read(reinterpret_cast<char*>(&s.difficulty), sizeof(s.difficulty));
            file.read(s.player_name, sizeof(s.player_name));
            sessions.push_back(s);
        }

        file.close();
        return true;
    }

    void save_to_json(const std::string& filename) {
        std::ofstream file(filename);
        if (!file) {
            std::cerr << "저장 실패\n";
            return;
        }

        file << "[\n";
        for (size_t i = 0; i < sessions.size(); ++i) {
            const auto& s = sessions[i];
            file << "  {\n";
            file << "    \"session_id\": " << s.session_id << ",\n";
            file << "    \"score\": " << s.score << ",\n";
            file << "    \"duration\": " << s.duration_seconds << ",\n";
            file << "    \"difficulty\": " << (int)s.difficulty << ",\n";
            file << "    \"player\": \"" << s.player_name << "\"\n";
            file << "  }";
            if (i < sessions.size() - 1) file << ",";
            file << "\n";
        }
        file << "]\n";

        file.close();
        std::cout << "✓ " << sessions.size() << "개 세션을 JSON 형식으로 저장\n";
    }

    void add_session(const GameSession& s) {
        sessions.push_back(s);
    }

    void show_top_10() {
        std::vector<GameSession> sorted = sessions;
        std::sort(sorted.begin(), sorted.end(),
                  [](const GameSession& a, const GameSession& b) {
                      return a.score > b.score;
                  });

        std::cout << "\n┌─ 상위 10 세션 ─────────────────────────────────┐\n";
        int count = 0;
        for (const auto& s : sorted) {
            if (count >= 10) break;
            const char* diff_str = (s.difficulty == 1) ? "Easy" :
                                   (s.difficulty == 2) ? "Normal" : "Hard";
            printf("│ #%d | %s | 점수: %u | 시간: %us | %s\n",
                   ++count, s.player_name, s.score, s.duration_seconds, diff_str);
        }
        std::cout << "└───────────────────────────────────────────────┘\n";
    }

    void compare_sizes() {
        std::cout << "\n파일 크기 비교:\n";

        save_to_binary("temp.bin");
        save_to_json("temp.json");

        std::ifstream bin("temp.bin", std::ios::binary | std::ios::ate);
        std::ifstream json("temp.json", std::ios::binary | std::ios::ate);

        size_t bin_size = bin.tellg();
        size_t json_size = json.tellg();

        std::cout << "이진 형식: " << bin_size << " 바이트\n";
        std::cout << "JSON 형식: " << json_size << " 바이트\n";
        std::cout << "압축률: " << (100.0 * bin_size / json_size) << "%\n";

        bin.close();
        json.close();
        std::remove("temp.bin");
        std::remove("temp.json");
    }

private:
    std::vector<GameSession> sessions;
};

int main() {
    srand(time(nullptr));
    GameSessionManager manager;

    std::cout << "=== 게임 세션 저장소 게임 ===\n\n";

    std::cout << "샘플 데이터 생성...\n";
    for (int i = 1; i <= 15; ++i) {
        GameSession s;
        s.session_id = i;
        s.score = 1000 + (rand() % 5000);
        s.duration_seconds = 60 + (rand() % 3600);
        s.difficulty = 1 + (rand() % 3);
        snprintf(s.player_name, sizeof(s.player_name), "Player%d", i);
        manager.add_session(s);
    }

    std::cout << "총 15개의 게임 세션 생성됨\n\n";

    manager.show_top_10();
    manager.compare_sizes();

    std::cout << "\n세션 저장 완료!\n";
    manager.save_to_binary("game_sessions.bin");
    manager.save_to_json("game_sessions.json");

    std::cout << "\n저장된 데이터 로드...\n";
    GameSessionManager loaded;
    if (loaded.load_from_binary("game_sessions.bin")) {
        std::cout << "✓ 이진 파일 로드 성공\n";
        loaded.show_top_10();
    }

    return 0;
}
