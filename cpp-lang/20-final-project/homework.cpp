#include <iostream>
#include <vector>
#include <memory>
#include <string>

// Simple Component Interface
class AbstractModule {
public:
    virtual ~AbstractModule() = default;
    virtual void initialize() = 0;
    virtual void run() = 0;
};

class StorageModule : public AbstractModule {
public:
    void initialize() override {
        std::cout << "[Storage] Initializing permanent data storage layer." << std::endl;
    }
    void run() override {
        std::cout << "[Storage] Retrieving simulated data record." << std::endl;
    }
};

class NetworkModule : public AbstractModule {
public:
    void initialize() override {
        std::cout << "[Network] Establishing connection to external API." << std::endl;
    }
    void run() override {
        std::cout << "[Network] Syncing state with remote server." << std::endl;
    }
};

// Service Manager (The central orchestrator using smart pointers for safety)
class ServiceManager {
private:
    std::vector<std::unique_ptr<AbstractModule>> dependencies;

public:
    // Dependency Injection via constructor/setter (demonstrating ownership transfer)
    void registerModule(std::unique_ptr<AbstractModule> module) {
        dependencies.push_back(std::move(module));
        std::cout << "[Manager] Module registered successfully." << std::endl;
    }

    void startSystem() {
        std::cout << "\n=== Starting Service System ===" << std::endl;
        for (const auto& module : dependencies) {
            module->initialize();
        }
        std::cout << "=== Initialization Complete ===\n" << std::endl;
    }

    void executeCycle() {
        std::cout << "--- Executing Operational Cycle ---\n";
        for (const auto& module : dependencies) {
            module->run();
        }
        std::cout << "--- Cycle Complete ---\n" << std::endl;
    }
};

int main() {
    ServiceManager manager;

    // Registering dependencies using std::make_unique and std::move
    manager.registerModule(std::make_unique<StorageModule>());
    manager.registerModule(std::make_unique<NetworkModule>());

    manager.startSystem();
    manager.executeCycle();

    // When manager is destroyed, all unique_ptrs automatically call destructors (RAII)
    std::cout << "Service System shutdown sequence initiated." << std::endl;
    return 0;
}