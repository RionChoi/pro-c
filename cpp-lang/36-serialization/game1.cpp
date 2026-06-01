#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>

class GameSave {
private:
    int level;
    int experience;
    int health;
    int gold;
    std::string characterName;

public:
    GameSave() : level(1), experience(0), health(100), gold(0), characterName("Hero") {}

    GameSave(int lv, int exp, int hp, int g, const std::string& name)
        : level(lv), experience(exp), health(hp), gold(g), characterName(name) {}

    // Getters
    int getLevel() const { return level; }
    int getExperience() const { return experience; }
    int getHealth() const { return health; }
    int getGold() const { return gold; }
    std::string getName() const { return characterName; }

    // Setters
    void setLevel(int lv) { level = lv; }
    void setExperience(int exp) { experience = exp; }
    void setHealth(int hp) { health = (hp > 0) ? hp : 0; }
    void setGold(int g) { gold = g; }
    void setName(const std::string& name) { characterName = name; }

    // JSON 직렬화
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\n"
            << "  \"name\": \"" << characterName << "\",\n"
            << "  \"level\": " << level << ",\n"
            << "  \"experience\": " << experience << ",\n"
            << "  \"health\": " << health << ",\n"
            << "  \"gold\": " << gold << "\n"
            << "}";
        return oss.str();
    }

    // JSON 역직렬화
    static GameSave fromJson(const std::string& json) {
        GameSave save;

        // 간단한 JSON 파싱
        auto extractValue = [&json](const std::string& key) -> std::string {
            size_t pos = json.find("\"" + key + "\":");
            if (pos == std::string::npos) return "";

            pos = json.find(":", pos) + 1;
            while (pos < json.length() && json[pos] == ' ') pos++;

            if (json[pos] == '\"') {
                size_t start = pos + 1;
                size_t end = json.find("\"", start);
                return json.substr(start, end - start);
            } else {
                size_t start = pos;
                size_t end = start;
                while (end < json.length() && json[end] != ',' && json[end] != '\n') {
                    end++;
                }
                return json.substr(start, end - start);
            }
        };

        save.characterName = extractValue("name");
        save.level = std::stoi(extractValue("level"));
        save.experience = std::stoi(extractValue("experience"));
        save.health = std::stoi(extractValue("health"));
        save.gold = std::stoi(extractValue("gold"));

        return save;
    }

    void display() const {
        std::cout << "\n=== 캐릭터 정보 ===\n"
                  << "이름: " << characterName << "\n"
                  << "레벨: " << level << "\n"
                  << "경험치: " << experience << "\n"
                  << "체력: " << health << "/100\n"
                  << "금: " << gold << "\n";
    }
};

int main() {
    std::cout << "=== 게임 세이브 시스템 ===\n";

    GameSave save;
    std::string filename = "game_save.json";

    // 기존 게임 로드
    {
        std::ifstream infile(filename);
        if (infile.good()) {
            std::string content((std::istreambuf_iterator<char>(infile)),
                               std::istreambuf_iterator<char>());
            save = GameSave::fromJson(content);
            std::cout << "\n이전 게임이 로드되었습니다.";
        } else {
            std::cout << "\n새 게임을 시작합니다.";
        }
    }

    save.display();

    // 게임 진행
    srand(static_cast<unsigned int>(time(nullptr)));
    bool playing = true;

    while (playing) {
        std::cout << "\n=== 행동 선택 ===\n"
                  << "1. 전투 (경험치 획득)\n"
                  << "2. 보물 탐색 (금 획득)\n"
                  << "3. 휴식 (체력 회복)\n"
                  << "4. 캐릭터명 변경\n"
                  << "5. 저장 및 종료\n"
                  << "선택: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "잘못된 입력입니다.\n";
            continue;
        }

        switch (choice) {
        case 1: {
            int expGain = 10 + (rand() % 20);
            save.setExperience(save.getExperience() + expGain);
            std::cout << "\n전투에서 승리했다! (+" << expGain << " EXP)\n";

            // 레벨 업 체크
            if (save.getExperience() >= 100 * save.getLevel()) {
                save.setLevel(save.getLevel() + 1);
                save.setExperience(0);
                std::cout << "★ 레벨 업! (Level " << save.getLevel() << ")\n";
            }

            // 체력 감소
            int damage = 5 + (rand() % 15);
            save.setHealth(save.getHealth() - damage);
            std::cout << "체력 " << damage << " 감소\n";

            if (save.getHealth() <= 0) {
                std::cout << "\n게임 오버! 체력이 0이 되었습니다.\n";
                playing = false;
            }
            break;
        }
        case 2: {
            int goldGain = 50 + (rand() % 100);
            save.setGold(save.getGold() + goldGain);
            std::cout << "\n보물을 발견했다! (+" << goldGain << " Gold)\n";
            break;
        }
        case 3: {
            int healed = 20 + (rand() % 20);
            int newHealth = save.getHealth() + healed;
            if (newHealth > 100) newHealth = 100;
            save.setHealth(newHealth);
            std::cout << "\n휴식했다. (+" << healed << " HP, 현재: " << newHealth << "/100)\n";
            break;
        }
        case 4: {
            std::string newName;
            std::cout << "\n새 캐릭터명: ";
            std::cin.ignore();
            std::getline(std::cin, newName);
            if (!newName.empty()) {
                save.setName(newName);
                std::cout << "캐릭터명이 변경되었습니다: " << newName << "\n";
            }
            continue;
        }
        case 5: {
            // 게임 저장
            {
                std::ofstream outfile(filename);
                outfile << save.toJson();
                std::cout << "\n게임이 저장되었습니다!\n";
            }
            save.display();
            std::cout << "\n게임을 종료합니다.\n";
            playing = false;
            break;
        }
        default:
            std::cout << "유효하지 않은 선택입니다.\n";
        }
    }

    return 0;
}
