#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

struct Student {
    int id;
    char name[64];
    float score;

    void serialize(std::ofstream& out) const {
        out.write(reinterpret_cast<const char*>(&id), sizeof(id));
        out.write(name, sizeof(name));
        out.write(reinterpret_cast<const char*>(&score), sizeof(score));
    }

    void deserialize(std::ifstream& in) {
        in.read(reinterpret_cast<char*>(&id), sizeof(id));
        in.read(name, sizeof(name));
        in.read(reinterpret_cast<char*>(&score), sizeof(score));
    }

    void display() const {
        std::cout << "ID: " << id << ", Name: " << name
                  << ", Score: " << score << '\n';
    }
};

int main() {
    std::vector<Student> students = {
        {1001, "Alice Kim", 85.5f},
        {1002, "Bob Park", 92.0f},
        {1003, "Charlie Lee", 78.5f}
    };

    std::cout << "=== 직렬화 (Serialization) ===\n";
    std::cout << "학생 데이터 저장 중...\n";
    std::ofstream out("students.bin", std::ios::binary);
    if (!out) {
        std::cerr << "파일 열기 실패\n";
        return 1;
    }

    for (const auto& student : students) {
        student.serialize(out);
    }
    out.close();
    std::cout << "저장 완료: students.bin\n\n";

    std::cout << "=== 역직렬화 (Deserialization) ===\n";
    std::cout << "학생 데이터 로드 중...\n";
    std::ifstream in("students.bin", std::ios::binary);
    if (!in) {
        std::cerr << "파일 열기 실패\n";
        return 1;
    }

    std::vector<Student> loaded;
    Student s;
    while (in.read(reinterpret_cast<char*>(&s.id), sizeof(s.id))) {
        in.read(s.name, sizeof(s.name));
        in.read(reinterpret_cast<char*>(&s.score), sizeof(s.score));
        loaded.push_back(s);
    }
    in.close();

    std::cout << "로드된 데이터:\n";
    for (const auto& student : loaded) {
        student.display();
    }

    return 0;
}
