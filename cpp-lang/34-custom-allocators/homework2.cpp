#include <iostream>
#include <memory_resource>
#include <vector>
#include <string>

int main() {
    std::cout << "=== std::pmr 메모리 풀 ===\n\n";

    // 1. monotonic_buffer_resource: 선형 버퍼, 해제 불가 (재할당만 가능)
    {
        std::cout << "--- 1. monotonic_buffer_resource ---\n";
        char buffer[1024];  // 1KB 스택 버퍼
        std::pmr::monotonic_buffer_resource mono(buffer, sizeof(buffer));
        std::pmr::polymorphic_allocator<int> alloc(&mono);

        std::pmr::vector<int> vec(alloc);
        std::cout << "빈 벡터 생성 완료\n";

        for (int i = 0; i < 100; ++i) {
            vec.push_back(i * 10);
        }
        std::cout << "100개의 정수 추가: " << vec.size() << "개\n";
        std::cout << "벡터 용량: " << vec.capacity() << "\n";
        std::cout << "처음 5개: ";
        for (int i = 0; i < 5; ++i) {
            std::cout << vec[i] << " ";
        }
        std::cout << "\n\n";
    }

    // 2. unsynchronized_pool_resource: 스레드 안전 안 함, 빠름
    {
        std::cout << "--- 2. unsynchronized_pool_resource ---\n";
        std::pmr::unsynchronized_pool_resource pool;
        std::pmr::polymorphic_allocator<std::pmr::string> alloc(&pool);

        std::pmr::vector<std::pmr::string> strings(alloc);
        std::cout << "문자열 벡터 생성\n";

        const char* names[] = {"Alice", "Bob", "Charlie", "Diana", "Eve"};
        for (const auto* name : names) {
            strings.push_back(std::pmr::string(name, alloc));
        }
        std::cout << "5개의 문자열 추가\n";
        std::cout << "저장된 문자열:\n";
        for (const auto& s : strings) {
            std::cout << "  - " << s << "\n";
        }
        std::cout << "\n";
    }

    // 3. synchronized_pool_resource: 스레드 안전 (뮤텍스 사용)
    {
        std::cout << "--- 3. synchronized_pool_resource ---\n";
        std::pmr::synchronized_pool_resource pool;
        std::pmr::polymorphic_allocator<double> alloc(&pool);

        std::pmr::vector<double> doubles(alloc);
        std::cout << "실수 벡터 생성\n";

        for (int i = 0; i < 50; ++i) {
            doubles.push_back(i * 3.14);
        }
        std::cout << "50개의 실수 추가\n";
        std::cout << "벡터 크기: " << doubles.size() << "\n";
        std::cout << "첫 번째 값: " << doubles[0] << "\n";
        std::cout << "마지막 값: " << doubles.back() << "\n\n";
    }

    // 4. new_delete_resource: 기본 할당자 (new/delete)
    {
        std::cout << "--- 4. new_delete_resource (기본) ---\n";
        auto* default_res = std::pmr::new_delete_resource();
        std::pmr::polymorphic_allocator<int> alloc(default_res);

        std::pmr::vector<int> vec(alloc);
        for (int i = 0; i < 10; ++i) {
            vec.push_back(i);
        }
        std::cout << "10개의 정수 할당 완료\n";
        std::cout << "저장된 값: ";
        for (int v : vec) {
            std::cout << v << " ";
        }
        std::cout << "\n\n";
    }

    std::cout << "=== 메모리 풀 테스트 완료 ===\n";
    return 0;
}
