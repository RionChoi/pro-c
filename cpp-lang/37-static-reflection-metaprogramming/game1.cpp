#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <tuple>

struct PlayerStats {
    std::string name;
    int level;
    int experience;
    int kills;
    double accuracy;

    bool isValid() const {
        return level >= 1 && level <= 100 &&
               experience >= 0 && experience <= 10000 &&
               kills >= 0 && kills <= 10000 &&
               accuracy >= 0.0 && accuracy <= 100.0;
    }
};

class PlayerValidator {
public:
    static bool validateStat(int value, int minVal, int maxVal, const std::string& fieldName) {
        if (value < minVal || value > maxVal) {
            std::cout << "  ✗ " << fieldName << " invalid: " << value
                      << " (range: " << minVal << "-" << maxVal << ")\n";
            return false;
        }
        std::cout << "  ✓ " << fieldName << ": " << value << "\n";
        return true;
    }

    static bool validateStat(double value, double minVal, double maxVal, const std::string& fieldName) {
        if (value < minVal || value > maxVal) {
            std::cout << "  ✗ " << fieldName << " invalid: " << std::fixed << std::setprecision(1)
                      << value << " (range: " << minVal << "-" << maxVal << "%)\n";
            return false;
        }
        std::cout << "  ✓ " << fieldName << ": " << std::fixed << std::setprecision(1) << value << "%\n";
        return true;
    }

    static bool validatePlayer(const PlayerStats& player) {
        std::cout << "\n=== 플레이어 검증: " << player.name << " ===\n";

        bool allValid = true;
        allValid &= validateStat(player.level, 1, 100, "Level");
        allValid &= validateStat(player.experience, 0, 10000, "Experience");
        allValid &= validateStat(player.kills, 0, 10000, "Kills");
        allValid &= validateStat(player.accuracy, 0.0, 100.0, "Accuracy");

        if (allValid) {
            std::cout << "  ✅ 모든 통계 유효\n";
        } else {
            std::cout << "  ❌ 유효하지 않은 통계 발견\n";
        }

        return allValid;
    }
};

struct Leaderboard {
    std::vector<PlayerStats> players;

    void addPlayer(const PlayerStats& player) {
        if (player.isValid()) {
            players.push_back(player);
        }
    }

    void sortByLevel() {
        std::sort(players.begin(), players.end(),
                 [](const PlayerStats& a, const PlayerStats& b) {
                     return a.level > b.level;
                 });
    }

    void sortByExperience() {
        std::sort(players.begin(), players.end(),
                 [](const PlayerStats& a, const PlayerStats& b) {
                     return a.experience > b.experience;
                 });
    }

    void sortByKills() {
        std::sort(players.begin(), players.end(),
                 [](const PlayerStats& a, const PlayerStats& b) {
                     return a.kills > b.kills;
                 });
    }

    void sortByAccuracy() {
        std::sort(players.begin(), players.end(),
                 [](const PlayerStats& a, const PlayerStats& b) {
                     return a.accuracy > b.accuracy;
                 });
    }

    void display(const std::string& sortType) const {
        std::cout << "\n=== 리더보드 (정렬: " << sortType << ") ===\n";
        std::cout << std::left << std::setw(15) << "Name"
                  << std::setw(8) << "Level"
                  << std::setw(12) << "Experience"
                  << std::setw(8) << "Kills"
                  << std::setw(12) << "Accuracy\n";
        std::cout << std::string(55, '-') << "\n";

        for (int i = 0; i < static_cast<int>(players.size()); ++i) {
            const auto& p = players[i];
            std::cout << std::left << std::setw(15) << p.name
                      << std::setw(8) << p.level
                      << std::setw(12) << p.experience
                      << std::setw(8) << p.kills
                      << std::fixed << std::setprecision(1) << p.accuracy << "%\n";
        }
    }
};

int main() {
    std::cout << "=== 플레이어 통계 검증 게임 ===\n";

    PlayerStats players[] = {
        {"Alice", 50, 5000, 250, 85.5},
        {"Bob", 45, 4500, 200, 78.3},
        {"Charlie", 60, 7500, 400, 92.1},
        {"David", 102, 5000, 150, 88.0},
        {"Eve", 30, 2000, 100, 75.0}
    };

    Leaderboard leaderboard;

    for (const auto& player : players) {
        bool isValid = PlayerValidator::validatePlayer(player);
        if (isValid) {
            leaderboard.addPlayer(player);
        }
    }

    std::cout << "\n총 유효한 플레이어: " << leaderboard.players.size() << "/"
              << (sizeof(players) / sizeof(players[0])) << "\n";

    leaderboard.sortByLevel();
    leaderboard.display("레벨 (높음→낮음)");

    leaderboard.sortByExperience();
    leaderboard.display("경험치 (높음→낮음)");

    leaderboard.sortByKills();
    leaderboard.display("킬 수 (높음→낮음)");

    leaderboard.sortByAccuracy();
    leaderboard.display("정확도 (높음→낮음)");

    return 0;
}
