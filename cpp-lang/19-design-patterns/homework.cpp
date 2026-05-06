#include <iostream>
#include <memory>

// Singleton Pattern Implementation
class Logger {
private:
    static Logger* instance;
    Logger() = default; // Private constructor
    Logger(const Logger&) = delete; // Delete copy constructor
    Logger& operator=(const Logger&) = delete; // Delete assignment operator

public:
    static Logger* getInstance() {
        if (!instance) {
            instance = new Logger();
        }
        return instance;
    }

    void log(const std::string& message) {
        std::cout << "[LOG] " << message << std::endl;
    }
};

Logger* Logger::instance = nullptr;

int main() {
    Logger* logger = Logger::getInstance();
    logger->log("Application starting up with Singleton Logger.");
    
    // Try to get another instance (should return the same one)
    Logger* logger2 = Logger::getInstance();
    if (logger == logger2) {
        logger2->log("Successfully retrieved the same Singleton instance.");
    }
    
    // Clean up (in a real scenario, a shutdown hook would be used)
    delete Logger::getInstance(); 

    return 0;
}
