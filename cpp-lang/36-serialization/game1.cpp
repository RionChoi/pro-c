#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <vector>
#include <cstdlib>

#pragma pack(1)
struct Item {
    char name[32];
    uint16_t quantity;
};
#pragma pack()

#pragma pack(1)
struct Character {
    char name[64];
    uint32_t level;
    uint32_t experience;
    uint32_t health;
    uint32_t max_health;
    Item inventory[5];
};
#pragma pack()

uint32_t hton32(uint32_t value) {
    return ((value & 0xFF) << 24) |
           ((value & 0xFF00) << 8) |
           ((value & 0xFF0000) >> 8) |
           ((value & 0xFF000000) >> 24);
}

uint32_t ntoh32(uint32_t value) {
    return hton32(value);
}

uint16_t hton16(uint16_t value) {
    return ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
}

uint16_t ntoh16(uint16_t value) {
    return hton16(value);
}

void saveCharacter(const Character& ch, const char* filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open save file\n";
        return;
    }

    uint32_t magic = hton32(0x52504747);
    file.write(reinterpret_cast<const char*>(&magic), sizeof(uint32_t));

    Character temp = ch;
    temp.level = hton32(ch.level);
    temp.experience = hton32(ch.experience);
    temp.health = hton32(ch.health);
    temp.max_health = hton32(ch.max_health);

    for (int i = 0; i < 5; ++i) {
        temp.inventory[i].quantity = hton16(ch.inventory[i].quantity);
    }

    file.write(reinterpret_cast<const char*>(&temp), sizeof(Character));
    std::cout << "Character saved to " << filename << "\n";
}

bool loadCharacter(Character& ch, const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open save file\n";
        return false;
    }

    uint32_t magic;
    if (!file.read(reinterpret_cast<char*>(&magic), sizeof(uint32_t))) {
        std::cerr << "Failed to read magic number\n";
        return false;
    }

    if (ntoh32(magic) != 0x52504747) {
        std::cerr << "Invalid save file format\n";
        return false;
    }

    Character temp;
    if (!file.read(reinterpret_cast<char*>(&temp), sizeof(Character))) {
        std::cerr << "Failed to read character data\n";
        return false;
    }

    ch.level = ntoh32(temp.level);
    ch.experience = ntoh32(temp.experience);
    ch.health = ntoh32(temp.health);
    ch.max_health = ntoh32(temp.max_health);
    std::strcpy(ch.name, temp.name);

    for (int i = 0; i < 5; ++i) {
        std::strcpy(ch.inventory[i].name, temp.inventory[i].name);
        ch.inventory[i].quantity = ntoh16(temp.inventory[i].quantity);
    }

    std::cout << "Character loaded from " << filename << "\n";
    return true;
}

void printCharacter(const Character& ch) {
    std::cout << "\n=== Character Status ===\n";
    std::cout << "Name: " << ch.name << "\n";
    std::cout << "Level: " << ch.level << "\n";
    std::cout << "Experience: " << ch.experience << "\n";
    std::cout << "Health: " << ch.health << " / " << ch.max_health << "\n";
    std::cout << "Inventory:\n";
    for (int i = 0; i < 5; ++i) {
        if (ch.inventory[i].quantity > 0) {
            std::cout << "  " << ch.inventory[i].name
                      << " x " << ch.inventory[i].quantity << "\n";
        }
    }
}

int main() {
    Character hero;
    std::strcpy(hero.name, "Archer");
    hero.level = 15;
    hero.experience = 5000;
    hero.health = 85;
    hero.max_health = 100;

    std::strcpy(hero.inventory[0].name, "Sword");
    hero.inventory[0].quantity = 1;
    std::strcpy(hero.inventory[1].name, "Potion");
    hero.inventory[1].quantity = 10;
    std::strcpy(hero.inventory[2].name, "Scroll");
    hero.inventory[2].quantity = 3;
    std::strcpy(hero.inventory[3].name, "Gem");
    hero.inventory[3].quantity = 5;
    std::strcpy(hero.inventory[4].name, "Key");
    hero.inventory[4].quantity = 2;

    std::cout << "Original character:\n";
    printCharacter(hero);

    Character original = hero;
    saveCharacter(hero, "hero_save.dat");

    hero.health = 10;
    hero.experience = 0;
    hero.level = 1;

    Character loaded;
    if (loadCharacter(loaded, "hero_save.dat")) {
        std::cout << "\nLoaded character:\n";
        printCharacter(loaded);

        bool match = (original.level == loaded.level &&
                      original.experience == loaded.experience &&
                      std::strcmp(original.name, loaded.name) == 0);

        if (match) {
            std::cout << "\n✓ Save/Load test passed\n";
            return 0;
        } else {
            std::cout << "\n✗ Save/Load test failed\n";
            return 1;
        }
    }

    return 1;
}
