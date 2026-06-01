#include <iostream>
#include <fstream>
#include <cstring>
#include <string>

struct Person {
    char name[64];
    int age;
    double score;
};

void serialize(const std::string& filename, const Person& p) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "파일 생성 실패\n";
        return;
    }

    file.write(p.name, sizeof(p.name));
    file.write(reinterpret_cast<const char*>(&p.age), sizeof(p.age));
    file.write(reinterpret_cast<const char*>(&p.score), sizeof(p.score));

    if (file.fail()) {
        std::cerr << "파일 쓰기 실패\n";
    }
    file.close();
}

bool deserialize(const std::string& filename, Person& p) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "파일 열기 실패\n";
        return false;
    }

    file.read(p.name, sizeof(p.name));
    file.read(reinterpret_cast<char*>(&p.age), sizeof(p.age));
    file.read(reinterpret_cast<char*>(&p.score), sizeof(p.score));

    if (file.fail()) {
        std::cerr << "파일 읽기 실패\n";
        return false;
    }
    file.close();
    return true;
}

int main() {
    Person original{"Bob", 30, 87.5};

    std::cout << "=== 직렬화 ===\n";
    std::cout << "저장할 데이터: " << original.name << ", " << original.age << ", " << original.score << "\n";
    serialize("person.bin", original);
    std::cout << "파일에 저장 완료\n\n";

    std::cout << "=== 역직렬화 ===\n";
    Person loaded{};
    if (deserialize("person.bin", loaded)) {
        std::cout << "파일에서 복원: " << loaded.name << ", " << loaded.age << ", " << loaded.score << "\n";
        std::cout << "데이터 일치: ";
        if (std::string(original.name) == loaded.name && original.age == loaded.age
            && original.score == loaded.score) {
            std::cout << "성공 ✓\n";
        } else {
            std::cout << "실패 ✗\n";
        }
    }

    return 0;
}
