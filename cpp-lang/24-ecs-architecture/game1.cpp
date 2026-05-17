// 세션 24 — 미니 게임: ECS 전투 시뮬레이터
// 플레이어가 ECS로 구성된 파티를 이끌고 적들을 물리친다.
// Component: Position, Health, Attack, Name, Tag
// System: TurnSystem, AISystem, CombatSystem, RenderSystem
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <limits>
#include <iomanip>

// ── Entity ────────────────────────────────────────────────────────────────────
using Entity = uint32_t;

// ── Components ────────────────────────────────────────────────────────────────
struct Name    { std::string value; };
struct Tag     { std::string label; };   // "player", "enemy"
struct Health  { int hp; int maxHp; };
struct Attack  { int minDmg; int maxDmg; int hitChance; };  // hitChance: 0~100
struct Defense { int armor; };           // 피해 감소
struct Speed   { int value; };           // 행동 순서

// ── ComponentStore<T> ─────────────────────────────────────────────────────────
template <typename T>
struct Store {
    std::unordered_map<Entity, T> data;
    void add(Entity e, T c)          { data[e] = std::move(c); }
    void remove(Entity e)            { data.erase(e); }
    bool has(Entity e) const         { return data.count(e) > 0; }
    T&       get(Entity e)       { return data.at(e); }
    const T& get(Entity e) const { return data.at(e); }
};

// ── Registry ──────────────────────────────────────────────────────────────────
struct Registry {
    Entity next = 0;
    std::vector<Entity> entities;

    Store<Name>    names;
    Store<Tag>     tags;
    Store<Health>  healths;
    Store<Attack>  attacks;
    Store<Defense> defenses;
    Store<Speed>   speeds;

    Entity create() {
        Entity e = ++next;
        entities.push_back(e);
        return e;
    }
    void destroy(Entity e) {
        entities.erase(std::remove(entities.begin(), entities.end(), e), entities.end());
        names.remove(e); tags.remove(e); healths.remove(e);
        attacks.remove(e); defenses.remove(e); speeds.remove(e);
    }
    bool alive(Entity e) const {
        return std::find(entities.begin(), entities.end(), e) != entities.end();
    }
    std::vector<Entity> by_tag(const std::string& t) const {
        std::vector<Entity> result;
        for (Entity e : entities)
            if (tags.has(e) && tags.get(e).label == t)
                result.push_back(e);
        return result;
    }
};

// ── 유틸리티 ──────────────────────────────────────────────────────────────────
std::mt19937 rng(std::random_device{}());

static int inputInt(const std::string& prompt) {
    int v;
    std::cout << prompt;
    while (!(std::cin >> v)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  숫자 입력: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return v;
}

// ── RenderSystem: 전투 상태 출력 ─────────────────────────────────────────────
void render(const Registry& reg) {
    std::cout << "\n  ── 파티 ──────────────────────────────\n";
    for (Entity e : reg.by_tag("player")) {
        auto& h = reg.healths.get(e);
        int bar = h.maxHp > 0 ? (h.hp * 20 / h.maxHp) : 0;
        std::cout << "  [" << reg.names.get(e).value << "] HP:"
                  << std::setw(3) << h.hp << "/" << h.maxHp
                  << " [" << std::string(bar, '#')
                  << std::string(20 - bar, '.') << "]\n";
    }
    std::cout << "  ── 적 ────────────────────────────────\n";
    for (Entity e : reg.by_tag("enemy")) {
        auto& h = reg.healths.get(e);
        int bar = h.maxHp > 0 ? (h.hp * 20 / h.maxHp) : 0;
        std::cout << "  [" << std::setw(2) << e << "] "
                  << std::setw(10) << std::left << reg.names.get(e).value
                  << " HP:" << std::setw(3) << h.hp << "/" << h.maxHp
                  << " [" << std::string(bar, '#')
                  << std::string(20 - bar, '.') << "]\n";
    }
    std::cout << "  ─────────────────────────────────────\n";
}

// ── CombatSystem: 공격 처리 ───────────────────────────────────────────────────
void do_attack(Registry& reg, Entity attacker, Entity target) {
    auto& atk = reg.attacks.get(attacker);
    auto& atkName = reg.names.get(attacker).value;
    auto& defName = reg.names.get(target).value;

    std::uniform_int_distribution<int> hitDist(0, 99);
    if (hitDist(rng) >= atk.hitChance) {
        std::cout << "  ❌ " << atkName << " → " << defName << " 빗나감!\n";
        return;
    }

    std::uniform_int_distribution<int> dmgDist(atk.minDmg, atk.maxDmg);
    int dmg = dmgDist(rng);

    if (reg.defenses.has(target)) {
        dmg = std::max(1, dmg - reg.defenses.get(target).armor);
    }

    reg.healths.get(target).hp -= dmg;
    std::cout << "  ⚔️  " << atkName << " → " << defName
              << " : -" << dmg << " HP\n";
}

// ── AISystem: 적 AI (가장 HP 낮은 플레이어 공격) ─────────────────────────────
void ai_turn(Registry& reg, Entity enemy) {
    auto players = reg.by_tag("player");
    if (players.empty()) return;

    Entity target = players[0];
    int    minHp  = reg.healths.get(players[0]).hp;
    for (Entity p : players) {
        int hp = reg.healths.get(p).hp;
        if (hp < minHp) { minHp = hp; target = p; }
    }
    do_attack(reg, enemy, target);
}

// ── DeathSystem: HP <= 0 제거 ────────────────────────────────────────────────
void death_check(Registry& reg) {
    std::vector<Entity> dead;
    for (Entity e : reg.entities)
        if (reg.healths.has(e) && reg.healths.get(e).hp <= 0)
            dead.push_back(e);
    for (Entity e : dead) {
        std::cout << "  💀 " << reg.names.get(e).value << " 쓰러짐!\n";
        reg.destroy(e);
    }
}

// ── 팩토리 함수 ───────────────────────────────────────────────────────────────
Entity make_hero(Registry& reg, const std::string& name,
                 int hp, int minD, int maxD, int armor, int spd) {
    Entity e = reg.create();
    reg.names.add(e,   {name});
    reg.tags.add(e,    {"player"});
    reg.healths.add(e, {hp, hp});
    reg.attacks.add(e, {minD, maxD, 85});
    reg.defenses.add(e,{armor});
    reg.speeds.add(e,  {spd});
    return e;
}

Entity make_enemy(Registry& reg, const std::string& name,
                  int hp, int minD, int maxD, int armor) {
    Entity e = reg.create();
    reg.names.add(e,   {name});
    reg.tags.add(e,    {"enemy"});
    reg.healths.add(e, {hp, hp});
    reg.attacks.add(e, {minD, maxD, 70});
    reg.defenses.add(e,{armor});
    reg.speeds.add(e,  {5});
    return e;
}

// ── main ──────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "╔══════════════════════════════════════╗\n"
              << "║   ⚔️  ECS 전투 시뮬레이터            ║\n"
              << "╚══════════════════════════════════════╝\n\n";

    Registry reg;

    // 파티 생성
    make_hero(reg, "전사",  120, 15, 25, 5, 8);
    make_hero(reg, "마법사",  70, 25, 40, 1, 6);
    make_hero(reg, "궁수",    80, 18, 28, 2, 9);

    // 적 생성
    make_enemy(reg, "고블린",  40, 8, 14, 1);
    make_enemy(reg, "오크",    80, 12, 20, 3);
    make_enemy(reg, "트롤",   120, 18, 28, 5);

    int round = 0;

    while (!reg.by_tag("player").empty() && !reg.by_tag("enemy").empty()) {
        ++round;
        std::cout << "\n══ 라운드 " << round << " ══════════════════════════\n";
        render(reg);

        // 행동 순서 계산 (Speed 높은 순)
        std::vector<Entity> turnOrder = reg.entities;
        std::sort(turnOrder.begin(), turnOrder.end(), [&](Entity a, Entity b) {
            int sa = reg.speeds.has(a) ? reg.speeds.get(a).value : 0;
            int sb = reg.speeds.has(b) ? reg.speeds.get(b).value : 0;
            return sa > sb;
        });

        for (Entity e : turnOrder) {
            if (!reg.alive(e)) continue;
            if (!reg.attacks.has(e)) continue;

            if (reg.tags.get(e).label == "player") {
                // 플레이어 턴: 공격 대상 선택
                auto enemies = reg.by_tag("enemy");
                if (enemies.empty()) break;

                std::cout << "\n  [" << reg.names.get(e).value << "] 행동\n";
                std::cout << "  공격 대상 선택:\n";
                for (std::size_t i = 0; i < enemies.size(); ++i) {
                    Entity en = enemies[i];
                    std::cout << "    " << (i + 1) << ". "
                              << reg.names.get(en).value
                              << " (HP:" << reg.healths.get(en).hp << ")\n";
                }

                int choice = inputInt("  번호: ");
                if (choice < 1 || choice > static_cast<int>(enemies.size()))
                    choice = 1;

                do_attack(reg, e, enemies[static_cast<std::size_t>(choice - 1)]);

            } else {
                // 적 AI 턴
                if (reg.by_tag("player").empty()) break;
                std::cout << "\n  [" << reg.names.get(e).value << "] 행동\n";
                ai_turn(reg, e);
            }

            death_check(reg);
        }
    }

    // 결과
    std::cout << "\n══════════════════════════════════════\n";
    if (!reg.by_tag("player").empty()) {
        std::cout << "  🏆 승리! (" << round << " 라운드)\n";
        std::cout << "  생존 파티원:\n";
        for (Entity e : reg.by_tag("player")) {
            auto& h = reg.healths.get(e);
            std::cout << "    " << reg.names.get(e).value
                      << " HP:" << h.hp << "/" << h.maxHp << "\n";
        }
    } else {
        std::cout << "  💀 전멸... (" << round << " 라운드)\n";
    }
    std::cout << "══════════════════════════════════════\n";

    return 0;
}
