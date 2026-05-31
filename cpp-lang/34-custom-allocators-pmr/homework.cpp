#include <iostream>
#include <vector>
#include <list>
#include <memory_resource>
#include <cstring>

struct Student {
    int id;
    std::pmr::string name;
    double gpa;

    Student(std::pmr::memory_resource* mr)
        : id(0), gpa(0.0), name(mr) {}
};

class SimplePoolAllocator : public std::pmr::memory_resource {
    std::pmr::memory_resource* upstream;
    std::size_t allocCount;

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        allocCount++;
        return upstream->allocate(bytes, alignment);
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override {
        upstream->deallocate(p, bytes, alignment);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

public:
    SimplePoolAllocator()
        : upstream(std::pmr::get_default_resource()), allocCount(0) {}

    std::size_t getAllocationCount() const {
        return allocCount;
    }
};

int main() {
    std::cout << "=== 메모리 자원과 할당자 ===\n\n";

    SimplePoolAllocator allocator;

    std::cout << "[1] 정수 벡터\n";
    std::pmr::vector<int> intVec(&allocator);
    for (int i = 0; i < 10; ++i) {
        intVec.push_back(i * 100);
    }
    std::cout << "크기: " << intVec.size() << "\n";
    std::cout << "할당 횟수: " << allocator.getAllocationCount() << "\n";
    std::cout << "값: ";
    for (int v : intVec) {
        std::cout << v << " ";
    }
    std::cout << "\n\n";

    SimplePoolAllocator studentAlloc;
    std::cout << "[2] 학생 객체 리스트\n";
    std::pmr::list<Student> students(&studentAlloc);

    for (int i = 0; i < 5; ++i) {
        students.emplace_back(&studentAlloc);
        Student& s = students.back();
        s.id = 1001 + i;
        s.name = "Student";
        s.name += char('0' + i + 1);
        s.gpa = 3.5 + (i * 0.1);
    }

    std::cout << "학생 수: " << students.size() << "\n";
    std::cout << "할당 횟수: " << studentAlloc.getAllocationCount() << "\n";
    std::cout << "학생 정보:\n";
    for (const auto& s : students) {
        std::cout << "  ID: " << s.id << ", 이름: " << s.name
                  << ", GPA: " << s.gpa << "\n";
    }

    std::cout << "\n[3] 문자열 벡터\n";
    SimplePoolAllocator stringAlloc;
    std::pmr::vector<std::pmr::string> strings(&stringAlloc);
    strings.push_back(std::pmr::string("Alice", &stringAlloc));
    strings.push_back(std::pmr::string("Bob", &stringAlloc));
    strings.push_back(std::pmr::string("Charlie", &stringAlloc));

    std::cout << "문자열 개수: " << strings.size() << "\n";
    std::cout << "할당 횟수: " << stringAlloc.getAllocationCount() << "\n";
    std::cout << "문자열 목록:\n";
    for (const auto& str : strings) {
        std::cout << "  - " << str << "\n";
    }

    std::cout << "\n메모리 자원 테스트 완료!\n";
    return 0;
}
