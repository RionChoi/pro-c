#include <iostream>
#include <vector>
#include <array>
#include <variant>
#include <memory>
#include <unordered_map>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <cstring>

using EntityID = uint32_t;
constexpr EntityID INVALID_ENTITY = 0xFFFFFFFF;

struct Position {
    double x, y, z;
    Position() : x(0), y(0), z(0) {}
    Position(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
};

struct Velocity {
    double dx, dy, dz;
    Velocity() : dx(0), dy(0), dz(0) {}
    Velocity(double dx_, double dy_, double dz_) : dx(dx_), dy(dy_), dz(dz_) {}
};

struct Health {
    int current, max;
    Health() : current(100), max(100) {}
    Health(int cur, int mx) : current(cur), max(mx) {}
};

struct Inventory {
    int gold, items_count;
    Inventory() : gold(0), items_count(0) {}
    Inventory(int g, int ic) : gold(g), items_count(ic) {}
};

using Component = std::variant<Position, Velocity, Health, Inventory>;

class Entity {
public:
    EntityID id;
    std::vector<Component> components;

    Entity(EntityID id_) : id(id_), components() {}

    template <typename T>
    void add_component(const T& comp) {
        components.emplace_back(comp);
    }

    template <typename T>
    T* get_component() {
        for (auto& comp : components) {
            if (std::holds_alternative<T>(comp)) {
                return &std::get<T>(comp);
            }
        }
        return nullptr;
    }

    bool has_component(size_t type_id) const {
        for (const auto& comp : components) {
            if (comp.index() == type_id) return true;
        }
        return false;
    }
};

class GameWorld {
private:
    std::unordered_map<EntityID, Entity> entities;
    EntityID next_entity_id;

public:
    GameWorld() : next_entity_id(1) {}

    EntityID create_entity() {
        EntityID id = next_entity_id++;
        entities.emplace(id, Entity(id));
        return id;
    }

    Entity* get_entity(EntityID id) {
        auto it = entities.find(id);
        return it != entities.end() ? &it->second : nullptr;
    }

    size_t entity_count() const { return entities.size(); }

    void update(double dt) {
        for (auto& [id, entity] : entities) {
            Position* pos = entity.get_component<Position>();
            Velocity* vel = entity.get_component<Velocity>();
            Health* health = entity.get_component<Health>();

            if (pos && vel) {
                pos->x += vel->dx * dt;
                pos->y += vel->dy * dt;
                pos->z += vel->dz * dt;
            }

            if (health && health->current > 0) {
                health->current = std::max(0, health->current - 1);
            }
        }
    }

    void serialize_to_file(const std::string& filename) const {
        std::ofstream file(filename, std::ios::binary);
        if (!file) return;

        uint32_t entity_count = static_cast<uint32_t>(entities.size());
        file.write(reinterpret_cast<const char*>(&entity_count), sizeof(entity_count));

        for (const auto& [id, entity] : entities) {
            file.write(reinterpret_cast<const char*>(&id), sizeof(id));
            uint32_t comp_count = static_cast<uint32_t>(entity.components.size());
            file.write(reinterpret_cast<const char*>(&comp_count), sizeof(comp_count));

            for (const auto& comp : entity.components) {
                uint8_t type_idx = static_cast<uint8_t>(comp.index());
                file.write(reinterpret_cast<const char*>(&type_idx), sizeof(type_idx));

                if (std::holds_alternative<Position>(comp)) {
                    const auto& p = std::get<Position>(comp);
                    file.write(reinterpret_cast<const char*>(&p.x), sizeof(p.x));
                    file.write(reinterpret_cast<const char*>(&p.y), sizeof(p.y));
                    file.write(reinterpret_cast<const char*>(&p.z), sizeof(p.z));
                } else if (std::holds_alternative<Velocity>(comp)) {
                    const auto& v = std::get<Velocity>(comp);
                    file.write(reinterpret_cast<const char*>(&v.dx), sizeof(v.dx));
                    file.write(reinterpret_cast<const char*>(&v.dy), sizeof(v.dy));
                    file.write(reinterpret_cast<const char*>(&v.dz), sizeof(v.dz));
                } else if (std::holds_alternative<Health>(comp)) {
                    const auto& h = std::get<Health>(comp);
                    file.write(reinterpret_cast<const char*>(&h.current), sizeof(h.current));
                    file.write(reinterpret_cast<const char*>(&h.max), sizeof(h.max));
                } else if (std::holds_alternative<Inventory>(comp)) {
                    const auto& inv = std::get<Inventory>(comp);
                    file.write(reinterpret_cast<const char*>(&inv.gold), sizeof(inv.gold));
                    file.write(reinterpret_cast<const char*>(&inv.items_count), sizeof(inv.items_count));
                }
            }
        }
    }

    void deserialize_from_file(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) return;

        entities.clear();
        next_entity_id = 1;

        uint32_t entity_count = 0;
        file.read(reinterpret_cast<char*>(&entity_count), sizeof(entity_count));

        for (uint32_t i = 0; i < entity_count; ++i) {
            EntityID id = 0;
            file.read(reinterpret_cast<char*>(&id), sizeof(id));

            Entity entity(id);
            uint32_t comp_count = 0;
            file.read(reinterpret_cast<char*>(&comp_count), sizeof(comp_count));

            for (uint32_t j = 0; j < comp_count; ++j) {
                uint8_t type_idx = 0;
                file.read(reinterpret_cast<char*>(&type_idx), sizeof(type_idx));

                if (type_idx == 0) {
                    double x, y, z;
                    file.read(reinterpret_cast<char*>(&x), sizeof(x));
                    file.read(reinterpret_cast<char*>(&y), sizeof(y));
                    file.read(reinterpret_cast<char*>(&z), sizeof(z));
                    entity.add_component(Position(x, y, z));
                } else if (type_idx == 1) {
                    double dx, dy, dz;
                    file.read(reinterpret_cast<char*>(&dx), sizeof(dx));
                    file.read(reinterpret_cast<char*>(&dy), sizeof(dy));
                    file.read(reinterpret_cast<char*>(&dz), sizeof(dz));
                    entity.add_component(Velocity(dx, dy, dz));
                } else if (type_idx == 2) {
                    int current, max;
                    file.read(reinterpret_cast<char*>(&current), sizeof(current));
                    file.read(reinterpret_cast<char*>(&max), sizeof(max));
                    entity.add_component(Health(current, max));
                } else if (type_idx == 3) {
                    int gold, items_count;
                    file.read(reinterpret_cast<char*>(&gold), sizeof(gold));
                    file.read(reinterpret_cast<char*>(&items_count), sizeof(items_count));
                    entity.add_component(Inventory(gold, items_count));
                }
            }

            entities.emplace(id, entity);
            if (id >= next_entity_id) next_entity_id = id + 1;
        }
    }
};

class EntityBuilder {
private:
    Entity entity;

public:
    explicit EntityBuilder(EntityID id) : entity(id) {}

    EntityBuilder& with_position(double x, double y, double z) {
        entity.add_component(Position(x, y, z));
        return *this;
    }

    EntityBuilder& with_velocity(double dx, double dy, double dz) {
        entity.add_component(Velocity(dx, dy, dz));
        return *this;
    }

    EntityBuilder& with_health(int current, int max) {
        entity.add_component(Health(current, max));
        return *this;
    }

    EntityBuilder& with_inventory(int gold, int items) {
        entity.add_component(Inventory(gold, items));
        return *this;
    }

    Entity build() { return entity; }
};

int main() {
    std::cout << "=== Game Engine with ECS Architecture ===" << std::endl << std::endl;

    GameWorld world;

    std::cout << "1. Creating 100-entity simulation:" << std::endl;
    for (EntityID i = 0; i < 100; ++i) {
        EntityID id = world.create_entity();
        Entity* entity = world.get_entity(id);

        double x = static_cast<double>(i % 10) * 10.0;
        double y = static_cast<double>(i / 10) * 10.0;
        double z = 0.0;

        entity->add_component(Position(x, y, z));
        entity->add_component(Velocity(1.0 + i * 0.01, 0.5 + i * 0.005, 0.0));
        entity->add_component(Health(100 - static_cast<int>(i) % 50, 100));
        entity->add_component(Inventory(i * 10, static_cast<int>(i) % 5));
    }
    std::cout << "   Created " << world.entity_count() << " entities." << std::endl << std::endl;

    std::cout << "2. Entity Builder Pattern (DSL):" << std::endl;
    EntityID hero_id = world.create_entity();
    Entity* hero = world.get_entity(hero_id);

    EntityBuilder builder(hero_id);
    Entity built_entity = builder
        .with_position(50.0, 50.0, 0.0)
        .with_velocity(2.0, 1.0, 0.0)
        .with_health(200, 200)
        .with_inventory(1000, 10)
        .build();

    *hero = built_entity;
    std::cout << "   Built hero entity #" << hero_id << " with DSL pattern." << std::endl << std::endl;

    std::cout << "3. World Physics Update (1.0 second, 0.01s timestep):" << std::endl;
    for (int step = 0; step < 100; ++step) {
        world.update(0.01);
    }

    Entity* sample = world.get_entity(1);
    if (sample) {
        Position* pos = sample->get_component<Position>();
        Velocity* vel = sample->get_component<Velocity>();
        Health* health = sample->get_component<Health>();
        if (pos && vel && health) {
            std::cout << "   Entity #1 after 1.0s:" << std::endl;
            std::cout << "   Position: (" << std::fixed << std::setprecision(2)
                      << pos->x << ", " << pos->y << ", " << pos->z << ")" << std::endl;
            std::cout << "   Velocity: (" << vel->dx << ", " << vel->dy << ", " << vel->dz << ")" << std::endl;
            std::cout << "   Health: " << health->current << "/" << health->max << std::endl;
        }
    }
    std::cout << std::endl;

    std::cout << "4. Game State Serialization (Save):" << std::endl;
    world.serialize_to_file("game_state.bin");
    std::cout << "   Saved world state to 'game_state.bin'." << std::endl << std::endl;

    std::cout << "5. Game State Deserialization (Load):" << std::endl;
    GameWorld loaded_world;
    loaded_world.deserialize_from_file("game_state.bin");
    std::cout << "   Loaded " << loaded_world.entity_count() << " entities from file." << std::endl << std::endl;

    std::cout << "6. Serialized Entity Verification:" << std::endl;
    Entity* loaded_entity = loaded_world.get_entity(1);
    if (loaded_entity) {
        Position* pos = loaded_entity->get_component<Position>();
        Health* health = loaded_entity->get_component<Health>();
        Inventory* inv = loaded_entity->get_component<Inventory>();
        if (pos && health && inv) {
            std::cout << "   Entity #1 (loaded):" << std::endl;
            std::cout << "   Position: (" << std::fixed << std::setprecision(2)
                      << pos->x << ", " << pos->y << ", " << pos->z << ")" << std::endl;
            std::cout << "   Health: " << health->current << "/" << health->max << std::endl;
            std::cout << "   Inventory Gold: " << inv->gold << std::endl;
        }
    }
    std::cout << std::endl;

    std::cout << "7. Component Type Query (Hero entity):" << std::endl;
    if (hero) {
        std::cout << "   Hero has Position: " << (hero->get_component<Position>() ? "yes" : "no") << std::endl;
        std::cout << "   Hero has Velocity: " << (hero->get_component<Velocity>() ? "yes" : "no") << std::endl;
        std::cout << "   Hero has Health: " << (hero->get_component<Health>() ? "yes" : "no") << std::endl;
        std::cout << "   Hero has Inventory: " << (hero->get_component<Inventory>() ? "yes" : "no") << std::endl;
    }
    std::cout << std::endl;

    std::cout << "=== Game Engine Complete ===" << std::endl;

    return 0;
}
