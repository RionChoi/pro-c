#include <iostream>
#include <vector>
#include <memory>

int alloc_count = 0;
int dealloc_count = 0;

template <typename T>
class CountingAllocator {
public:
    using value_type = T;

    CountingAllocator() = default;

    template <typename U>
    CountingAllocator(const CountingAllocator<U>&) {}

    T* allocate(std::size_t n) {
        ++alloc_count;
        std::cout << "allocate: " << n << " 개 (총 할당: " << alloc_count << ")\n";
        return std::allocator<T>().allocate(n);
    }

    void deallocate(T* p, std::size_t n) {
        ++dealloc_count;
        std::cout << "deallocate: " << n << " 개 (총 해제: " << dealloc_count << ")\n";
        std::allocator<T>().deallocate(p, n);
    }

    template <typename U>
    bool operator==(const CountingAllocator<U>&) const {
        return true;
    }

    template <typename U>
    bool operator!=(const CountingAllocator<U>&) const {
        return false;
    }
};

int main() {
    std::cout << "=== 커스텀 할당자 데모 ===\n\n";

    {
        std::cout << "std::vector<int> 생성 및 추가:\n";
        std::vector<int, CountingAllocator<int>> vec;
        vec.push_back(10);
        vec.push_back(20);
        vec.push_back(30);
        std::cout << "\n크기: " << vec.size() << "\n";
        std::cout << "용량: " << vec.capacity() << "\n\n";

        std::cout << "벡터 소멸 및 메모리 해제:\n";
    }

    std::cout << "\n=== 최종 통계 ===\n";
    std::cout << "총 할당 횟수: " << alloc_count << "\n";
    std::cout << "총 해제 횟수: " << dealloc_count << "\n";

    return 0;
}
