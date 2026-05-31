#include <iostream>
#include <memory_resource>
#include <vector>
#include <string>
#include <cstring>

struct MemoryStats {
    std::size_t allocated = 0;
    std::size_t deallocated = 0;
    std::size_t allocationCount = 0;

    std::size_t currentUsage() const {
        return allocated - deallocated;
    }

    void print() const {
        std::cout << "  할당: " << allocated << " bytes\n";
        std::cout << "  해제: " << deallocated << " bytes\n";
        std::cout << "  현재 사용: " << currentUsage() << " bytes\n";
        std::cout << "  할당 횟수: " << allocationCount << "\n";
    }
};

class StatsResource : public std::pmr::memory_resource {
    std::pmr::memory_resource* upstream;
    MemoryStats& stats;

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        void* ptr = upstream->allocate(bytes, alignment);
        stats.allocated += bytes;
        stats.allocationCount++;
        return ptr;
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override {
        upstream->deallocate(p, bytes, alignment);
        stats.deallocated += bytes;
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

public:
    StatsResource(MemoryStats& s)
        : upstream(std::pmr::get_default_resource()), stats(s) {}
};

struct Person {
    int id;
    std::pmr::string name;
    std::pmr::string email;

    Person(int i, std::pmr::memory_resource* mr)
        : id(i), name(mr), email(mr) {}
};

int main() {
    std::cout << "=== std::pmr 다형적 메모리 자원 ===\n\n";

    // 버퍼 리소스 테스트
    std::cout << "[1] monotonic_buffer_resource 테스트\n";
    char buffer[1024];
    std::pmr::monotonic_buffer_resource bufferRes(buffer, sizeof(buffer));

    std::pmr::vector<int> vec1(&bufferRes);
    for (int i = 0; i < 20; ++i) {
        vec1.push_back(i);
    }
    std::cout << "벡터 크기: " << vec1.size() << "\n";
    std::cout << "벡터 용량: " << vec1.capacity() << "\n\n";

    // 통계 리소스 테스트
    std::cout << "[2] 커스텀 StatsResource 테스트\n";
    MemoryStats stats;
    StatsResource statsRes(stats);

    {
        std::pmr::vector<std::pmr::string> names(&statsRes);
        std::cout << "이름 벡터 생성...\n";
        names.push_back(std::pmr::string("Alice", &statsRes));
        names.push_back(std::pmr::string("Bob", &statsRes));
        names.push_back(std::pmr::string("Charlie", &statsRes));

        std::cout << "이름 개수: " << names.size() << "\n";
        stats.print();

        std::cout << "\n이름 목록:\n";
        for (const auto& name : names) {
            std::cout << "  - " << name << "\n";
        }
    }

    std::cout << "\n[3] Person 벡터\n";
    MemoryStats personStats;
    StatsResource personRes(personStats);

    {
        std::pmr::vector<Person> people(&personRes);
        people.emplace_back(1, &personRes);
        people[0].name = "정준영";
        people[0].email = "jungyoung@example.com";

        people.emplace_back(2, &personRes);
        people[1].name = "이지은";
        people[1].email = "jieun@example.com";

        std::cout << "사람 수: " << people.size() << "\n";
        personStats.print();

        std::cout << "\n사람 정보:\n";
        for (const auto& p : people) {
            std::cout << "  ID: " << p.id << ", 이름: " << p.name
                      << ", 이메일: " << p.email << "\n";
        }
    }

    std::cout << "\nstd::pmr 테스트 완료!\n";
    return 0;
}
