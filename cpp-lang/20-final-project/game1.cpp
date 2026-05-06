// Mini-project game applying pattern and module concepts.

#include <iostream>
#include <vector>
#include <string>
#include <memory>

// Assuming the existence of the ServiceManager (from homework.cpp)
// and AbstractModule (from homework.cpp).
class ServiceManager; 

// --- Mock Implementations for stand-alone game file ---
class MockStorageModule : public AbstractModule {
public:
    void initialize() override { std::cout << "[MOCK Storage] Ready." << std::endl; }
    void run() override { std::cout << "[MOCK Storage] Providing state." << std::endl; }
};

class MockNetworkModule : public AbstractModule {
public:
    void initialize() override { std::cout << "[MOCK Network] Connected." << std::endl; }
    void run() override { std::cout << "[MOCK Network] Receiving data." << std::endl; }
};
// -----------------------------------------------------


void simulateGame(ServiceManager& manager) {
    std::cout << "\n==== Starting Game Simulation ===" << std::endl;

    // Game loop uses the services provided by the manager
    std::cout << "Game Logic: Requesting data from the system..." << std::endl;
    manager.executeCycle(); 

    std::cout << "Game Logic: Data received and processed successfully." << std::endl;
}


int main() {
    // The game acts as a client that consumes the system via the manager
    ServiceManager game_manager;

    // Injecting the mock modules instead of real ones for the game environment
    game_manager.registerModule(std::make_unique<MockStorageModule>());
    game_manager.registerModule(std::make_unique<MockNetworkModule>());

    // Initialize the underlying system services
    game_manager.startSystem(); 
    
    // Run the game layer on top
    simulateGame(game_manager);

    return 0;
}
