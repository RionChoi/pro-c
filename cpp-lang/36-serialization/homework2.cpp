#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <cstring>

const uint16_t MAGIC_NUMBER = 0xCAFE;
const uint8_t PROTOCOL_VERSION = 1;

struct Student {
    std::string name;
    int age;
    double gpa;
};

bool saveBinary(const char* filename, const std::vector<Student>& students) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "파일 열기 실패: " << filename << "\n";
        return false;
    }

    file.write(reinterpret_cast<const char*>(&MAGIC_NUMBER), sizeof(MAGIC_NUMBER));
    file.write(reinterpret_cast<const char*>(&PROTOCOL_VERSION), sizeof(PROTOCOL_VERSION));

    uint32_t count = students.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& s : students) {
        uint32_t name_len = s.name.length();
        file.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
        file.write(s.name.c_str(), name_len);

        file.write(reinterpret_cast<const char*>(&s.age), sizeof(s.age));
        file.write(reinterpret_cast<const char*>(&s.gpa), sizeof(s.gpa));
    }

    file.close();
    return true;
}

bool loadBinary(const char* filename, std::vector<Student>& students) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "파일 열기 실패: " << filename << "\n";
        return false;
    }

    uint16_t magic;
    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    if (magic != MAGIC_NUMBER) {
        std::cerr << "잘못된 파일 형식 (매직 넘버 불일치)\n";
        return false;
    }

    uint8_t version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));
    if (version != PROTOCOL_VERSION) {
        std::cerr << "지원하지 않는 버전: " << static_cast<int>(version) << "\n";
        return false;
    }

    uint32_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));

    students.clear();
    for (uint32_t i = 0; i < count; ++i) {
        uint32_t name_len;
        file.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));

        char name_buffer[256];
        file.read(name_buffer, name_len);
        name_buffer[name_len] = '\0';

        int age;
        double gpa;
        file.read(reinterpret_cast<char*>(&age), sizeof(age));
        file.read(reinterpret_cast<char*>(&gpa), sizeof(gpa));

        students.push_back({std::string(name_buffer), age, gpa});
    }

    file.close();
    return true;
}

int main() {
    std::cout << "=== 바이너리 프로토콜 직렬화 데모 ===\n\n";

    std::vector<Student> original{
        {"Alice", 20, 3.85},
        {"Bob", 21, 3.42},
        {"Charlie", 19, 3.95}
    };

    std::cout << "원본 학생 " << original.size() << "명:\n";
    for (const auto& s : original) {
        std::cout << "  - " << s.name << ", " << s.age << "세, GPA: " << s.gpa << "\n";
    }
    std::cout << "\n";

    if (saveBinary("students.bin", original)) {
        std::cout << "students.bin 파일에 저장되었습니다.\n\n";
    }

    std::vector<Student> loaded;
    if (loadBinary("students.bin", loaded)) {
        std::cout << "로드한 학생 " << loaded.size() << "명:\n";
        for (const auto& s : loaded) {
            std::cout << "  - " << s.name << ", " << s.age << "세, GPA: " << s.gpa << "\n";
        }
        std::cout << "\n검증: ";
        bool match = (original.size() == loaded.size());
        for (size_t i = 0; i < original.size() && match; ++i) {
            match = (original[i].name == loaded[i].name &&
                     original[i].age == loaded[i].age &&
                     original[i].gpa == loaded[i].gpa);
        }
        std::cout << (match ? "일치함 ✓" : "불일치 ✗") << "\n";
    }

    return 0;
}
