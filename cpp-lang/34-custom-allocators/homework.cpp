#include <iostream>
#include <vector>
#include <memory>

// Counting Allocator — 할당/해제 횟수 추적
template <typename T>
class CountingAllocator {
private:
    static size_t alloc_count;
    static size_t dealloc_count;
    static size_t total_allocated;

public:
    using value_type = T;

    CountingAllocator() = default;

    template <typename U>
    CountingAllocator(const CountingAllocator<U>&) {}

    T* allocate(size_t n) {
        alloc_count++;
        total_allocated += n * sizeof(T);
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    void deallocate(T* p, size_t n) noexcept {
        dealloc_count++;
        ::operator delete(p);
    }

    static void print_stats() {
        std::cout << "할당 횟수: " << alloc_count << '\n';
        std::cout << "해제 횟수: " << dealloc_count << '\n';
        std::cout << "총 할당 크기: " << total_allocated << " bytes\n";
    }

    static void reset() {
        alloc_count = 0;
        dealloc_count = 0;
        total_allocated = 0;
    }
};

template <typename T>
size_t CountingAllocator<T>::alloc_count = 0;

template <typename T>
size_t CountingAllocator<T>::dealloc_count = 0;

template <typename T>
size_t CountingAllocator<T>::total_allocated = 0;

// Tracking Allocator — 현재 활성 할당 추적
template <typename T>
class TrackingAllocator {
private:
    static size_t current_allocated;
    static size_t peak_allocated;

public:
    using value_type = T;

    TrackingAllocator() = default;

    template <typename U>
    TrackingAllocator(const TrackingAllocator<U>&) {}

    T* allocate(size_t n) {
        size_t bytes = n * sizeof(T);
        current_allocated += bytes;
        if (current_allocated > peak_allocated) {
            peak_allocated = current_allocated;
        }
        return static_cast<T*>(::operator new(bytes));
    }

    void deallocate(T* p, size_t n) noexcept {
        current_allocated -= n * sizeof(T);
        ::operator delete(p);
    }

    static void print_stats() {
        std::cout << "현재 할당: " << current_allocated << " bytes\n";
        std::cout << "피크 할당: " << peak_allocated << " bytes\n";
    }

    static void reset() {
        current_allocated = 0;
        peak_allocated = 0;
    }
};

template <typename T>
size_t TrackingAllocator<T>::current_allocated = 0;

template <typename T>
size_t TrackingAllocator<T>::peak_allocated = 0;

int main() {
    std::cout << "=== Counting Allocator 테스트 ===\n";

    CountingAllocator<int>::reset();
    {
        std::vector<int, CountingAllocator<int>> vec;
        for (int i = 0; i < 10; i++) {
            vec.push_back(i);
        }
        std::cout << "벡터 크기: " << vec.size() << '\n';
    }
    CountingAllocator<int>::print_stats();

    std::cout << "\n=== Tracking Allocator 테스트 ===\n";

    TrackingAllocator<double>::reset();
    {
        std::vector<double, TrackingAllocator<double>> vec1;
        std::vector<double, TrackingAllocator<double>> vec2;

        for (int i = 0; i < 5; i++) {
            vec1.push_back(3.14 * i);
            vec2.push_back(2.71 * i);
        }

        std::cout << "vec1 크기: " << vec1.size() << '\n';
        std::cout << "vec2 크기: " << vec2.size() << '\n';
        TrackingAllocator<double>::print_stats();
    }

    std::cout << "해제 후:\n";
    TrackingAllocator<double>::print_stats();

    return 0;
}
