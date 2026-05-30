#include <iostream>
#include <memory_resource>
#include <vector>
#include <string>
#include <chrono>
#include <cstddef>
#include <array>

// std::pmr 데모: 다양한 메모리 할당 전략
int main() {
    std::cout << "=== std::pmr (Polymorphic Memory Resource) 데모 ===\n\n";

    // 1. 기본 할당자 (malloc/free 기반)
    {
        std::cout << "1. 기본 할당자 (new_delete_resource):\n";
        std::pmr::vector<int> vec(std::pmr::new_delete_resource());

        for (int i = 0; i < 5; ++i) {
            vec.push_back(i * 10);
        }

        std::cout << "  벡터: ";
        for (int v : vec) std::cout << v << " ";
        std::cout << "\n\n";
    }

    // 2. 단조 메모리 풀 (monotonic_buffer_resource)
    {
        std::cout << "2. 단조 메모리 풀 (monotonic_buffer_resource):\n";
        std::array<std::byte, 1024> buffer;
        std::pmr::monotonic_buffer_resource mono_res(buffer.data(), buffer.size());

        std::pmr::vector<std::string> strings(&mono_res);
        strings.push_back("Hello");
        strings.push_back("World");
        strings.push_back("Allocator");

        std::cout << "  문자열들: ";
        for (const auto& s : strings) {
            std::cout << s << " ";
        }
        std::cout << "\n  (주의: 풀 삭제 시 모든 객체 자동 삭제됨)\n\n";
    }

    // 3. 고정 크기 블록 풀
    {
        std::cout << "3. 고정 크기 블록 풀 (unsynchronized_pool_resource):\n";
        std::pmr::unsynchronized_pool_resource pool;

        std::pmr::vector<int> vec1(&pool);
        std::pmr::vector<double> vec2(&pool);

        auto start = std::chrono::high_resolution_clock::now();

        // 풀 할당: 더 빠름 (재사용 가능한 블록)
        for (int i = 0; i < 1000; ++i) {
            vec1.push_back(i);
        }

        auto mid = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < 1000; ++i) {
            vec2.push_back(i * 3.14);
        }

        auto end = std::chrono::high_resolution_clock::now();

        auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(mid - start);
        auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end - mid);

        std::cout << "  vec1 1000개 추가: " << duration1.count() << "μs\n";
        std::cout << "  vec2 1000개 추가: " << duration2.count() << "μs\n";
        std::cout << "  (풀 메모리 재사용으로 성능 개선)\n\n";
    }

    // 4. 커스텀 메모리 리소스 (간단한 예제)
    {
        std::cout << "4. 커스텀 메모리 리소스:\n";

        class CountingResource : public std::pmr::memory_resource {
            int alloc_count = 0;

            void* do_allocate(std::size_t bytes, std::size_t alignment) override {
                alloc_count++;
                return std::pmr::new_delete_resource()->allocate(bytes, alignment);
            }

            void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) override {
                std::pmr::new_delete_resource()->deallocate(p, bytes, alignment);
            }

            bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
                return this == &other;
            }

        public:
            int get_alloc_count() const { return alloc_count; }
        };

        CountingResource counting;
        std::pmr::vector<std::string> vec(&counting);

        vec.push_back("First");
        vec.push_back("Second");
        vec.push_back("Third");

        std::cout << "  할당 횟수: " << counting.get_alloc_count() << "\n";
        std::cout << "  벡터 요소: ";
        for (const auto& s : vec) {
            std::cout << s << " ";
        }
        std::cout << "\n\n";
    }

    // 5. PMR 컨테이너의 리소스 선택
    {
        std::cout << "5. PMR 컨테이너의 유연성:\n";

        // 다양한 리소스로 같은 타입의 컨테이너 생성
        std::pmr::vector<int> vec1(std::pmr::new_delete_resource());
        std::pmr::vector<int> vec2(std::pmr::null_memory_resource());

        vec1.push_back(42);
        std::cout << "  vec1 (new_delete): " << vec1[0] << "\n";

        // vec2는 null_memory_resource이므로 할당 시도 시 std::bad_alloc 발생
        try {
            vec2.push_back(99);
        } catch (const std::bad_alloc&) {
            std::cout << "  vec2 (null): bad_alloc 예외 발생 (예상된 동작)\n";
        }
    }

    return 0;
}
