// 세션 24 — 핵심 과제: ECS 코어
// 적용 개념: Entity ID, ComponentStore<T>, World, 타입 안전 저장소
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <typeindex>
#include <memory>
#include <stdexcept>
#include <iomanip>

// ── Entity ────────────────────────────────────────────────────────────────────
using Entity = uint32_t;
constexpr Entity NULL_ENTITY = 0;

// ── Components (순수 데이터) ──────────────────────────────────────────────────
struct Position  { float x = 0, y = 0; };
struct Velocity  { float dx = 0, dy = 0; };
struct Health    { int current = 0, max = 0; };
struct Name      { std::string value; };
struct Tag       { std::string label; };   // "player", "enemy", "item"

// ── ComponentStore<T>: 타입별 컴포넌트 저장소 ─────────────────────────────────
template <typename T>
class ComponentStore {
public:
    void add(Entity e, T component) {
        data_[e] = std::move(component);
    }
    void remove(Entity e) { data_.erase(e); }
    bool has(Entity e) const { return data_.count(e) > 0; }

    T& get(Entity e) {
        auto it = data_.find(e);
        if (it == data_.end())
            throw std::runtime_error("Component not found");
        return it->second;
    }
    const T& get(Entity e) const {
        auto it = data_.find(e);
        if (it == data_.end())
            throw std::runtime_error("Component not found");
        return it->second;
    }

    const std::unordered_map<Entity, T>& all() const { return data_; }

private:
    std::unordered_map<Entity, T> data_;
};

// ── World: 엔티티 + 컴포넌트 저장소 통합 관리 ─────────────────────────────────
class World {
public:
    // 엔티티 생성/삭제
    Entity create() { return ++nextId_; }

    void destroy(Entity e) {
        positions_.remove(e);
        velocities_.remove(e);
        healths_.remove(e);
        names_.remove(e);
        tags_.remove(e);
        alive_.erase(e);
    }

    // 컴포넌트 추가
    World& add(Entity e, Position  c) { positions_.add(e, c);  alive_.insert(e); return *this; }
    World& add(Entity e, Velocity  c) { velocities_.add(e, c); alive_.insert(e); return *this; }
    World& add(Entity e, Health    c) { healths_.add(e, c);    alive_.insert(e); return *this; }
    World& add(Entity e, Name      c) { names_.add(e, c);      alive_.insert(e); return *this; }
    World& add(Entity e, Tag       c) { tags_.add(e, c);       alive_.insert(e); return *this; }

    // 컴포넌트 접근
    ComponentStore<Position>&  positions()  { return positions_; }
    ComponentStore<Velocity>&  velocities() { return velocities_; }
    ComponentStore<Health>&    healths()    { return healths_; }
    ComponentStore<Name>&      names()      { return names_; }
    ComponentStore<Tag>&       tags()       { return tags_; }

    // 특정 컴포넌트를 모두 가진 엔티티 조회
    std::vector<Entity> query_pos_vel() const {
        std::vector<Entity> result;
        for (auto& [e, _] : positions_.all())
            if (velocities_.has(e))
                result.push_back(e);
        return result;
    }

    std::vector<Entity> query_pos_health() const {
        std::vector<Entity> result;
        for (auto& [e, _] : positions_.all())
            if (healths_.has(e))
                result.push_back(e);
        return result;
    }

    const std::unordered_set<Entity>& alive() const { return alive_; }

private:
    Entity nextId_ = 0;
    std::unordered_set<Entity> alive_;
    ComponentStore<Position> positions_;
    ComponentStore<Velocity> velocities_;
    ComponentStore<Health>   healths_;
    ComponentStore<Name>     names_;
    ComponentStore<Tag>      tags_;
};

// ── 출력 헬퍼 ────────────────────────────────────────────────────────────────
void print_entity(World& w, Entity e) {
    std::cout << "  Entity[" << e << "]";
    if (w.names().has(e))
        std::cout << " name=" << w.names().get(e).value;
    if (w.tags().has(e))
        std::cout << " tag=" << w.tags().get(e).label;
    if (w.positions().has(e)) {
        auto& p = w.positions().get(e);
        std::cout << std::fixed << std::setprecision(1)
                  << " pos=(" << p.x << "," << p.y << ")";
    }
    if (w.velocities().has(e)) {
        auto& v = w.velocities().get(e);
        std::cout << " vel=(" << v.dx << "," << v.dy << ")";
    }
    if (w.healths().has(e)) {
        auto& h = w.healths().get(e);
        std::cout << " hp=" << h.current << "/" << h.max;
    }
    std::cout << "\n";
}

// ── main ──────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "=== ECS 코어 과제 ===\n\n";

    World world;

    // 1. 엔티티 생성 및 컴포넌트 부착
    std::cout << "[1] 엔티티 생성\n";

    Entity player = world.create();
    world.add(player, Name{"영웅 아서"})
         .add(player, Tag{"player"})
         .add(player, Position{0, 0})
         .add(player, Velocity{1, 0})
         .add(player, Health{100, 100});

    Entity goblin = world.create();
    world.add(goblin, Name{"고블린"})
         .add(goblin, Tag{"enemy"})
         .add(goblin, Position{10, 5})
         .add(goblin, Velocity{-1, 0})
         .add(goblin, Health{30, 30});

    Entity chest = world.create();
    world.add(chest, Name{"보물 상자"})
         .add(chest, Tag{"item"})
         .add(chest, Position{5, 5});
    // chest는 Velocity 없음 — 움직이지 않음

    Entity orc = world.create();
    world.add(orc, Name{"오크"})
         .add(orc, Tag{"enemy"})
         .add(orc, Position{15, 0})
         .add(orc, Velocity{-2, 0})
         .add(orc, Health{60, 60});

    print_entity(world, player);
    print_entity(world, goblin);
    print_entity(world, chest);
    print_entity(world, orc);

    // 2. 컴포넌트 유무 확인
    std::cout << "\n[2] 컴포넌트 유무 확인\n";
    std::cout << "  chest has Velocity: " << world.velocities().has(chest) << "\n";
    std::cout << "  chest has Position: " << world.positions().has(chest)  << "\n";
    std::cout << "  player has Health:  " << world.healths().has(player)   << "\n";

    // 3. 컴포넌트 수정
    std::cout << "\n[3] 컴포넌트 수정\n";
    world.positions().get(player).x += 3;
    world.healths().get(goblin).current -= 10;
    std::cout << "  player 이동 후 x = "
              << world.positions().get(player).x << "\n";
    std::cout << "  goblin 피격 후 hp = "
              << world.healths().get(goblin).current << "\n";

    // 4. 쿼리: Position + Velocity를 가진 엔티티
    std::cout << "\n[4] 쿼리: Position + Velocity 보유 엔티티\n";
    for (Entity e : world.query_pos_vel()) {
        std::string nm = world.names().has(e) ? world.names().get(e).value : "?";
        std::cout << "  " << nm << " (id=" << e << ")\n";
    }

    // 5. 쿼리: Position + Health를 가진 엔티티
    std::cout << "\n[5] 쿼리: Position + Health 보유 엔티티 (전투 가능)\n";
    for (Entity e : world.query_pos_health()) {
        std::string nm = world.names().has(e) ? world.names().get(e).value : "?";
        auto& h = world.healths().get(e);
        std::cout << "  " << nm << " hp=" << h.current << "/" << h.max << "\n";
    }

    // 6. 엔티티 삭제
    std::cout << "\n[6] 엔티티 삭제 (goblin)\n";
    world.destroy(goblin);
    std::cout << "  삭제 후 goblin has Health: "
              << world.healths().has(goblin) << "\n";
    std::cout << "  삭제 후 query_pos_health 수: "
              << world.query_pos_health().size() << "\n";

    return 0;
}
