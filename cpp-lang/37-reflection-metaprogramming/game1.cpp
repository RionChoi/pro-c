#include <iostream>
#include <type_traits>
#include <tuple>
#include <string>
#include <vector>
#include <sstream>

template<typename T>
class Serializer {
public:
    template<typename U = T>
    typename std::enable_if<std::is_same_v<U, bool>, std::string>::type
    serialize(const U& value) {
        return "BOOL:" + std::string(value ? "true" : "false");
    }

    template<typename U = T>
    typename std::enable_if<std::is_integral_v<U> && !std::is_same_v<U, bool>, std::string>::type
    serialize(const U& value) {
        return "INT:" + std::to_string(value);
    }

    template<typename U = T>
    typename std::enable_if<std::is_floating_point_v<U>, std::string>::type
    serialize(const U& value) {
        return "FLOAT:" + std::to_string(value);
    }

    template<typename U = T>
    typename std::enable_if<std::is_same_v<U, std::string>, std::string>::type
    serialize(const U& value) {
        return "STR:" + value;
    }
};

struct Character {
    std::string name;
    int level;
    double health;
    bool is_alive;
};

struct World {
    std::vector<Character> characters;

    void add_character(const Character& ch) {
        characters.push_back(ch);
    }

    void display() {
        std::cout << "\n=== 게임 월드 ===\n";
        for (const auto& ch : characters) {
            std::cout << "이름: " << ch.name
                      << " | 레벨: " << ch.level
                      << " | 체력: " << ch.health
                      << " | 생존: " << (ch.is_alive ? "Yes" : "No") << '\n';
        }
    }

    void serialize_characters() {
        std::cout << "\n=== 직렬화 ===\n";
        Serializer<std::string> str_ser;
        Serializer<int> int_ser;
        Serializer<double> dbl_ser;
        Serializer<bool> bool_ser;

        for (const auto& ch : characters) {
            std::cout << str_ser.serialize(ch.name) << '\n'
                      << int_ser.serialize(ch.level) << '\n'
                      << dbl_ser.serialize(ch.health) << '\n'
                      << bool_ser.serialize(ch.is_alive) << '\n';
        }
    }
};

int main() {
    World world;

    Character hero{"용사", 10, 100.0, true};
    Character wizard{"마법사", 8, 60.0, true};
    Character zombie{"좀비", 2, 30.0, true};

    world.add_character(hero);
    world.add_character(wizard);
    world.add_character(zombie);

    world.display();
    world.serialize_characters();

    std::cout << "\n=== 타입 정보 ===\n";
    std::cout << "Character::name 타입은 std::string? "
              << std::is_same_v<decltype(hero.name), std::string> << '\n';
    std::cout << "Character::level 타입은 정수? "
              << std::is_integral_v<decltype(hero.level)> << '\n';
    std::cout << "Character::health 타입은 부동소수점? "
              << std::is_floating_point_v<decltype(hero.health)> << '\n';
    std::cout << "Character::is_alive 타입은 bool? "
              << std::is_same_v<decltype(hero.is_alive), bool> << '\n';

    return 0;
}
