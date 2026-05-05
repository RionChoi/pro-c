#include <iostream>
#include <thread>
#include <chrono>

// 스레드에서 실행될 함수
void worker_function(int id) {
    std::cout << "Thread " << id << " started. Doing work...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * id));
    std::cout << "Thread " << id << " finished.\n";
}

void task_basic_thread_creation() {
    std::cout << "\n--- Task 1: Basic Thread Creation ---\n";
    
    // std::thread 생성자 호출 시 worker_function과 arg(1)을 인수로 받음.
    std::thread t1(worker_function, 1);
    std::thread t2(worker_function, 2);

    // 스레드가 종료될 때까지 기다립니다 (Join). 이는 스레드가 강제 종료되는 것을 막는 중요한 단계입니다.
    t1.join();
    t2.join();
    std::cout << "All basic threads joined successfully.\n";
}

int main() {
    task_basic_thread_creation();
    return 0;
}