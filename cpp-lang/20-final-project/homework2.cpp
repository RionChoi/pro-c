// Extended exercise focusing on resource management and complex data handling.
// This file extends homework.cpp by introducing a complex data structure 
// (like a transaction pipeline) that requires careful memory management 
// and potentially C++11 concurrency features.

#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <mutex>
#include <thread>

// Simulating a complex data object (e.g., a financial transaction)
struct Transaction {
    double amount;
    std::string description;
    bool is_processed = false;
};

// Thread-safe Transaction Pipeline Manager
class TransactionPipeline {
private:
    std::vector<Transaction> queue;
    std::mutex mtx; // Mutex for thread safety
    bool running = true;

    void processTransaction(Transaction& tx) {
        // Simulate complex processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        tx.is_processed = true;
    }

public:
    // A worker thread function
    void workerThread() {
        while (running) {
            Transaction tx;
            bool took_transaction = false;

            // RAII lock_guard ensures the mutex is released automatically
            std::unique_lock<std::mutex> lock(mtx); 

            if (!queue.empty()) {
                tx = std::move(queue.front()); // Move operation for efficiency
                queue.erase(queue.begin());
                took_transaction = true;
            }
            lock.unlock(); // Explicit unlock after taking the transaction

            if (took_transaction) {
                processTransaction(tx);
            }
        }
    }

    void submitTransaction(Transaction tx) {
        std::lock_guard<std::mutex> lock(mtx); // Lock for write access
        queue.push_back(std::move(tx));
    }

    // Runs multiple worker threads to process the queue
    void startProcessing(int num_threads) {
        std::cout << "Starting Transaction Pipeline with " << num_threads << " threads." << std::endl;
        std::vector<std::thread> workers;
        for (int i = 0; i < num_threads; ++i) {
            workers.emplace_back(&TransactionPipeline::workerThread, this);
        }

        // Simulate submitting batch tasks
        for (int i = 0; i < 20; ++i) {
            Transaction t = { (double)(i + 1) * 100.5, "Batch Item " + std::to_string(i)};
            submitTransaction(t);
        }

        // Wait until the queue is empty and then shut down
        while (true) {
            std::lock_guard<std::mutex> lock(mtx);
            if (queue.empty()) {
                running = false;
                break;
            }
            // Sleep briefly to allow threads to run
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
        }
        
        // Join all worker threads
        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }
};

int main() {
    TransactionPipeline pipeline;
    pipeline.startProcessing(4); // Use 4 concurrent worker threads
    std::cout << "\nAll transactions processed. Pipeline shutdown complete." << std::endl;
    return 0;
}