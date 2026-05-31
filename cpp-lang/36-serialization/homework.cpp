#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>

#pragma pack(1)
struct Person {
    uint32_t age;
    char name[64];
    double height;
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

void serializePerson(const Person& p, std::ofstream& out) {
    uint32_t age_network = hton32(p.age);
    out.write(reinterpret_cast<const char*>(&age_network), sizeof(uint32_t));
    out.write(p.name, sizeof(p.name));
    out.write(reinterpret_cast<const char*>(&p.height), sizeof(double));
}

bool deserializePerson(Person& p, std::ifstream& in) {
    uint32_t age_network;
    if (!in.read(reinterpret_cast<char*>(&age_network), sizeof(uint32_t))) {
        return false;
    }
    p.age = ntoh32(age_network);

    if (!in.read(p.name, sizeof(p.name))) {
        return false;
    }

    if (!in.read(reinterpret_cast<char*>(&p.height), sizeof(double))) {
        return false;
    }

    return true;
}

int main() {
    const char* filename = "person_data.bin";

    Person original;
    original.age = 30;
    std::strcpy(original.name, "Alice Johnson");
    original.height = 5.7;

    {
        std::ofstream outfile(filename, std::ios::binary);
        if (!outfile) {
            std::cerr << "Failed to open file for writing\n";
            return 1;
        }
        serializePerson(original, outfile);
        std::cout << "Serialized Person:\n";
        std::cout << "  Age: " << original.age << "\n";
        std::cout << "  Name: " << original.name << "\n";
        std::cout << "  Height: " << original.height << "\n";
    }

    Person restored;
    {
        std::ifstream infile(filename, std::ios::binary);
        if (!infile) {
            std::cerr << "Failed to open file for reading\n";
            return 1;
        }
        if (!deserializePerson(restored, infile)) {
            std::cerr << "Failed to deserialize\n";
            return 1;
        }
        std::cout << "\nDeserialized Person:\n";
        std::cout << "  Age: " << restored.age << "\n";
        std::cout << "  Name: " << restored.name << "\n";
        std::cout << "  Height: " << restored.height << "\n";
    }

    bool match = (original.age == restored.age &&
                  std::strcmp(original.name, restored.name) == 0 &&
                  original.height == restored.height);

    std::cout << "\nMatch: " << (match ? "Yes" : "No") << "\n";

    return match ? 0 : 1;
}
