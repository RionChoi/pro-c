#include <iostream>
#include <tuple>
#include <string>
#include <sstream>
#include <type_traits>
#include <cstdint>

// 타입 포맷 헬퍼
template <typename T>
std::string formatValue(const T& val) {
    if constexpr (std::is_integral_v<T>) {
        return std::to_string(val);
    } else if constexpr (std::is_floating_point_v<T>) {
        std::ostringstream oss;
        oss << val;
        return oss.str();
    } else if constexpr (std::is_same_v<T, std::string>) {
        return val;
    }
    return "?";
}

// 게임 캐릭터 클래스
struct Character {
    std::string name;
    uint32_t level;
    uint32_t hp;
    float attackPower;
    std::string characterClass;

    // 리플렉션용 tuple 변환
    auto reflect() const {
        return std::make_tuple(name, level, hp, attackPower, characterClass);
    }
};

// 필드 메타데이터
struct CharacterMeta {
    static constexpr size_t fieldCount = 5;

    template <size_t Index>
    static constexpr const char* fieldName() {
        if constexpr (Index == 0) return "name";
        if constexpr (Index == 1) return "level";
        if constexpr (Index == 2) return "hp";
        if constexpr (Index == 3) return "attackPower";
        if constexpr (Index == 4) return "characterClass";
    }
};

// 캐릭터 스탯 출력
template <size_t... I>
void printCharacterStats(const Character& ch, std::index_sequence<I...>) {
    auto reflected = ch.reflect();
    std::cout << "=== 캐릭터 정보 ===\n";
    ((std::cout << CharacterMeta::fieldName<I>() << ": "
               << formatValue(std::get<I>(reflected)) << "\n"), ...);
}

void displayCharacter(const Character& ch) {
    printCharacterStats(ch, std::index_sequence_for<std::string, uint32_t,
                                                      uint32_t, float, std::string>{});
}

// 경험치 획득 시뮬레이션
void gainExperience(Character& ch, uint32_t exp) {
    std::cout << "\n" << ch.name << "이(가) " << exp << " 경험치를 획득했습니다.\n";
    if (exp >= 100) {
        ch.level++;
        ch.hp += 20;
        ch.attackPower += 2.5f;
        std::cout << "레벨 업! 새로운 레벨: " << ch.level << "\n";
    }
}

int main() {
    Character warrior = {"Conan", 5, 100, 25.5f, "Warrior"};
    Character mage = {"Merlin", 7, 60, 35.0f, "Mage"};

    std::cout << "전사 캐릭터:\n";
    displayCharacter(warrior);

    std::cout << "\n마법사 캐릭터:\n";
    displayCharacter(mage);

    gainExperience(warrior, 120);
    std::cout << "\n업그레이드된 전사:\n";
    displayCharacter(warrior);

    return 0;
}
