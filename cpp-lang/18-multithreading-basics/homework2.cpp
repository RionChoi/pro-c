#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

std::mutex mtx; // Global mutex to protect shared_counter
int shared_counter = 0;

void increment_counter_safe() {
    for (int i = 0; i < 10000; ++i) {
        std::lock_guard<std::mutex> lock(mtx); // RAII: acquires lock on construction, releases on destruction
        shared_counter++;
    }
}

void increment_counter_unsafe() {
    for (int i = 0; i < 10000; ++i) {
        // No mutex protection, leads to race condition
        shared_counter++; 
    }
}

int main() {
    const int num_threads = 10;
    std::vector<std::thread> threads;

    std::cout << "--- Demonstrating Race Condition (Unsafe) ---\n";
    shared_counter = 0; // Reset counter
    threads.clear();

    // Create threads to increment counter unsafely
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(increment_counter_unsafe);
    }

    // Join all threads
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Unsafe final shared_counter value: " << shared_counter << " (Expected: " << (num_threads * 10000) << ")\n";
    std::cout << "Note: The 'Unsafe' value will likely be less than expected due to race conditions.\n";

    std::cout << "\n--- Demonstrating Mutex and Lock_Guard (Safe) ---\n";
    shared_counter = 0; // Reset counter
    threads.clear();

    // Create threads to increment counter safely with mutex and lock_guard
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(increment_counter_safe);
    }

    // Join all threads
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Safe final shared_counter value: " << shared_counter << " (Expected: " << (num_threads * 10000) << ")\n";
    std::cout << "Note: The 'Safe' value should be exactly the expected value.\n";

    std::cout << "\nExiting main.\n";

    return 0;
}
