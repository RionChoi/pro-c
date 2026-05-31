#include <iostream>
#include <sstream>
#include <type_traits>
#include <memory>
#include <map>
#include <string>
#include <typeinfo>
#include <vector>

class Component {
public:
    virtual ~Component() = default;
    virtual std::string serialize() const = 0;
    virtual bool deserialize(const std::string& data) = 0;
};

struct Transform : public Component {
    float x = 0.0f, y = 0.0f, z = 0.0f;

    std::string serialize() const override {
        return std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z);
    }

    bool deserialize(const std::string& data) override {
        std::istringstream iss(data);
        return (iss >> x >> y >> z).operator bool();
    }
};

struct Health : public Component {
    int hp = 100;
    int max_hp = 100;

    std::string serialize() const override {
        return std::to_string(hp) + " " + std::to_string(max_hp);
    }

    bool deserialize(const std::string& data) override {
        std::istringstream iss(data);
        return (iss >> hp >> max_hp).operator bool();
    }
};

struct Damage : public Component {
    int attack_power = 10;
    std::string weapon = "sword";

    std::string serialize() const override {
        return std::to_string(attack_power) + " " + weapon;
    }

    bool deserialize(const std::string& data) override {
        std::istringstream iss(data);
        return (iss >> attack_power >> weapon).operator bool();
    }
};

class Entity {
private:
    std::string name;
    std::map<std::string, std::shared_ptr<Component>> components;

public:
    Entity(const std::string& n) : name(n) {}

    template <typename T>
    T* add_component() {
        static_assert(std::is_base_of_v<Component, T>, "T must be a Component");
        auto comp = std::make_shared<T>();
        std::string type_name = typeid(T).name();
        components[type_name] = comp;
        return comp.get();
    }

    template <typename T>
    T* get_component() {
        static_assert(std::is_base_of_v<Component, T>, "T must be a Component");
        std::string type_name = typeid(T).name();
        auto it = components.find(type_name);
        if (it != components.end()) {
            return dynamic_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    template <typename T>
    bool has_component() const {
        static_assert(std::is_base_of_v<Component, T>, "T must be a Component");
        std::string type_name = typeid(T).name();
        return components.find(type_name) != components.end();
    }

    std::string get_name() const { return name; }

    std::string serialize() const {
        std::ostringstream oss;
        oss << name << "\n";
        for (const auto& [type_name, comp] : components) {
            oss << type_name << ":" << comp->serialize() << "\n";
        }
        return oss.str();
    }

    bool deserialize(const std::string& data) {
        std::istringstream iss(data);
        std::getline(iss, name);

        std::string line;
        while (std::getline(iss, line)) {
            if (line.empty()) continue;

            size_t colon_pos = line.find(':');
            if (colon_pos == std::string::npos) continue;

            std::string type_name = line.substr(0, colon_pos);
            std::string comp_data = line.substr(colon_pos + 1);

            if (type_name.find("Transform") != std::string::npos) {
                auto t = add_component<Transform>();
                t->deserialize(comp_data);
            } else if (type_name.find("Health") != std::string::npos) {
                auto h = add_component<Health>();
                h->deserialize(comp_data);
            } else if (type_name.find("Damage") != std::string::npos) {
                auto d = add_component<Damage>();
                d->deserialize(comp_data);
            }
        }
        return true;
    }

    void print_components() const {
        std::cout << "엔티티: " << name << "\n";
        if (auto t = const_cast<Entity*>(this)->get_component<Transform>()) {
            std::cout << "  위치: (" << t->x << ", " << t->y << ", " << t->z << ")\n";
        }
        if (auto h = const_cast<Entity*>(this)->get_component<Health>()) {
            std::cout << "  체력: " << h->hp << "/" << h->max_hp << "\n";
        }
        if (auto d = const_cast<Entity*>(this)->get_component<Damage>()) {
            std::cout << "  공격력: " << d->attack_power << " (무기: " << d->weapon << ")\n";
        }
    }
};

int main() {
    std::cout << "=== 타입 안전 게임 엔진 (리플렉션 기반) ===\n\n";

    Entity player("Hero");
    auto transform = player.add_component<Transform>();
    transform->x = 10.0f;
    transform->y = 20.0f;
    transform->z = 0.0f;

    auto health = player.add_component<Health>();
    health->hp = 80;
    health->max_hp = 100;

    auto damage = player.add_component<Damage>();
    damage->attack_power = 15;
    damage->weapon = "iron sword";

    std::cout << "플레이어 생성:\n";
    player.print_components();

    std::cout << "\n컴포넌트 쿼리:\n";
    std::cout << "  Transform 있음: " << (player.has_component<Transform>() ? "예" : "아니오") << "\n";
    std::cout << "  Health 있음: " << (player.has_component<Health>() ? "예" : "아니오") << "\n";
    std::cout << "  Damage 있음: " << (player.has_component<Damage>() ? "예" : "아니오") << "\n";

    std::string serialized = player.serialize();
    std::cout << "\n직렬화된 게임 상태:\n";
    std::cout << serialized;

    Entity loaded("Unknown");
    loaded.deserialize(serialized);

    std::cout << "역직렬화된 플레이어:\n";
    loaded.print_components();

    if (auto loaded_transform = loaded.get_component<Transform>()) {
        if (loaded_transform->x == transform->x &&
            loaded_transform->y == transform->y &&
            loaded_transform->z == transform->z) {
            std::cout << "\n✓ Transform 데이터 일치\n";
        }
    }

    if (auto loaded_health = loaded.get_component<Health>()) {
        if (loaded_health->hp == health->hp && loaded_health->max_hp == health->max_hp) {
            std::cout << "✓ Health 데이터 일치\n";
        }
    }

    if (auto loaded_damage = loaded.get_component<Damage>()) {
        if (loaded_damage->attack_power == damage->attack_power &&
            loaded_damage->weapon == damage->weapon) {
            std::cout << "✓ Damage 데이터 일치\n";
        }
    }

    return 0;
}
