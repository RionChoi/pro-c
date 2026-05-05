#include <iostream>
#include <vector>
#include <string>
#include <memory> // For std::unique_ptr, std::shared_ptr
#include <random> // For random numbers
#include <chrono> // For seeding random number generator

// Base class for a game entity
class GameEntity {
protected:
    std::string name;
    int health;

public:
    GameEntity(const std::string& n, int h) : name(n), health(h) {
        std::cout << "Entity " << name << " created with " << health << " health.\n";
    }
    virtual ~GameEntity() {
        std::cout << "Entity " << name << " destroyed.\n";
    }

    virtual void attack(GameEntity& target) = 0;
    virtual void takeDamage(int damage) {
        health -= damage;
        std::cout << name << " took " << damage << " damage. Health: " << health << "\n";
        if (health <= 0) {
            std::cout << name << " has been defeated!\n";
        }
    }
    bool isAlive() const {
        return health > 0;
    }
    const std::string& getName() const {
        return name;
    }
    int getHealth() const {
        return health;
    }
};

// Player class
class Player : public GameEntity {
private:
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution;
public:
    Player(const std::string& n) : GameEntity(n, 100),
        generator(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count())),
        distribution(10, 25) {}

    void attack(GameEntity& target) override {
        int damage = distribution(generator);
        std::cout << name << " attacks " << target.getName() << " for " << damage << " damage.\n";
        target.takeDamage(damage);
    }
};

// Enemy class
class Enemy : public GameEntity {
private:
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution;
public:
    Enemy(const std::string& n, int h) : GameEntity(n, h),
        generator(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count())),
        distribution(5, 15) {}

    void attack(GameEntity& target) override {
        int damage = distribution(generator);
        std::cout << name << " attacks " << target.getName() << " for " << damage << " damage.\n";
        target.takeDamage(damage);
    }
};

// Game class to manage entities using smart pointers
class Game {
private:
    std::vector<std::unique_ptr<GameEntity>> entities;
    std::shared_ptr<Player> player;

public:
    Game(const std::string& playerName) {
        player = std::make_shared<Player>(playerName);
        entities.push_back(std::make_unique<Enemy>("Goblin", 50));
        entities.push_back(std::make_unique<Enemy>("Orc", 70));
        std::cout << "Game initialized. Player: " << player->getName() << ". Total entities: " << entities.size() << "\n";
    }

    void addEnemy(const std::string& name, int health) {
        entities.push_back(std::make_unique<Enemy>(name, health));
        std::cout << "Added new enemy: " << name << ". Total entities: " << entities.size() << "\n";
    }

    void startBattle() {
        std::cout << "\n--- Battle Start! ---\n";

        while (player->isAlive() && !entities.empty()) {
            // Player's turn
            std::cout << "\n" << player->getName() << "'s turn!\n";
            // Target the first alive enemy
            size_t target_idx = 0;
            while (target_idx < entities.size() && !entities[target_idx]->isAlive()) {
                target_idx++;
            }

            if (target_idx < entities.size()) {
                player->attack(*entities[target_idx]);
                if (!entities[target_idx]->isAlive()) {
                    std::cout << entities[target_idx]->getName() << " was defeated by " << player->getName() << "!\n";
                    // Remove defeated enemy using unique_ptr features
                    entities.erase(entities.begin() + static_cast<long>(target_idx));
                }
            } else {
                std::cout << "No more enemies left!\n";
                break;
            }

            if (entities.empty()) {
                std::cout << "All enemies defeated!\n";
                break;
            }

            // Enemies' turn
            std::cout << "\nEnemies' turn!\n";
            for (const auto& enemy : entities) {
                if (enemy->isAlive()) {
                    enemy->attack(*player);
                    if (!player->isAlive()) {
                        std::cout << player->getName() << " has been defeated! Game Over!\n";
                        return;
                    }
                }
            }
            std::cout << "---\n";
        }

        std::cout << "\n--- Battle End ---\n";
        if (player->isAlive()) {
            std::cout << player->getName() << " is victorious!\n";
        } else {
            std::cout << "You were defeated. Game Over!\n";
        }
    }
};

int main() {
    // Create a game instance, managing entities with smart pointers
    Game myGame("Hero");

    // Add another enemy dynamically
    myGame.addEnemy("Dragon", 150);

    // Start the battle simulation
    myGame.startBattle();

    std::cout << "\nGame application ending. All smart pointers will clean up resources.\n";

    return 0;
}
