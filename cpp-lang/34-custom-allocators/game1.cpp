#include <iostream>
#include <vector>
#include <memory_resource>
#include <string>
#include <cstdlib>
#include <ctime>

// 게임용 추적 리소스
class GameMemoryResource : public std::pmr::memory_resource {
private:
    std::pmr::memory_resource *upstream;
    std::size_t total_allocated = 0;
    std::size_t peak_usage = 0;
    int allocation_count = 0;

    void *do_allocate(std::size_t bytes, std::size_t alignment) override {
        void *ptr = upstream->allocate(bytes, alignment);
        total_allocated += bytes;
        if (total_allocated > peak_usage) {
            peak_usage = total_allocated;
        }
        allocation_count++;
        return ptr;
    }

    void do_deallocate(void *p, std::size_t bytes, std::size_t alignment) override {
        upstream->deallocate(p, bytes, alignment);
        total_allocated -= bytes;
    }

    bool do_is_equal(const memory_resource &other) const noexcept override {
        return this == &other;
    }

public:
    GameMemoryResource()
        : upstream(std::pmr::new_delete_resource()) {}

    std::size_t current_usage() const { return total_allocated; }
    std::size_t get_peak_usage() const { return peak_usage; }
    int get_allocation_count() const { return allocation_count; }
};

int main() {
    std::cout << "🎮 메모리 효율성 게임\n";
    std::cout << "=== 벡터 크기를 추측하고 메모리 사용량을 최소화하세요 ===\n\n";

    GameMemoryResource game_res;
    std::srand(std::time(nullptr));

    int score = 0;
    int round = 0;

    while (round < 3) {
        round++;
        std::cout << "🔹 라운드 " << round << ":\n";

        int target_size = 10 + std::rand() % 90;
        std::cout << "목표 벡터 크기: " << target_size << " 요소\n";
        std::cout << "크기를 맞췄을 때의 용량(capacity)을 예측하세요: ";

        int guess;
        std::cin >> guess;

        // 게임 실행
        {
            std::pmr::vector<int> v(&game_res);
            v.reserve(guess);
            for (int i = 0; i < target_size; ++i) {
                v.push_back(i);
            }

            int actual_capacity = v.capacity();
            int error = std::abs(guess - actual_capacity);
            int round_score = std::max(0, 100 - error * 10);
            score += round_score;

            std::cout << "실제 용량: " << actual_capacity << "\n";
            std::cout << "오차: " << error << " → 점수: " << round_score << " 점\n";
            std::cout << "누적 점수: " << score << "\n\n";
        }
    }

    std::cout << "📊 게임 완료!\n";
    std::cout << "최종 점수: " << score << " / 300\n";
    std::cout << "피크 메모리: " << game_res.get_peak_usage() << " 바이트\n";
    std::cout << "총 할당 횟수: " << game_res.get_allocation_count() << "\n";

    if (score >= 250) {
        std::cout << "🏆 우수! 메모리 관리 전문가!\n";
    } else if (score >= 150) {
        std::cout << "👍 좋음! 계속 연습하세요.\n";
    } else {
        std::cout << "📚 더 많은 학습이 필요합니다.\n";
    }

    return 0;
}
