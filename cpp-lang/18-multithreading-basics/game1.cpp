#include <iostream>
#include <vector>
#include <string>
#include <future>   // For std::async, std::future
#include <chrono>   // For std::chrono::milliseconds
#include <numeric>  // For std::accumulate
#include <random>

// Function to simulate a time-consuming calculation (e.g., complex game score)
long long calculate_complex_score(const std::vector<int>& player_scores) {
    std::cout << "[Async Task] Starting complex score calculation...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // Simulate heavy computation
    long long total_score = 0;
    for (int score : player_scores) {
        total_score += score;
    }
    std::cout << "[Async Task] Complex score calculation finished.\n";
    return total_score * 10; // Apply some multiplier
}

// Function to simulate loading game assets asynchronously
std::string load_assets_async(const std::string& asset_name) {
    std::cout << "[Async Task] Loading asset: " << asset_name << "...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1500)); // Simulate I/O operation
    std::string result = asset_name + " loaded successfully!";
    std::cout << "[Async Task] Asset loading finished for: " << asset_name << "\n";
    return result;
}

int main() {
    std::cout << "--- Game Simulation with Asynchronous Tasks ---\n";

    // Simulate player scores
    std::vector<int> current_player_scores = {100, 150, 200, 75, 125};
    std::cout << "Player scores recorded. Initiating score calculation...\n";

    // 1. Asynchronously calculate complex game score
    // std::launch::async ensures it runs in a new thread.
    std::future<long long> score_future = std::async(std::launch::async, calculate_complex_score, current_player_scores);

    // 2. Asynchronously load game assets
    std::future<std::string> asset_future1 = std::async(std::launch::async, load_assets_async, "TexturePack_HD");
    std::future<std::string> asset_future2 = std::async(std::launch::async, load_assets_async, "AudioFiles_SFX");

    std::cout << "Main thread: Performing other game logic while async tasks run...\n";
    // Simulate other main thread work
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "Main thread: Still working...\n";

    // Get results from futures. .get() will block until the result is ready.
    std::cout << "\nMain thread: Waiting for complex score calculation...\n";
    long long final_score = score_future.get();
    std::cout << "Main thread: Final Game Score: " << final_score << "\n";

    std::cout << "\nMain thread: Waiting for asset loading...\n";
    std::string asset_result1 = asset_future1.get();
    std::cout << "Main thread: " << asset_result1 << "\n";

    std::string asset_result2 = asset_future2.get();
    std::cout << "Main thread: " << asset_result2 << "\n";

    std::cout << "\nGame initialization complete. Starting game loop (not implemented here).\n";

    return 0;
}
