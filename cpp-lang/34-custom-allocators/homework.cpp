#include <iostream>
#include <vector>
#include <memory>

template <typename T>
class CountingAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <typename U>
    struct rebind {
        using other = CountingAllocator<U>;
    };

    static size_type allocation_count;
    static size_type deallocation_count;
    static size_type bytes_allocated;

    CountingAllocator() = default;

    template <typename U>
    CountingAllocator(const CountingAllocator<U>&) {}

    pointer allocate(size_type n) {
        ++allocation_count;
        bytes_allocated += n * sizeof(T);
        std::cout << "[할당] " << n << " 개 (" << (n * sizeof(T))
                  << " 바이트), 총 할당: " << allocation_count << "회\n";
        return new T[n];
    }

    void deallocate(pointer p, size_type n) {
        ++deallocation_count;
        std::cout << "[해제] " << n << " 개 (" << (n * sizeof(T))
                  << " 바이트), 총 해제: " << deallocation_count << "회\n";
        delete[] p;
    }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        new (p) U(std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(U* p) {
        p->~U();
    }

    bool operator==(const CountingAllocator&) const { return true; }
    bool operator!=(const CountingAllocator&) const { return false; }
};

template <typename T>
typename CountingAllocator<T>::size_type CountingAllocator<T>::allocation_count = 0;

template <typename T>
typename CountingAllocator<T>::size_type CountingAllocator<T>::deallocation_count = 0;

template <typename T>
typename CountingAllocator<T>::size_type CountingAllocator<T>::bytes_allocated = 0;

int main() {
    std::cout << "=== 커스텀 할당자 (CountingAllocator) ===\n\n";

    {
        std::cout << "--- 벡터 생성 및 크기 조정 ---\n";
        std::vector<int, CountingAllocator<int>> vec;
        vec.push_back(10);
        vec.push_back(20);
        vec.push_back(30);
        std::cout << "벡터 크기: " << vec.size() << "\n";
        std::cout << "벡터 용량: " << vec.capacity() << "\n\n";

        std::cout << "--- 추가 삽입 ---\n";
        for (int i = 0; i < 5; ++i) {
            vec.push_back(40 + i * 10);
        }
        std::cout << "벡터 크기: " << vec.size() << "\n";
        std::cout << "벡터 용량: " << vec.capacity() << "\n\n";

        std::cout << "--- 벡터 내용 ---\n";
        for (int v : vec) {
            std::cout << v << " ";
        }
        std::cout << "\n\n";
    }

    std::cout << "=== 할당자 통계 ===\n";
    std::cout << "총 할당 횟수: " << CountingAllocator<int>::allocation_count << "\n";
    std::cout << "총 해제 횟수: " << CountingAllocator<int>::deallocation_count << "\n";
    std::cout << "총 할당된 바이트: " << CountingAllocator<int>::bytes_allocated << "\n";

    return 0;
}
