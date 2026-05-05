#include <iostream>
#include <thread>
#include <mutex>

int shared_counter = 0;
std::mutex counter_mutex;

// 스레드 안전성을 확인하기 위해 카운터를 증가시키는 함수
void safe_increment(int iterations) {
    for (int i = 0; i < iterations; ++i) {
        // Critical Section: std::lock_guard를 사용하여 스레드가 빠져나갈 때 자동으로 락을 해제합니다 (RAII).
        std::lock_guard<std::mutex> lock(counter_mutex);
        shared_counter++; 
    }
}

void task_mutex_safety() {
    std::cout << "\n--- Task 2: Mutex Safety (Race Condition Prevention) ---\n";
    
    // 초기화
    shared_counter = 0;
    
    // 다수의 스레드를 생성하여 동시에 작업 실행
    std::vector<std::thread> threads;
    int num_threads = 10;
    int iterations_per_thread = 10000;
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(safe_increment, iterations_per_thread);
    }

    // 모든 스레드가 끝날 때까지 대기합니다. 이 과정이 안전한 최종 결과 보장을 합니다.
    for (auto& t : threads) {
        t.join();
    }

    int expected = num_threads * iterations_per_thread;
    std::cout << "Expected count: " << expected << "\n";
    std::cout << "Actual counter value: " << shared_counter << "\n";
    if (shared_counter == expected) {
        std::cout << "✅ SUCCESS: Mutex가 정상적으로 작동하여 카운터 무결성이 유지되었습니다.\n";
    } else {
        std::cout << "❌ FAIL: 카운터 불일치. 동기화에 실패했습니다.\n";
    }
}

int main() {
    task_mutex_safety();
    return 0;
}