#include <iostream>
#include <vector>
#include <memory>
#include <cstring>

template <typename T>
class CountingAllocator {
private:
    static std::size_t allocations;
    static std::size_t deallocations;
    static std::size_t total_allocated;

public:
    using value_type = T;

    CountingAllocator() = default;

    template <typename U>
    CountingAllocator(const CountingAllocator<U>&) {}

    T* allocate(std::size_t n) {
        T* ptr = new T[n];
        allocations++;
        total_allocated += n * sizeof(T);
        std::cout << "할당: " << n << "개 (총 " << total_allocated << " 바이트)\n";
        return ptr;
    }

    void deallocate(T* p, std::size_t n) {
        delete[] p;
        deallocations++;
        std::cout << "해제: " << n << "개\n";
    }

    template <typename U>
    struct rebind {
        using other = CountingAllocator<U>;
    };

    static void print_stats() {
        std::cout << "\n=== 할당자 통계 ===\n";
        std::cout << "할당 횟수: " << allocations << "\n";
        std::cout << "해제 횟수: " << deallocations << "\n";
        std::cout << "총 할당량: " << total_allocated << " 바이트\n";
    }
};

template <typename T>
std::size_t CountingAllocator<T>::allocations = 0;

template <typename T>
std::size_t CountingAllocator<T>::deallocations = 0;

template <typename T>
std::size_t CountingAllocator<T>::total_allocated = 0;

template <typename T, typename U>
bool operator==(const CountingAllocator<T>&, const CountingAllocator<U>&) {
    return true;
}

template <typename T, typename U>
bool operator!=(const CountingAllocator<T>&, const CountingAllocator<U>&) {
    return false;
}

int main() {
    {
        std::vector<int, CountingAllocator<int>> vec;

        std::cout << "벡터에 10개 정수 추가\n";
        for (int i = 0; i < 10; i++) {
            vec.push_back(i);
        }

        std::cout << "\n벡터 내용: ";
        for (int v : vec) {
            std::cout << v << " ";
        }
        std::cout << "\n";
    }

    CountingAllocator<int>::print_stats();

    return 0;
}
