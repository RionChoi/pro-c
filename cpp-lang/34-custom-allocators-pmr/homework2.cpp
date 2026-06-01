#include <iostream>
#include <memory_resource>
#include <vector>
#include <cstring>
#include <iomanip>

class CountingMemoryResource : public std::pmr::memory_resource {
private:
    std::pmr::memory_resource* upstream;
    size_t allocation_count = 0;
    size_t deallocation_count = 0;
    size_t total_bytes_allocated = 0;
    size_t current_bytes = 0;

    void* do_allocate(size_t bytes, size_t alignment) override {
        allocation_count++;
        total_bytes_allocated += bytes;
        current_bytes += bytes;
        void* ptr = upstream->allocate(bytes, alignment);

        std::cout << "[할당] 크기=" << bytes
                  << " bytes, 정렬=" << alignment
                  << ", 주소=" << ptr << "\n";

        return ptr;
    }

    void do_deallocate(void* ptr, size_t bytes, size_t alignment) override {
        deallocation_count++;
        current_bytes -= bytes;
        upstream->deallocate(ptr, bytes, alignment);
        std::cout << "[해제] 크기=" << bytes
                  << " bytes, 주소=" << ptr << "\n";
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

public:
    explicit CountingMemoryResource(std::pmr::memory_resource* up = std::pmr::new_delete_resource())
        : upstream(up) {}

    void print_stats() const {
        std::cout << "\n=== 메모리 통계 ===\n";
        std::cout << "할당 횟수:     " << allocation_count << "\n";
        std::cout << "해제 횟수:     " << deallocation_count << "\n";
        std::cout << "총 할당 바이트: " << total_bytes_allocated << "\n";
        std::cout << "현재 사용:     " << current_bytes << " bytes\n";
    }
};

int main() {
    std::cout << "=== 커스텀 메모리 리소스 ===\n\n";

    CountingMemoryResource cmr;

    {
        std::cout << "1. int 벡터 할당/해제:\n";
        std::pmr::vector<int> vec(&cmr);

        for (int i = 0; i < 5; ++i) {
            vec.push_back(i * 10);
        }
        std::cout << "벡터 크기: " << vec.size() << "\n";
    }

    cmr.print_stats();

    CountingMemoryResource cmr2;

    std::cout << "\n2. 문자열 벡터:\n";
    {
        std::pmr::vector<std::pmr::string> strings(&cmr2);

        strings.push_back(std::pmr::string("Alpha", &cmr2));
        strings.push_back(std::pmr::string("Beta", &cmr2));
        strings.push_back(std::pmr::string("Gamma", &cmr2));

        std::cout << "문자열 개수: " << strings.size() << "\n";
    }

    cmr2.print_stats();

    return 0;
}
