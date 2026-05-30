#include <iostream>
#include <memory_resource>
#include <vector>
#include <string>

class Student {
private:
    std::string name;
    int student_id;
    double gpa;

public:
    Student(const std::string& n, int id, double g)
        : name(n), student_id(id), gpa(g) {}

    void display() const {
        std::cout << "ID: " << student_id << ", 이름: " << name
                  << ", GPA: " << gpa << "\n";
    }
};

int main() {
    constexpr std::size_t BUFFER_SIZE = 4096;
    std::byte buffer[BUFFER_SIZE];

    std::pmr::monotonic_buffer_resource mbr(buffer, BUFFER_SIZE);
    std::pmr::polymorphic_allocator<Student> alloc(&mbr);

    std::pmr::vector<Student> students(alloc);

    std::cout << "=== Monotonic Buffer Resource 사용 ===\n";
    std::cout << "학생 추가\n";

    students.emplace_back("김철수", 001, 3.85);
    students.emplace_back("이영희", 002, 4.00);
    students.emplace_back("박민준", 003, 3.45);

    std::cout << "\n학생 목록:\n";
    for (const auto& s : students) {
        s.display();
    }

    std::cout << "\nBuffer 내 사용된 바이트: " << mbr.upstream_resource()->is_equal(*mbr.upstream_resource()) << "\n";
    std::cout << "모든 할당이 자동으로 해제됩니다 (블록 종료 시)\n";

    return 0;
}
