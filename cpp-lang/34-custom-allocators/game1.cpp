#include <iostream>
#include <vector>
#include <memory_resource>
#include <chrono>
#include <cstdlib>
#include <ctime>

class AllocationGame {
private:
    static constexpr int NUM_ROUNDS = 5;
    static constexpr int OPERATIONS_PER_ROUND = 1000;
    int player_score = 0;
    int cpu_score = 0;

public:
    void display_menu() const {
        std::cout << "\n=== 메모리 할당자 성능 게임 ===\n";
        std::cout << "당신이 가장 효율적인 할당 전략을 선택하세요!\n\n";
    }

    int choose_strategy() {
        std::cout << "할당 전략을 선택하세요:\n";
        std::cout << "1. 기본 할당자 (std::allocator)\n";
        std::cout << "2. Monotonic Buffer (선형 할당)\n";
        std::cout << "3. Pool Resource (풀 기반)\n";
        std::cout << "선택 (1-3): ";

        int choice;
        if (std::cin >> choice && choice >= 1 && choice <= 3) {
            return choice;
        }
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return 1;
    }

    long long run_allocation_test(int strategy, const std::string& name) {
        std::cout << "\n[테스트: " << name << "]\n";

        auto start = std::chrono::high_resolution_clock::now();

        if (strategy == 1) {
            // 기본 할당자
            std::vector<std::vector<int>> data;
            for (int i = 0; i < 100; i++) {
                std::vector<int> inner;
                for (int j = 0; j < 100; j++) {
                    inner.push_back(j);
                }
                data.push_back(inner);
            }
        } else if (strategy == 2) {
            // Monotonic Buffer
            char buffer[1024 * 1024];
            std::pmr::monotonic_buffer_resource res(buffer, sizeof(buffer));
            std::pmr::vector<std::pmr::vector<int>> data(&res);
            for (int i = 0; i < 100; i++) {
                std::pmr::vector<int> inner(&res);
                for (int j = 0; j < 100; j++) {
                    inner.push_back(j);
                }
                data.push_back(inner);
            }
        } else {  // strategy == 3
            // Pool Resource
            std::pmr::unsynchronized_pool_resource res;
            std::pmr::vector<std::pmr::vector<int>> data(&res);
            for (int i = 0; i < 100; i++) {
                std::pmr::vector<int> inner(&res);
                for (int j = 0; j < 100; j++) {
                    inner.push_back(j);
                }
                data.push_back(inner);
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end - start
        );

        std::cout << "소요 시간: " << duration.count() << " 마이크로초\n";
        return duration.count();
    }

    void play_round(int round_num) {
        std::cout << "\n========== 라운드 " << round_num << " ==========\n";

        int player_choice = choose_strategy();
        std::string strategy_names[] = {"", "기본 할당자", "Monotonic Buffer", "Pool Resource"};

        // 플레이어 전략 실행
        auto player_time = run_allocation_test(player_choice, "당신의 선택");

        // CPU 전략 선택 (랜덤)
        int cpu_choice = (std::rand() % 3) + 1;
        auto cpu_time = run_allocation_test(cpu_choice, "CPU 선택 (" + strategy_names[cpu_choice] + ")");

        // 점수 계산
        std::cout << "\n[라운드 결과]\n";
        std::cout << "당신: " << player_time << " 마이크로초\n";
        std::cout << "CPU: " << cpu_time << " 마이크로초\n";

        if (player_time < cpu_time) {
            std::cout << "🎉 당신이 이겼습니다! (+10점)\n";
            player_score += 10;
        } else if (player_time > cpu_time) {
            std::cout << "❌ CPU가 이겼습니다! (CPU +10점)\n";
            cpu_score += 10;
        } else {
            std::cout << "🤝 무승부입니다!\n";
        }

        std::cout << "현재 점수: 당신 " << player_score << " vs CPU " << cpu_score << "\n";
    }

    void display_final_results() {
        std::cout << "\n========== 최종 결과 ==========\n";
        std::cout << "당신의 점수: " << player_score << "\n";
        std::cout << "CPU의 점수: " << cpu_score << "\n";

        if (player_score > cpu_score) {
            std::cout << "\n🏆 축하합니다! 당신이 승리했습니다!\n";
        } else if (player_score < cpu_score) {
            std::cout << "\n💻 CPU가 승리했습니다. 다시 도전하세요!\n";
        } else {
            std::cout << "\n🤝 동점입니다!\n";
        }
    }

    void play() {
        display_menu();

        for (int i = 1; i <= NUM_ROUNDS; i++) {
            play_round(i);
        }

        display_final_results();
    }
};

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    AllocationGame game;
    game.play();

    std::cout << "\n게임 종료. 감사합니다!\n";
    return 0;
}
