#include <iostream>
#include <fstream>
#include <cstring>

struct Student {
    int id;
    char name[64];
    double gpa;
    int year;
};

void save_student(const char* filename, const Student& student) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "파일 열기 실패: " << filename << '\n';
        return;
    }
    file.write(reinterpret_cast<const char*>(&student), sizeof(student));
    file.close();
    std::cout << "학생 정보 저장 완료: " << filename << '\n';
}

Student load_student(const char* filename) {
    Student student{};
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "파일 열기 실패: " << filename << '\n';
        return student;
    }
    file.read(reinterpret_cast<char*>(&student), sizeof(student));
    file.close();
    return student;
}

void print_student(const Student& student) {
    std::cout << "=== 학생 정보 ===\n"
              << "ID: " << student.id << '\n'
              << "이름: " << student.name << '\n'
              << "GPA: " << student.gpa << '\n'
              << "학년: " << student.year << '\n';
}

int main() {
    Student student1;
    student1.id = 2024001;
    strcpy(student1.name, "김철수");
    student1.gpa = 3.85;
    student1.year = 3;

    save_student("student.bin", student1);

    Student loaded = load_student("student.bin");
    print_student(loaded);

    return 0;
}
