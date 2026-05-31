#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>
#include <cstdlib>
#include <ctime>

struct Item {
    std::string name;
    int quantity;
};

struct Player {
    std::string name;
    int level;
    int experience;
    int health;
    std::vector<Item> inventory;
};

template <typename T>
class StructReflection {
public:
    template <typename Func>
    static void for_each_member(T& obj, Func func);
};

template <>
class StructReflection<Item> {
public:
    template <typename Func>
    static void for_each_member(Item& obj, Func func) {
        func("name", obj.name);
        func("quantity", obj.quantity);
    }
};

template <>
class StructReflection<Player> {
public:
    template <typename Func>
    static void for_each_member(Player& obj, Func func) {
        func("name", obj.name);
        func("level", obj.level);
        func("experience", obj.experience);
        func("health", obj.health);
    }
};

std::string escape_json_string(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c;
        }
    }
    return result;
}

std::string to_json_value(const std::string& value) {
    return "\"" + escape_json_string(value) + "\"";
}

std::string to_json_value(int value) {
    return std::to_string(value);
}

std::string to_json_value(double value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string to_json_value(const Item& item) {
    std::string json = "{";
    json += "\"name\": \"" + escape_json_string(item.name) + "\", ";
    json += "\"quantity\": " + std::to_string(item.quantity);
    json += "}";
    return json;
}

std::string player_to_json(const Player& player) {
    std::string json = "{\n";
    json += "  \"name\": \"" + escape_json_string(player.name) + "\",\n";
    json += "  \"level\": " + std::to_string(player.level) + ",\n";
    json += "  \"experience\": " + std::to_string(player.experience) + ",\n";
    json += "  \"health\": " + std::to_string(player.health) + ",\n";
    json += "  \"inventory\": [\n";

    for (size_t i = 0; i < player.inventory.size(); ++i) {
        json += "    " + to_json_value(player.inventory[i]);
        if (i < player.inventory.size() - 1) {
            json += ",";
        }
        json += "\n";
    }

    json += "  ]\n";
    json += "}\n";
    return json;
}

bool json_to_player(const std::string& json_str, Player& player) {
    size_t name_start = json_str.find("\"name\": \"") + 9;
    size_t name_end = json_str.find("\"", name_start);
    player.name = json_str.substr(name_start, name_end - name_start);

    size_t level_start = json_str.find("\"level\": ") + 9;
    size_t level_end = json_str.find(",", level_start);
    player.level = std::stoi(json_str.substr(level_start, level_end - level_start));

    size_t exp_start = json_str.find("\"experience\": ") + 14;
    size_t exp_end = json_str.find(",", exp_start);
    player.experience = std::stoi(json_str.substr(exp_start, exp_end - exp_start));

    size_t health_start = json_str.find("\"health\": ") + 10;
    size_t health_end = json_str.find(",", health_start);
    player.health = std::stoi(json_str.substr(health_start, health_end - health_start));

    player.inventory.clear();
    size_t inv_start = json_str.find("\"inventory\": [");
    if (inv_start != std::string::npos) {
        size_t item_start = inv_start + 14;
        while (true) {
            size_t obj_start = json_str.find("{", item_start);
            if (obj_start == std::string::npos) break;

            size_t obj_end = json_str.find("}", obj_start);
            if (obj_end == std::string::npos) break;

            std::string item_json = json_str.substr(obj_start, obj_end - obj_start + 1);

            Item item;
            size_t item_name_start = item_json.find("\"name\": \"") + 9;
            size_t item_name_end = item_json.find("\"", item_name_start);
            item.name = item_json.substr(item_name_start, item_name_end - item_name_start);

            size_t item_qty_start = item_json.find("\"quantity\": ") + 12;
            size_t item_qty_end = item_json.find("}", item_qty_start);
            item.quantity = std::stoi(item_json.substr(item_qty_start, item_qty_end - item_qty_start));

            player.inventory.push_back(item);
            item_start = obj_end + 1;
        }
    }

    return true;
}

bool save_player_json(const std::string& filename, const Player& player) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "파일 열기 실패" << std::endl;
        return false;
    }

    file << player_to_json(player);
    file.close();
    return true;
}

bool load_player_json(const std::string& filename, Player& player) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "파일 열기 실패" << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return json_to_player(buffer.str(), player);
}

void display_player(const Player& player) {
    std::cout << "\n=== 플레이어 상태 ===" << std::endl;
    std::cout << "이름: " << player.name << std::endl;
    std::cout << "레벨: " << player.level << std::endl;
    std::cout << "경험치: " << player.experience << std::endl;
    std::cout << "체력: " << player.health << std::endl;
    std::cout << "인벤토리 (" << player.inventory.size() << " 개):" << std::endl;
    for (const auto& item : player.inventory) {
        std::cout << "  - " << item.name << " x" << item.quantity << std::endl;
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    Player player;
    player.name = "용사";
    player.level = 1;
    player.experience = 0;
    player.health = 100;
    player.inventory.push_back({"검", 1});
    player.inventory.push_back({"포션", 5});

    std::cout << "=== RPG 게임 (JSON 직렬화) ===" << std::endl;

    while (true) {
        std::cout << "\n1. 게임 진행" << std::endl;
        std::cout << "2. 플레이어 상태 확인" << std::endl;
        std::cout << "3. JSON으로 저장" << std::endl;
        std::cout << "4. JSON에서 로드" << std::endl;
        std::cout << "5. JSON 내용 확인" << std::endl;
        std::cout << "6. 종료" << std::endl;
        std::cout << "선택: ";

        int choice;
        if (std::cin >> choice) {
            switch (choice) {
                case 1: {
                    int damage = rand() % 20 + 5;
                    player.health -= damage;
                    if (player.health < 0) player.health = 0;

                    int reward = rand() % 30 + 10;
                    player.experience += reward;
                    player.level = 1 + player.experience / 100;

                    std::cout << "몬스터와 전투! 피해: " << damage << ", 경험치: " << reward << std::endl;

                    if (player.health == 0) {
                        std::cout << "게임 오버!" << std::endl;
                        return 0;
                    }
                    break;
                }
                case 2: {
                    display_player(player);
                    break;
                }
                case 3: {
                    if (save_player_json("player.json", player)) {
                        std::cout << "플레이어 정보를 JSON으로 저장했습니다." << std::endl;
                    }
                    break;
                }
                case 4: {
                    if (load_player_json("player.json", player)) {
                        std::cout << "플레이어 정보를 JSON에서 로드했습니다." << std::endl;
                        display_player(player);
                    }
                    break;
                }
                case 5: {
                    std::ifstream file("player.json");
                    if (file) {
                        std::cout << "\n=== JSON 파일 내용 ===" << std::endl;
                        std::string line;
                        while (std::getline(file, line)) {
                            std::cout << line << std::endl;
                        }
                        file.close();
                    } else {
                        std::cerr << "JSON 파일이 없습니다." << std::endl;
                    }
                    break;
                }
                case 6: {
                    std::cout << "게임을 종료합니다." << std::endl;
                    return 0;
                }
                default:
                    std::cout << "잘못된 선택입니다." << std::endl;
            }
        }
    }

    return 0;
}
