#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <cstring>

struct ScoreEntry {
    char name[64];
    int32_t score;
    uint32_t timestamp;
};

class ScoreBoard {
    std::vector<ScoreEntry> scores_;
    std::string filename_;

public:
    ScoreBoard(const std::string& filename) : filename_(filename) {
        load();
    }

    void save() {
        std::ofstream out(filename_, std::ios::binary);
        if (!out) return;

        uint32_t count = scores_.size();
        out.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (const auto& entry : scores_) {
            out.write(entry.name, sizeof(entry.name));
            out.write(reinterpret_cast<const char*>(&entry.score), sizeof(entry.score));
            out.write(reinterpret_cast<const char*>(&entry.timestamp), sizeof(entry.timestamp));
        }

        out.close();
    }

    void load() {
        std::ifstream in(filename_, std::ios::binary);
        if (!in) return;

        uint32_t count = 0;
        in.read(reinterpret_cast<char*>(&count), sizeof(count));

        for (uint32_t i = 0; i < count; ++i) {
            ScoreEntry entry;
            in.read(entry.name, sizeof(entry.name));
            in.read(reinterpret_cast<char*>(&entry.score), sizeof(entry.score));
            in.read(reinterpret_cast<char*>(&entry.timestamp), sizeof(entry.timestamp));
            scores_.push_back(entry);
        }

        in.close();
    }

    void add_score(const std::string& name, int32_t score) {
        ScoreEntry entry;
        strncpy(entry.name, name.c_str(), sizeof(entry.name) - 1);
        entry.name[sizeof(entry.name) - 1] = '\0';
        entry.score = score;
        entry.timestamp = std::time(nullptr);
        scores_.push_back(entry);

        std::sort(scores_.begin(), scores_.end(),
            [](const ScoreEntry& a, const ScoreEntry& b) {
                return a.score > b.score;
            });

        if (scores_.size() > 10) {
            scores_.erase(scores_.begin() + 10, scores_.end());
        }

        save();
    }

    void display() {
        std::cout << "\n=== 상위 10명 랭킹 ===\n";
        for (size_t i = 0; i < scores_.size(); ++i) {
            std::cout << (i + 1) << ". " << scores_[i].name
                      << " - " << scores_[i].score << " 점\n";
        }
    }
};

int main() {
    std::srand(std::time(nullptr));
    ScoreBoard board("scores.bin");

    std::cout << "=== 점수 맞히기 게임 ===\n";

    for (int game = 0; game < 3; ++game) {
        int target = std::rand() % 100 + 1;
        int attempts = 0;
        int guess = 0;

        std::cout << "\n게임 " << (game + 1) << ": 1~100 사이의 숫자를 맞혀보세요!\n";

        while (guess != target) {
            std::cout << "추측: ";
            std::cin >> guess;
            attempts++;

            if (guess < target) {
                std::cout << "더 큰 숫자입니다.\n";
            } else if (guess > target) {
                std::cout << "더 작은 숫자입니다.\n";
            } else {
                int score = std::max(1000 - (attempts * 50), 100);
                std::cout << "정답! " << attempts << " 회 만에 맞히셨습니다.\n";
                std::cout << "점수: " << score << "\n";

                std::string name;
                std::cout << "이름 입력: ";
                std::cin.ignore();
                std::getline(std::cin, name);

                board.add_score(name, score);
            }
        }
    }

    board.display();

    return 0;
}
