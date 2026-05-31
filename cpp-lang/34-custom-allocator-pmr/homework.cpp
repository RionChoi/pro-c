// homework.cpp — 커스텀 할당자 TrackingAllocator
// 할당자 Named Requirements를 만족하는 TrackingAllocator<T> 를 작성하고
// STL 컨테이너(vector, list)에 적용하여 내부 할당 패턴을 관찰한다.

#include <iostream>
#include <vector>
#include <list>
#include <numeric>
#include <cstddef>
#include <new>

// 모든 TrackingAllocator 인스턴스가 공유하는 통계 구조체
struct AllocStats {
    std::size_t alloc_count   = 0;
    std::size_t dealloc_count = 0;
    std::size_t live_bytes    = 0;
    std::size_t peak_bytes    = 0;

    void on_alloc(std::size_t bytes) noexcept {
        ++alloc_count;
        live_bytes += bytes;
        if (live_bytes > peak_bytes)
            peak_bytes = live_bytes;
    }

    void on_dealloc(std::size_t bytes) noexcept {
        ++dealloc_count;
        live_bytes -= bytes;
    }

    void print(const char* title) const {
        std::cout << title << "\n"
                  << "  할당 횟수:  " << alloc_count   << "\n"
                  << "  해제 횟수:  " << dealloc_count << "\n"
                  << "  현재 바이트: " << live_bytes   << "\n"
                  << "  최대 바이트: " << peak_bytes   << "\n";
    }

    void reset() noexcept {
        alloc_count   = 0;
        dealloc_count = 0;
        live_bytes    = 0;
        peak_bytes    = 0;
    }
};

// 전역 통계 (모든 TrackingAllocator<T> 인스턴스가 공유)
static AllocStats g_stats;

// ---- TrackingAllocator : C++ Allocator Named Requirements 충족 ----

template <typename T>
struct TrackingAllocator {
    using value_type = T;

    TrackingAllocator() noexcept = default;

    // rebind 생성자: 컨테이너가 내부 노드 타입용 할당자로 변환할 때 사용
    template <typename U>
    TrackingAllocator(const TrackingAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        const std::size_t bytes = n * sizeof(T);
        g_stats.on_alloc(bytes);
        std::cout << "    [+] allocate   n=" << n
                  << " sizeof(T)=" << sizeof(T)
                  << " => " << bytes << " 바이트\n";
        return static_cast<T*>(::operator new(bytes));
    }

    void deallocate(T* ptr, std::size_t n) noexcept {
        const std::size_t bytes = n * sizeof(T);
        g_stats.on_dealloc(bytes);
        std::cout << "    [-] deallocate n=" << n
                  << " sizeof(T)=" << sizeof(T)
                  << " => " << bytes << " 바이트\n";
        ::operator delete(ptr);
    }
};

// 같은 전역 힙을 사용하므로 모든 인스턴스가 동등
template <typename T, typename U>
bool operator==(const TrackingAllocator<T>&,
                const TrackingAllocator<U>&) noexcept { return true; }

// ---- 데모 ----------------------------------------------------------------

int main() {
    std::cout << "==============================================\n";
    std::cout << " 커스텀 할당자 — TrackingAllocator 실습\n";
    std::cout << "==============================================\n\n";

    // 1. std::vector: push_back 시 용량 초과마다 재할당 발생
    std::cout << "1. vector<int> — reserve 없이 push_back × 8\n";
    std::cout << "   (기하급수적 재할당 관찰)\n";
    {
        std::vector<int, TrackingAllocator<int>> v;
        for (int i = 1; i <= 8; ++i)
            v.push_back(i);
        const int sum = std::accumulate(v.begin(), v.end(), 0);
        std::cout << "   결과: 합계=" << sum
                  << "  capacity=" << v.capacity() << "\n";
    }
    std::cout << "   [vector 소멸 후]\n";
    g_stats.print("   ▶ vector 통계:");
    g_stats.reset();

    std::cout << "\n2. vector<int> — reserve(8) 후 push_back × 8\n";
    std::cout << "   (재할당 없이 한 번만 할당)\n";
    {
        std::vector<int, TrackingAllocator<int>> v;
        v.reserve(8);
        for (int i = 1; i <= 8; ++i)
            v.push_back(i);
        const int sum = std::accumulate(v.begin(), v.end(), 0);
        std::cout << "   결과: 합계=" << sum
                  << "  capacity=" << v.capacity() << "\n";
    }
    g_stats.print("   ▶ reserve(8) vector 통계:");
    g_stats.reset();

    // 2. std::list: 각 노드마다 개별 allocate 호출
    std::cout << "\n3. list<int> — push_back × 5\n";
    std::cout << "   (노드마다 allocate — 내부 node 타입으로 rebind 확인)\n";
    {
        std::list<int, TrackingAllocator<int>> lst;
        for (int x : {100, 200, 300, 400, 500})
            lst.push_back(x);
        std::cout << "   내용:";
        for (int x : lst) std::cout << " " << x;
        std::cout << "\n";
    }
    g_stats.print("   ▶ list 통계:");

    return 0;
}
