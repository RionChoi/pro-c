#include <iostream>
#include <vector>
#include <memory_resource>

int main() {
    std::cout << "=== std::pmr 메모리 리소스 데모 ===\n\n";

    std::cout << "--- new_delete_resource (기본 할당자) ---\n";
    {
        auto* resource = std::pmr::new_delete_resource();
        std::pmr::polymorphic_allocator<int> alloc(resource);

        std::pmr::vector<int> vec(alloc);
        vec.push_back(100);
        vec.push_back(200);
        vec.push_back(300);

        std::cout << "크기: " << vec.size() << ", 용량: " << vec.capacity() << "\n";
    }

    std::cout << "\n--- monotonic_buffer_resource (스택 메모리 기반) ---\n";
    {
        unsigned char buffer[1024];
        std::pmr::monotonic_buffer_resource resource(buffer, sizeof(buffer));
        std::pmr::polymorphic_allocator<int> alloc(&resource);

        std::pmr::vector<int> vec1(alloc);
        std::pmr::vector<int> vec2(alloc);
        std::pmr::vector<int> vec3(alloc);

        vec1.push_back(1);
        vec1.push_back(2);
        vec1.push_back(3);

        vec2.push_back(10);
        vec2.push_back(20);

        vec3.push_back(100);

        std::cout << "vec1: ";
        for (int x : vec1) std::cout << x << " ";
        std::cout << "\n";

        std::cout << "vec2: ";
        for (int x : vec2) std::cout << x << " ";
        std::cout << "\n";

        std::cout << "vec3: ";
        for (int x : vec3) std::cout << x << " ";
        std::cout << "\n";

        std::cout << "모든 벡터가 같은 버퍼에서 메모리 할당됨 (비용 절감)\n";
    }

    std::cout << "\n--- unsynchronized_pool_resource ---\n";
    {
        std::pmr::unsynchronized_pool_resource resource;
        std::pmr::polymorphic_allocator<double> alloc(&resource);

        std::pmr::vector<double> values(alloc);
        for (int i = 1; i <= 5; ++i) {
            values.push_back(i * 1.5);
        }

        std::cout << "값: ";
        for (double v : values) std::cout << v << " ";
        std::cout << "\n";
        std::cout << "Pool resource로 다양한 크기 메모리 효율적 관리\n";
    }

    return 0;
}
