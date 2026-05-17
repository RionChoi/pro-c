// 세션 24 — 확장 과제: ECS Systems
// 적용 개념: MovementSystem, HealthSystem, RenderSystem, 시스템 루프
#include <iostream>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>
#include <algorithm>

// ── Entity ────────────────────────────────────────────────────────────────────
using Entity = uint32_t;

// ── Components ────────────────────────────────────────────────────────────────
struct Position  { float x = 0, y = 0; };
struct Velocity  { float dx = 0, dy = 0; };
struct Health    { int hp = 0, maxHp = 0; bool dead = false; };
struct Renderable{ char symbol = '?'; std::string color; };
struct Collider  { float radius = 1.0f; };
struct Name      { std::string value; };

// ── ComponentStore<T> ─────────────────────────────────────────────────────────
template <typename T>
struct ComponentStore {
    std::unordered_map<Entity, T> data;

    void  add(Entity e, T c)       { data[e] = std::move(c); }
    void  remove(Entity e)         { data.erase(e); }
    bool  has(Entity e) const      { return data.count(e) > 0; }
    T&    get(Entity e)            { return data.at(e); }
    const T& get(Entity e) const   { return data.at(e); }
};

// ── Registry: 모든 컴포넌트를 통합 관리 ──────────────────────────────────────
struct Registry {
    Entity nextId = 0;
    std::unordered_set<Entity> entities;

    ComponentStore<Position>   positions;
    ComponentStore<Velocity>   velocities;
    ComponentStore<Health>     healths;
    ComponentStore<Renderable> renderables;
    ComponentStore<Collider>   colliders;
    ComponentStore<Name>       names;

    Entity create() {
        Entity e = ++nextId;
        entities.insert(e);
        return e;
    }
    void destroy(Entity e) {
        entities.erase(e);
        positions.remove(e);
        velocities.remove(e);
        healths.remove(e);
        renderables.remove(e);
        colliders.remove(e);
        names.remove(e);
    }
};

// ── Systems ───────────────────────────────────────────────────────────────────

// MovementSystem: Position += Velocity * dt
struct MovementSystem {
    static void update(Registry& reg, float dt) {
        for (auto& [e, vel] : reg.velocities.data) {
            if (!reg.positions.has(e)) continue;
            auto& pos = reg.positions.get(e);
            pos.x += vel.dx * dt;
            pos.y += vel.dy * dt;
        }
    }
};

// BounceSystem: 경계(±20) 에서 반사
struct BounceSystem {
    static constexpr float BOUND = 20.0f;
    static void update(Registry& reg) {
        for (auto& [e, vel] : reg.velocities.data) {
            if (!reg.positions.has(e)) continue;
            auto& pos = reg.positions.get(e);
            if (pos.x >  BOUND || pos.x < -BOUND) { vel.dx = -vel.dx; pos.x = std::clamp(pos.x, -BOUND, BOUND); }
            if (pos.y >  BOUND || pos.y < -BOUND) { vel.dy = -vel.dy; pos.y = std::clamp(pos.y, -BOUND, BOUND); }
        }
    }
};

// CollisionSystem: 두 엔티티가 충돌 시 hp 감소
struct CollisionSystem {
    static void update(Registry& reg) {
        std::vector<Entity> collidables;
        for (Entity e : reg.entities)
            if (reg.positions.has(e) && reg.colliders.has(e) && reg.healths.has(e))
                collidables.push_back(e);

        for (std::size_t i = 0; i < collidables.size(); ++i) {
            for (std::size_t j = i + 1; j < collidables.size(); ++j) {
                Entity a = collidables[i], b = collidables[j];
                auto& pa = reg.positions.get(a);
                auto& pb = reg.positions.get(b);
                float dx = pa.x - pb.x, dy = pa.y - pb.y;
                float dist = std::sqrt(dx*dx + dy*dy);
                float ra = reg.colliders.get(a).radius;
                float rb = reg.colliders.get(b).radius;
                if (dist < ra + rb) {
                    reg.healths.get(a).hp -= 5;
                    reg.healths.get(b).hp -= 5;
                }
            }
        }
    }
};

// HealthSystem: hp <= 0 이면 dead 표시
struct HealthSystem {
    static void update(Registry& reg) {
        std::vector<Entity> toRemove;
        for (auto& [e, h] : reg.healths.data)
            if (h.hp <= 0) { h.dead = true; toRemove.push_back(e); }
        for (Entity e : toRemove) {
            std::string nm = reg.names.has(e) ? reg.names.get(e).value : "?";
            std::cout << "  💀 [" << nm << "] 사망\n";
            reg.destroy(e);
        }
    }
};

// RenderSystem: 간단한 텍스트 렌더링
struct RenderSystem {
    static void update(const Registry& reg) {
        std::cout << "  ── 화면 ─────────────────────────────\n";
        for (Entity e : reg.entities) {
            if (!reg.renderables.has(e) || !reg.positions.has(e)) continue;
            auto& r = reg.renderables.get(e);
            auto& p = reg.positions.get(e);
            std::string nm = reg.names.has(e) ? reg.names.get(e).value : "?";
            std::cout << "  " << r.symbol << " [" << nm << "]"
                      << std::fixed << std::setprecision(1)
                      << " (" << p.x << "," << p.y << ")";
            if (reg.healths.has(e)) {
                auto& h = reg.healths.get(e);
                std::cout << " hp=" << h.hp << "/" << h.maxHp;
            }
            std::cout << "\n";
        }
    }
};

// ── main ──────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "=== ECS Systems 과제 ===\n\n";

    Registry reg;

    // 엔티티 생성
    Entity player = reg.create();
    reg.names.add(player,      {"플레이어"});
    reg.positions.add(player,  {0, 0});
    reg.velocities.add(player, {3, 2});
    reg.healths.add(player,    {100, 100, false});
    reg.renderables.add(player,{'@', "green"});
    reg.colliders.add(player,  {1.5f});

    Entity enemy1 = reg.create();
    reg.names.add(enemy1,      {"고블린"});
    reg.positions.add(enemy1,  {4, 3});
    reg.velocities.add(enemy1, {-2, 1});
    reg.healths.add(enemy1,    {30, 30, false});
    reg.renderables.add(enemy1,{'G', "red"});
    reg.colliders.add(enemy1,  {1.0f});

    Entity enemy2 = reg.create();
    reg.names.add(enemy2,      {"오크"});
    reg.positions.add(enemy2,  {-5, 8});
    reg.velocities.add(enemy2, {1, -1});
    reg.healths.add(enemy2,    {60, 60, false});
    reg.renderables.add(enemy2,{'O', "red"});
    reg.colliders.add(enemy2,  {2.0f});

    Entity item = reg.create();
    reg.names.add(item,        {"포션"});
    reg.positions.add(item,    {10, 10});
    reg.renderables.add(item,  {'!', "blue"});
    // item: velocity/health 없음 — 정적 오브젝트

    // 시뮬레이션 루프 (5 틱)
    const float dt = 1.0f;
    for (int tick = 1; tick <= 5; ++tick) {
        std::cout << "\n[틱 " << tick << "]\n";
        MovementSystem::update(reg, dt);
        BounceSystem::update(reg);
        CollisionSystem::update(reg);
        HealthSystem::update(reg);
        RenderSystem::update(reg);
    }

    std::cout << "\n최종 생존 엔티티 수: " << reg.entities.size() << "\n";

    return 0;
}
