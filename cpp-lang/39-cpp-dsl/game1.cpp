#include <iostream>
#include <string>
#include <vector>

class Character {
private:
    std::string name;
    std::string char_class;
    int health;
    std::vector<std::pair<std::string, int>> skills;
    std::vector<std::string> equipment;

public:
    Character() : name(""), char_class(""), health(0) {}

    class Builder {
    private:
        std::string name;
        std::string char_class;
        int health;
        std::vector<std::pair<std::string, int>> skills;
        std::vector<std::string> equipment;

    public:
        Builder() : name(""), char_class(""), health(0) {}

        Builder& withName(const std::string& n) {
            name = n;
            return *this;
        }

        Builder& ofClass(const std::string& c) {
            char_class = c;
            return *this;
        }

        Builder& withHealth(int h) {
            health = h;
            return *this;
        }

        Builder& addSkill(const std::string& skill_name, int proficiency) {
            skills.push_back({skill_name, proficiency});
            return *this;
        }

        Builder& equip(const std::string& item) {
            equipment.push_back(item);
            return *this;
        }

        Character create() const {
            Character character;
            character.name = name;
            character.char_class = char_class;
            character.health = health;
            character.skills = skills;
            character.equipment = equipment;
            return character;
        }
    };

    static Builder build() {
        return Builder();
    }

    void display() const {
        std::cout << "=== " << name << " ===" << std::endl;
        std::cout << "클래스: " << char_class << std::endl;
        std::cout << "체력: " << health << std::endl;

        if (!skills.empty()) {
            std::cout << "스킬:" << std::endl;
            for (const auto& skill : skills) {
                std::cout << "  - " << skill.first << " (숙련도: " << skill.second << "%)" << std::endl;
            }
        }

        if (!equipment.empty()) {
            std::cout << "장비:" << std::endl;
            for (const auto& item : equipment) {
                std::cout << "  - " << item << std::endl;
            }
        }
        std::cout << std::endl;
    }
};

int main() {
    std::cout << "=== 게임 캐릭터 생성기 (Character DSL) ===" << std::endl << std::endl;

    // Character 1: Warrior
    std::cout << "캐릭터 1 - 전사:" << std::endl;
    Character warrior = Character::build()
        .withName("Aragorn")
        .ofClass("Warrior")
        .withHealth(100)
        .addSkill("Sword Mastery", 90)
        .addSkill("Leadership", 80)
        .equip("Iron Sword")
        .equip("Steel Shield")
        .equip("Plate Armor")
        .create();
    warrior.display();

    // Character 2: Mage
    std::cout << "캐릭터 2 - 마법사:" << std::endl;
    Character mage = Character::build()
        .withName("Gandalf")
        .ofClass("Mage")
        .withHealth(60)
        .addSkill("Fireball", 95)
        .addSkill("Teleportation", 85)
        .addSkill("Mana Shield", 75)
        .equip("Wooden Staff")
        .equip("Robe of Power")
        .create();
    mage.display();

    // Character 3: Rogue
    std::cout << "캐릭터 3 - 도둑:" << std::endl;
    Character rogue = Character::build()
        .withName("Legolas")
        .ofClass("Rogue")
        .withHealth(70)
        .addSkill("Backstab", 92)
        .addSkill("Stealth", 88)
        .addSkill("Lockpicking", 80)
        .equip("Dagger")
        .equip("Bow")
        .equip("Leather Armor")
        .create();
    rogue.display();

    // Character 4: Paladin
    std::cout << "캐릭터 4 - 성기사:" << std::endl;
    Character paladin = Character::build()
        .withName("Uther")
        .ofClass("Paladin")
        .withHealth(95)
        .addSkill("Holy Light", 87)
        .addSkill("Divine Shield", 85)
        .addSkill("Hammer Mastery", 89)
        .equip("Holy Hammer")
        .equip("Holy Shield")
        .equip("Plate Armor")
        .equip("Holy Symbol")
        .create();
    paladin.display();

    // Character 5: Ranger
    std::cout << "캐릭터 5 - 레인저:" << std::endl;
    Character ranger = Character::build()
        .withName("Hawkeye")
        .ofClass("Ranger")
        .withHealth(80)
        .addSkill("Archery", 93)
        .addSkill("Tracking", 86)
        .addSkill("Survival", 82)
        .equip("Longbow")
        .equip("Arrow Quiver")
        .equip("Leather Armor")
        .create();
    ranger.display();

    // Character 6: Necromancer
    std::cout << "캐릭터 6 - 죽음의 마법사:" << std::endl;
    Character necromancer = Character::build()
        .withName("Sauron")
        .ofClass("Necromancer")
        .withHealth(85)
        .addSkill("Raise Dead", 99)
        .addSkill("Dark Curse", 91)
        .addSkill("Soul Drain", 88)
        .equip("Dark Scepter")
        .equip("Robe of Darkness")
        .equip("Ring of Power")
        .create();
    necromancer.display();

    std::cout << "=== 캐릭터 생성 완료 ===" << std::endl;

    return 0;
}
