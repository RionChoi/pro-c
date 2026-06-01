#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdint>

struct Student {
    uint16_t id;
    char name[64];
    double gpa;
    uint32_t year;
};

void save_students(const std::string& filename, const std::vector<Student>& students) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        std::cerr << "파일을 열 수 없습니다: " << filename << '\n';
        return;
    }

    uint32_t count = students.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& s : students) {
        out.write(reinterpret_cast<const char*>(&s.id), sizeof(s.id));
        out.write(s.name, sizeof(s.name));
        out.write(reinterpret_cast<const char*>(&s.gpa), sizeof(s.gpa));
        out.write(reinterpret_cast<const char*>(&s.year), sizeof(s.year));
    }

    out.close();
    std::cout << "저장 완료: " << students.size() << " 명의 학생 데이터\n";
}

std::vector<Student> load_students(const std::string& filename) {
    std::vector<Student> students;
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        std::cerr << "파일을 열 수 없습니다: " << filename << '\n';
        return students;
    }

    uint32_t count = 0;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (uint32_t i = 0; i < count; ++i) {
        Student s;
        in.read(reinterpret_cast<char*>(&s.id), sizeof(s.id));
        in.read(s.name, sizeof(s.name));
        in.read(reinterpret_cast<char*>(&s.gpa), sizeof(s.gpa));
        in.read(reinterpret_cast<char*>(&s.year), sizeof(s.year));

        if (in.gcount() > 0) {
            students.push_back(s);
        }
    }

    in.close();
    std::cout << "로드 완료: " << students.size() << " 명의 학생 데이터\n";
    return students;
}

void display_students(const std::vector<Student>& students) {
    std::cout << "\n=== 학생 목록 ===\n";
    for (const auto& s : students) {
        std::cout << "ID: " << s.id << " | 이름: " << s.name
                  << " | GPA: " << s.gpa << " | 학년: " << s.year << '\n';
    }
}

int main() {
    std::vector<Student> students = {
        {1001, "Alice Johnson", 3.95, 4},
        {1002, "Bob Smith", 3.75, 3},
        {1003, "Charlie Brown", 3.85, 2}
    };

    save_students("students.bin", students);

    auto loaded = load_students("students.bin");
    display_students(loaded);

    return 0;
}
