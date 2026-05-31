#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>

struct Person {
    uint32_t id;
    char name[64];
    uint8_t age;
    float salary;
};

void serialize(std::ofstream& file, const Person& person) {
    file.write(reinterpret_cast<const char*>(&person.id), sizeof(person.id));
    file.write(person.name, sizeof(person.name));
    file.write(reinterpret_cast<const char*>(&person.age), sizeof(person.age));
    file.write(reinterpret_cast<const char*>(&person.salary), sizeof(person.salary));
}

bool deserialize(std::ifstream& file, Person& person) {
    file.read(reinterpret_cast<char*>(&person.id), sizeof(person.id));
    if (!file) return false;

    file.read(person.name, sizeof(person.name));
    if (!file) return false;

    file.read(reinterpret_cast<char*>(&person.age), sizeof(person.age));
    if (!file) return false;

    file.read(reinterpret_cast<char*>(&person.salary), sizeof(person.salary));
    if (!file) return false;

    return true;
}

int main() {
    const char* filename = "persons.bin";

    // 저장
    {
        std::ofstream out(filename, std::ios::binary);
        if (!out) {
            std::cerr << "파일 열기 실패\n";
            return 1;
        }

        Person p1 = {1, "Alice", 30, 50000.0f};
        Person p2 = {2, "Bob", 25, 45000.0f};

        serialize(out, p1);
        serialize(out, p2);
        out.close();

        std::cout << "저장 완료: " << filename << "\n";
    }

    // 불러오기
    {
        std::ifstream in(filename, std::ios::binary);
        if (!in) {
            std::cerr << "파일 열기 실패\n";
            return 1;
        }

        Person p;
        int count = 0;
        while (deserialize(in, p)) {
            std::cout << "ID: " << p.id << ", 이름: " << p.name
                      << ", 나이: " << static_cast<int>(p.age)
                      << ", 급여: " << p.salary << "\n";
            count++;
        }

        std::cout << "읽음: " << count << "명\n";
        in.close();
    }

    return 0;
}
