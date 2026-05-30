#include <iostream>
#include <vector>
#include <map>
#include <memory>

// 간단한 추적 할당자
template <typename T>
class TrackingAllocator {
public:
    using value_type = T;

    template <typename U>
    struct rebind {
        using other = TrackingAllocator<U>;
    };

    TrackingAllocator() : alloc_count(0), dealloc_count(0) {}

    template <typename U>
    TrackingAllocator(const TrackingAllocator<U>& other)
        : alloc_count(0), dealloc_count(0) {}

    T* allocate(std::size_t n) {
        alloc_count += n;
        return static_cast<T*>(operator new(n * sizeof(T)));
    }

    void deallocate(T* p, std::size_t n) {
        dealloc_count += n;
        operator delete(p);
    }

    bool operator==(const TrackingAllocator& other) const { return true; }
    bool operator!=(const TrackingAllocator& other) const { return false; }

    std::size_t get_alloc_count() const { return alloc_count; }
    std::size_t get_dealloc_count() const { return dealloc_count; }

private:
    std::size_t alloc_count;
    std::size_t dealloc_count;
};

// 정렬 할당자 (특정 바이트에 정렬된 메모리 할당)
template <typename T, std::size_t Alignment = alignof(T)>
class AlignedAllocator {
public:
    using value_type = T;

    template <typename U>
    struct rebind {
        using other = AlignedAllocator<U, Alignment>;
    };

    T* allocate(std::size_t n) {
        void* ptr = nullptr;
        if (posix_memalign(&ptr, Alignment, n * sizeof(T)) != 0) {
            throw std::bad_alloc();
        }
        return static_cast<T*>(ptr);
    }

    void deallocate(T* p, std::size_t) {
        free(p);
    }

    bool operator==(const AlignedAllocator& other) const { return true; }
    bool operator!=(const AlignedAllocator& other) const { return false; }
};

int main() {
    std::cout << "=== 커스텀 할당자 데모 ===\n\n";

    // 1. TrackingAllocator 사용
    {
        std::cout << "1. TrackingAllocator 사용:\n";
        TrackingAllocator<int> alloc;
        std::vector<int, TrackingAllocator<int>> vec(alloc);

        vec.push_back(10);
        vec.push_back(20);
        vec.push_back(30);

        std::cout << "  벡터 크기: " << vec.size() << "\n";
        std::cout << "  할당된 요소: " << alloc.get_alloc_count() << "\n";
        std::cout << "  해제된 요소: " << alloc.get_dealloc_count() << "\n\n";
    }

    // 2. 정렬 할당자로 벡터 생성
    {
        std::cout << "2. 64바이트 정렬된 메모리:\n";
        using AlignedVec = std::vector<double, AlignedAllocator<double, 64>>;
        AlignedVec vec;

        vec.push_back(3.14);
        vec.push_back(2.71);

        std::cout << "  주소: " << (void*)vec.data() << "\n";
        std::cout << "  64의 배수 정렬: "
                  << ((uintptr_t)vec.data() % 64 == 0 ? "예" : "아니오") << "\n\n";
    }

    // 3. Map과 커스텀 할당자
    {
        std::cout << "3. Map과 TrackingAllocator:\n";
        std::map<std::string, int, std::less<std::string>,
                 TrackingAllocator<std::pair<const std::string, int>>> map;

        map["Alice"] = 85;
        map["Bob"] = 92;
        map["Charlie"] = 78;

        std::cout << "  맵 크기: " << map.size() << "\n";
        for (const auto& [name, score] : map) {
            std::cout << "    " << name << ": " << score << "\n";
        }
    }

    return 0;
}
