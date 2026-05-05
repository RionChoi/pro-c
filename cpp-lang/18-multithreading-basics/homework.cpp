#include <iostream>
#include <thread> // For std::thread
#include <vector> // For std::vector
#include <chrono> // For std::chrono::seconds

// Function to be executed by a thread
void thread_function(int id) {
    std::cout << "Thread " << id << ": Starting work...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulate work
    std::cout << "Thread " << id << ": Finished work.\n";
}

// Function to demonstrate passing arguments to a thread
void thread_with_args(int id, const std::string& message) {
    std::cout << "Thread " << id << ": Message received: " << message << "\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Thread " << id << ": Args processing finished.\n";
}

// Function to demonstrate a detached thread
void detached_thread_function() {
    std::cout << "Detached thread: Starting background work...\n";
    std::this_thread::sleep_for(std::chrono::seconds(3)); // Simulate long background task
    std::cout << "Detached thread: Background work finished. Exiting.\n";
}

int main() {
    std::cout << "--- std::thread Demonstration ---\n";

    // 1. Basic thread creation and join
    std::cout << "Main thread: Creating thread1...\n";
    std::thread thread1(thread_function, 1);
    std::cout << "Main thread: Waiting for thread1 to finish (join)...\n";
    thread1.join(); // Main thread waits for thread1 to complete
    std::cout << "Main thread: Thread1 joined.\n";

    // 2. Thread with arguments
    std::cout << "\nMain thread: Creating thread2 with arguments...\n";
    std::string msg = "Hello from main!";
    // Note: passing by value for primitive types, by const reference for objects often makes sense.
    // If a non-const reference is needed, ensure the object outlives the thread or pass std::ref.
    std::thread thread2(thread_with_args, 2, msg); 
    std::cout << "Main thread: Waiting for thread2 to finish (join)...\n";
    thread2.join();
    std::cout << "Main thread: Thread2 joined.\n";

    // 3. Multiple threads using a vector
    std::cout << "\nMain thread: Creating multiple threads...\n";
    std::vector<std::thread> threads;
    for (int i = 3; i <= 5; ++i) {
        threads.emplace_back(thread_function, i);
    }

    std::cout << "Main thread: Waiting for all threads in vector to finish...\n";
    for (auto& t : threads) {
        t.join(); // Join each thread
    }
    std::cout << "Main thread: All threads in vector joined.\n";

    // 4. Detached thread
    // A detached thread runs independently. The main thread does not wait for it.
    // If the main thread exits before the detached thread, the detached thread might be terminated.
    std::cout << "\nMain thread: Creating a detached thread (thread_detached)...\n";
    std::thread thread_detached(detached_thread_function);
    thread_detached.detach(); // Detach the thread
    std::cout << "Main thread: Thread_detached has been detached. It runs in the background.\n";
    std::cout << "Main thread: Doing some other work briefly...\n";
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Give detached thread a chance to start
    std::cout << "Main thread: Finishing its work and exiting. Detached thread might still be running.\n";

    // It's important to either join or detach a thread. If not, the program will terminate
    // with an error when the std::thread object is destroyed.

    return 0;
}
