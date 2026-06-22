#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

struct Item {
    std::string name;
    int effect;

    Item(const std::string& n = "", int e = 0) : name(n), effect(e) {}

    std::string to_json() const {
        std::ostringstream oss;
        oss << "{\"name\": \"" << name << "\", \"effect\": " << effect << "}";
        return oss.str();
    }
};

class Character {
public:
    std::string name;
    int hp;
    int max_hp;
    int exp;
    std::vector<Item> inventory;

    Character(const std::string& n = "Unknown", int h = 100)
        : name(n), hp(h), max_hp(h), exp(0) {}

    void add_item(const std::string& item_name, int effect) {
        inventory.push_back(Item(item_name, effect));
        std::cout << "[획득] " << item_name << " (효과: " << effect << ")"
                  << std::endl;
    }

    void take_damage(int damage) {
        hp = std::max(0, hp - damage);
        std::cout << "[피해] " << damage << "만큼 피해를 입었습니다!" << std::endl;
        std::cout << "현재 HP: " << hp << "/" << max_hp << std::endl;
    }

    void heal(int amount) {
        hp = std::min(max_hp, hp + amount);
        std::cout << "[회복] " << amount << "만큼 회복되었습니다!" << std::endl;
        std::cout << "현재 HP: " << hp << "/" << max_hp << std::endl;
    }

    void gain_exp(int amount) {
        exp += amount;
        std::cout << "[경험치] " << amount << "을(를) 획득했습니다!" << std::endl;
        std::cout << "총 경험치: " << exp << std::endl;
    }

    void display_status() const {
        std::cout << "\n=== [" << name << "] 캐릭터 정보 ===" << std::endl;
        std::cout << "HP: " << hp << "/" << max_hp << std::endl;
        std::cout << "경험치: " << exp << std::endl;
        std::cout << "인벤토리 (" << inventory.size() << "개):" << std::endl;
        for (size_t i = 0; i < inventory.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << inventory[i].name
                      << " (효과: " << inventory[i].effect << ")"
                      << std::endl;
        }
        std::cout << std::endl;
    }

    std::string to_json() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"name\": \"" << name << "\",\n";
        oss << "  \"hp\": " << hp << ",\n";
        oss << "  \"max_hp\": " << max_hp << ",\n";
        oss << "  \"exp\": " << exp << ",\n";
        oss << "  \"inventory\": [\n";
        for (size_t i = 0; i < inventory.size(); ++i) {
            if (i > 0) oss << ",\n";
            oss << "    " << inventory[i].to_json();
        }
        oss << "\n  ]\n";
        oss << "}";
        return oss.str();
    }

    void save_to_file(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file) {
            std::cerr << "파일 저장 실패" << std::endl;
            return;
        }
        file << to_json();
        file.close();
        std::cout << "[저장] " << filename << "에 저장되었습니다." << std::endl;
    }

    static Character load_from_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "파일 로드 실패" << std::endl;
            return Character("Unknown");
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string json = buffer.str();
        file.close();

        Character ch;

        size_t name_pos = json.find("\"name\": \"");
        if (name_pos != std::string::npos) {
            size_t start = name_pos + 10;
            size_t end = json.find("\"", start);
            ch.name = json.substr(start, end - start);
        }

        size_t hp_pos = json.find("\"hp\": ");
        if (hp_pos != std::string::npos) {
            ch.hp = std::stoi(json.substr(hp_pos + 6));
        }

        size_t max_hp_pos = json.find("\"max_hp\": ");
        if (max_hp_pos != std::string::npos) {
            ch.max_hp = std::stoi(json.substr(max_hp_pos + 10));
        }

        size_t exp_pos = json.find("\"exp\": ");
        if (exp_pos != std::string::npos) {
            ch.exp = std::stoi(json.substr(exp_pos + 7));
        }

        size_t inv_pos = json.find("\"inventory\": [");
        if (inv_pos != std::string::npos) {
            size_t inv_end = json.find("]", inv_pos);
            std::string inv_str = json.substr(inv_pos + 14, inv_end - (inv_pos + 14));

            size_t item_start = 0;
            while (item_start < inv_str.length()) {
                size_t item_begin = inv_str.find("{", item_start);
                if (item_begin == std::string::npos) break;

                size_t name_start = inv_str.find("\"name\": \"", item_begin);
                if (name_start != std::string::npos) {
                    size_t name_end = inv_str.find("\"", name_start + 10);
                    std::string item_name = inv_str.substr(name_start + 10,
                                                           name_end - (name_start + 10));

                    size_t effect_start = inv_str.find("\"effect\": ", name_end);
                    if (effect_start != std::string::npos) {
                        int effect = std::stoi(inv_str.substr(effect_start + 10));
                        ch.inventory.push_back(Item(item_name, effect));
                    }
                }
                item_start = item_begin + 1;
            }
        }

        return ch;
    }
};

int main() {
    std::cout << "=== 캐릭터 관리 게임 ===" << std::endl;

    Character player;
    bool running = true;

    while (running) {
        std::cout << "\n[메뉴] 1.생성 2.로드 3.정보 4.게임진행 5.저장 6.종료: ";
        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1: {
            std::cout << "캐릭터 이름: ";
            std::cin >> player.name;
            player.hp = player.max_hp = 100;
            player.exp = 0;
            player.inventory.clear();
            std::cout << "[생성] " << player.name
                      << " 캐릭터가 생성되었습니다." << std::endl;
            break;
        }
        case 2: {
            std::cout << "파일명: ";
            std::string filename;
            std::cin >> filename;
            player = Character::load_from_file(filename);
            std::cout << "[로드] " << player.name << " 캐릭터가 로드되었습니다."
                      << std::endl;
            break;
        }
        case 3: {
            player.display_status();
            break;
        }
        case 4: {
            std::cout << "\n[게임진행] 1.몬스터와 전투 2.보물상자 3.종료: ";
            int game_choice;
            std::cin >> game_choice;

            if (game_choice == 1) {
                player.take_damage(25);
                if (player.hp > 0) {
                    player.heal(15);
                    player.gain_exp(50);
                } else {
                    std::cout << "[패배] 캐릭터가 쓰러졌습니다!" << std::endl;
                    player.hp = 1;
                }
            } else if (game_choice == 2) {
                player.add_item("회복 포션", 30);
                player.gain_exp(30);
            }
            break;
        }
        case 5: {
            std::cout << "파일명: ";
            std::string filename;
            std::cin >> filename;
            player.save_to_file(filename);
            break;
        }
        case 6: {
            running = false;
            std::cout << "게임을 종료합니다." << std::endl;
            break;
        }
        default:
            std::cout << "잘못된 선택입니다." << std::endl;
        }
    }

    return 0;
}
