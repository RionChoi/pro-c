#include <iostream>
#include <vector>
#include <memory_resource>
#include <string>
#include <chrono>

// 커스텀 메모리 리소스: 할당/해제 추적
class TrackingMemoryResource : public std::pmr::memory_resource {
private:
    std::pmr::memory_resource *upstream;
    std::size_t total_allocated = 0;
    std::size_t allocation_count = 0;
    std::size_t deallocation_count = 0;

    void *do_allocate(std::size_t bytes, std::size_t alignment) override {
        void *ptr = upstream->allocate(bytes, alignment);
        total_allocated += bytes;
        allocation_count++;
        std::cout << "📦 할당: " << bytes << " 바이트 (총: " << total_allocated << ")\n";
        return ptr;
    }

    void do_deallocate(void *p, std::size_t bytes, std::size_t alignment) override {
        upstream->deallocate(p, bytes, alignment);
        deallocation_count++;
        std::cout << "🗑️  해제: " << bytes << " 바이트\n";
    }

    bool do_is_equal(const memory_resource &other) const noexcept override {
        return this == &other;
    }

public:
    TrackingMemoryResource()
        : upstream(std::pmr::new_delete_resource()) {}

    void print_stats() const {
        std::cout << "\n📊 메모리 통계:\n";
        std::cout << "  할당 횟수: " << allocation_count << "\n";
        std::cout << "  해제 횟수: " << deallocation_count << "\n";
        std::cout << "  총 할당: " << total_allocated << " 바이트\n";
    }
};

int main() {
    std::cout << "=== 커스텀 할당자 & std::pmr ===\n\n";

    // 1. 기본 할당자 (new_delete_resource)
    std::cout << "1️⃣  기본 할당자 (new_delete_resource):\n";
    auto *resource = std::pmr::new_delete_resource();
    std::pmr::vector<int> v1(resource);
    v1.push_back(10);
    v1.push_back(20);
    v1.push_back(30);
    std::cout << "벡터: [";
    for (int x : v1) std::cout << x << " ";
    std::cout << "]\n\n";

    // 2. Monotonic Buffer Resource (고정 버퍼, 빠른 선형 할당)
    std::cout << "2️⃣  Monotonic Buffer Resource:\n";
    std::byte buffer[256];
    std::pmr::monotonic_buffer_resource mono_res(buffer, sizeof(buffer));
    std::pmr::vector<int> v2(&mono_res);
    for (int i = 0; i < 5; ++i) {
        v2.push_back(i * 10);
    }
    std::cout << "Monotonic 벡터: [";
    for (int x : v2) std::cout << x << " ";
    std::cout << "]\n\n";

    // 3. 커스텀 추적 리소스
    std::cout << "3️⃣  커스텀 추적 리소스:\n";
    TrackingMemoryResource tracker;
    std::pmr::vector<std::string> v3(&tracker);
    v3.push_back("C++");
    v3.push_back("메모리");
    v3.push_back("관리");
    std::cout << "추적 벡터: [";
    for (const auto &s : v3) std::cout << s << " ";
    std::cout << "]\n";
    tracker.print_stats();

    // 4. std::pmr::string 사용
    std::cout << "\n4️⃣  std::pmr::string:\n";
    std::pmr::string str(&tracker);
    str = "Polymorphic Memory Resource";
    std::cout << "PMR 문자열: " << str << "\n";
    tracker.print_stats();

    return 0;
}
