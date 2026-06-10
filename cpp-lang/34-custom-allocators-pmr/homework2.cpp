// homework2.cpp — std::pmr 폴리모픽 메모리 리소스 실습
// monotonic_buffer_resource, unsynchronized_pool_resource 를 활용하고
// 커스텀 memory_resource(LoggingResource)를 구현하여 업스트림 체이닝을 시연한다.

#include <iostream>
#include <memory_resource>
#include <vector>
#include <list>
#include <algorithm>
#include <numeric>
#include <array>
#include <cstddef>

// ---- 커스텀 메모리 리소스: 할당/해제를 기록하는 래퍼 --------------------

class LoggingResource : public std::pmr::memory_resource {
public:
    explicit LoggingResource(
        const char*                name,
        std::pmr::memory_resource* upstream = std::pmr::get_default_resource())
        : name_(name), upstream_(upstream) {}

    void print_stats() const {
        std::cout << "  [" << name_ << "] 할당 횟수=" << alloc_count_
                  << "  총 할당=" << total_bytes_ << " 바이트\n";
    }

private:
    void* do_allocate(std::size_t bytes, std::size_t align) override {
        ++alloc_count_;
        total_bytes_ += bytes;
        std::cout << "  [" << name_ << " +] " << bytes
                  << " 바이트 (정렬=" << align << ")\n";
        return upstream_->allocate(bytes, align);
    }

    void do_deallocate(void* p, std::size_t bytes, std::size_t align) override {
        std::cout << "  [" << name_ << " -] " << bytes << " 바이트\n";
        upstream_->deallocate(p, bytes, align);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

    const char*                name_;
    std::pmr::memory_resource* upstream_;
    std::size_t                alloc_count_ = 0;
    std::size_t                total_bytes_ = 0;
};

// ---- 데모 ----------------------------------------------------------------

int main() {
    std::cout << "==============================================\n";
    std::cout << " std::pmr — 폴리모픽 메모리 리소스 실습\n";
    std::cout << "==============================================\n\n";

    // 1. monotonic_buffer_resource — 스택 아레나, 힙 할당 없음
    std::cout << "1. monotonic_buffer_resource (512 바이트 스택 아레나)\n";
    std::cout << "   힙(heap) 할당 없이 push_back × 8:\n";
    {
        std::array<std::byte, 512> buf{};
        // 버퍼 초과 시 null_memory_resource → bad_alloc (초과 방지)
        std::pmr::monotonic_buffer_resource arena(
            buf.data(), buf.size(), std::pmr::null_memory_resource());

        std::pmr::vector<int> v(&arena);
        v.reserve(8);
        for (int i = 1; i <= 8; ++i)
            v.push_back(i * i);

        const int sum = std::accumulate(v.begin(), v.end(), 0);
        std::cout << "   제곱수 합 = " << sum << "\n";
        std::cout << "   ✓ 전체 할당이 스택 버퍼 내에서 완료됨\n\n";
    }

    // 2. 커스텀 LoggingResource — 할당 이벤트 추적
    std::cout << "2. 커스텀 LoggingResource (기본 힙을 업스트림으로)\n";
    {
        LoggingResource logger("Logger");

        std::pmr::vector<int> nums(&logger);
        nums.reserve(5);
        for (int i = 10; i <= 50; i += 10)
            nums.push_back(i);

        std::cout << "   내용:";
        for (int x : nums) std::cout << " " << x;
        std::cout << "\n";
        logger.print_stats();
        std::cout << "\n";
    }

    // 3. 업스트림 체이닝: LoggingResource → monotonic_buffer_resource
    std::cout << "3. 체이닝: LoggingResource → monotonic_buffer_resource\n";
    {
        std::array<std::byte, 2048> buf{};
        std::pmr::monotonic_buffer_resource mono(buf.data(), buf.size());
        LoggingResource logger("Chain", &mono);  // mono 를 업스트림으로 지정

        std::pmr::vector<int> v(&logger);
        for (int i = 0; i < 20; ++i)
            v.push_back(i);

        std::sort(v.begin(), v.end(), std::greater<int>{});
        std::cout << "   내림차순 앞 5개:";
        for (std::size_t i = 0; i < 5; ++i)
            std::cout << " " << v[i];
        std::cout << "\n";
        logger.print_stats();
        std::cout << "\n";
    }

    // 4. unsynchronized_pool_resource — 반복 할당/해제 최적화
    std::cout << "4. unsynchronized_pool_resource (풀 재사용 데모)\n";
    {
        LoggingResource logger("Pool");
        std::pmr::unsynchronized_pool_resource pool(
            std::pmr::pool_options{/* max_blocks_per_chunk= */ 8,
                                   /* largest_required_pool_block= */ 256},
            &logger);

        std::pmr::list<int> lst(&pool);

        // 6개 push_back → 3개 pop_front (반환) → 다시 4개 push_back
        // 반환된 노드가 풀에 캐시되어 재사용됨
        for (int i = 1; i <= 6; ++i) lst.push_back(i);
        for (int k = 0; k < 3; ++k) lst.pop_front();
        for (int i = 100; i <= 103; ++i) lst.push_back(i);

        std::cout << "   최종 리스트:";
        for (int x : lst) std::cout << " " << x;
        std::cout << "\n";
        logger.print_stats();
    }

    return 0;
}
