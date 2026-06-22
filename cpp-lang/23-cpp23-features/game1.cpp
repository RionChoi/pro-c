// 세션 23 — 미니 게임: 스킬 체인 RPG
// std::expected로 스킬/행동 결과를 처리한다.
// 성공 시 and_then 체인으로 연속 행동, 실패 시 or_else로 복구
#include <iostream>
#include <expected>
#include <string>
#include <vector>
#include <random>
#include <limits>
#include <ranges>
#include <iomanip>

// ── 오류 타입 ────────────────────────────────────────────────────────────────
enum class ActionError {
    NotEnoughMana,
    TargetDead,
    SkillMissed,
    HeroDefeated,
};

std::string to_string(ActionError e) {
    switch (e) {
    case ActionError::NotEnoughMana: return "마나 부족";
    case ActionError::TargetDead:    return "대상이 이미 사망";
    case ActionError::SkillMissed:   return "스킬 빗나감";
    case ActionError::HeroDefeated:  return "영웅 사망";
    }
    return "알 수 없음";
}

// ── 게임 상태 ────────────────────────────────────────────────────────────────
struct Hero {
    std::string name;
    int hp    = 100;
    int mana  = 50;
    int level = 1;

    bool alive() const { return hp > 0; }
    void print() const {
        std::cout << "  [" << name << "] HP:" << hp
                  << " MP:" << mana << " Lv:" << level << "\n";
    }
};

struct Enemy {
    std::string name;
    int hp    = 0;
    int maxHp = 0;

    bool alive() const { return hp > 0; }
    void print() const {
        std::cout << "  [" << name << "] HP:" << hp << "/" << maxHp << "\n";
    }
};

// ── 전투 결과 ────────────────────────────────────────────────────────────────
struct DamageResult {
    int damage;
    std::string message;
};

// ── 스킬 함수들 (expected 반환) ───────────────────────────────────────────────
std::mt19937 rng(std::random_device{}());

std::expected<DamageResult, ActionError>
normal_attack(Hero& /*hero*/, Enemy& enemy) {
    if (!enemy.alive()) return std::unexpected(ActionError::TargetDead);

    std::uniform_int_distribution<int> hitDist(0, 99);
    if (hitDist(rng) < 15)  // 15% 빗나감
        return std::unexpected(ActionError::SkillMissed);

    std::uniform_int_distribution<int> dmgDist(8, 15);
    int dmg = dmgDist(rng);
    enemy.hp -= dmg;
    if (enemy.hp < 0) enemy.hp = 0;
    return DamageResult{dmg, "기본 공격"};
}

std::expected<DamageResult, ActionError>
fire_bolt(Hero& hero, Enemy& enemy) {
    if (!enemy.alive()) return std::unexpected(ActionError::TargetDead);
    if (hero.mana < 10) return std::unexpected(ActionError::NotEnoughMana);

    hero.mana -= 10;
    std::uniform_int_distribution<int> dmgDist(20, 35);
    int dmg = dmgDist(rng);
    enemy.hp -= dmg;
    if (enemy.hp < 0) enemy.hp = 0;
    return DamageResult{dmg, "화염 볼트"};
}

std::expected<DamageResult, ActionError>
thunder_strike(Hero& hero, Enemy& enemy) {
    if (!enemy.alive()) return std::unexpected(ActionError::TargetDead);
    if (hero.mana < 20) return std::unexpected(ActionError::NotEnoughMana);

    std::uniform_int_distribution<int> hitDist(0, 99);
    if (hitDist(rng) < 30) {  // 30% 빗나감
        hero.mana -= 5;       // 마나는 절반만 소모
        return std::unexpected(ActionError::SkillMissed);
    }

    hero.mana -= 20;
    std::uniform_int_distribution<int> dmgDist(40, 60);
    int dmg = dmgDist(rng);
    enemy.hp -= dmg;
    if (enemy.hp < 0) enemy.hp = 0;
    return DamageResult{dmg, "번개 강타"};
}

// ── 결과 출력 ────────────────────────────────────────────────────────────────
void print_action(const std::string& skill,
                  const std::expected<DamageResult, ActionError>& r) {
    if (r) {
        std::cout << "  ✅ [" << r->message << "] 피해: " << r->damage << "\n";
    } else {
        std::cout << "  ❌ [" << skill << "] 실패 — " << to_string(r.error()) << "\n";
    }
}

// ── 입력 ─────────────────────────────────────────────────────────────────────
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

// ── 적 목록 ──────────────────────────────────────────────────────────────────
std::vector<Enemy> make_enemies() {
    return {
        {"고블린",   30,  30},
        {"오크",     60,  60},
        {"드래곤", 120, 120},
    };
}

// ── 전투 메뉴 ────────────────────────────────────────────────────────────────
void print_battle_menu() {
    std::cout << "\n  ── 행동 선택 ──────────────────\n"
              << "  1. 기본 공격   (MP 0)\n"
              << "  2. 화염 볼트   (MP 10)\n"
              << "  3. 번개 강타   (MP 20, 빗나감 30%)\n"
              << "  0. 도망\n"
              << "  선택: ";
}

// ── main ─────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "╔══════════════════════════════════════╗\n"
              << "║   ⚔️  스킬 체인 RPG (C++23 expected) ║\n"
              << "╚══════════════════════════════════════╝\n\n";

    Hero hero{"영웅", 100, 50, 1};
    auto enemies = make_enemies();

    int total_dmg = 0;
    int kills     = 0;

    for (auto& enemy : enemies) {
        if (!hero.alive()) break;

        std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << "  적 등장: " << enemy.name << "\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";

        while (hero.alive() && enemy.alive()) {
            hero.print();
            enemy.print();
            print_battle_menu();

            int choice = inputInt("");

            if (choice == 0) {
                std::cout << "  💨 도망쳤다!\n";
                goto next_enemy;
            }

            std::expected<DamageResult, ActionError> result =
                std::unexpected(ActionError::SkillMissed);

            if (choice == 1) {
                result = normal_attack(hero, enemy);
            } else if (choice == 2) {
                // and_then: fire_bolt 성공 시 추가 효과
                result = fire_bolt(hero, enemy)
                    .and_then([&](DamageResult r) -> std::expected<DamageResult, ActionError> {
                        // 적이 아직 살아있으면 화상 추가 피해 (5)
                        if (enemy.alive()) {
                            enemy.hp -= 5;
                            if (enemy.hp < 0) enemy.hp = 0;
                            r.damage += 5;
                            r.message += " (화상 +5)";
                        }
                        return r;
                    });
            } else if (choice == 3) {
                // or_else: 번개 실패 시 기본 공격으로 폴백
                result = thunder_strike(hero, enemy)
                    .or_else([&](ActionError e) -> std::expected<DamageResult, ActionError> {
                        if (e == ActionError::SkillMissed) {
                            std::cout << "  ↩️  번개 빗나감 → 기본 공격으로 전환\n";
                            return normal_attack(hero, enemy);
                        }
                        return std::unexpected(e);
                    });
            } else {
                std::cout << "  올바른 번호를 입력하세요.\n";
                continue;
            }

            print_action("스킬", result);
            if (result) total_dmg += result->damage;

            // 적 반격
            if (enemy.alive()) {
                std::uniform_int_distribution<int> eDmgDist(5, 15);
                int edm = eDmgDist(rng);
                hero.hp -= edm;
                if (hero.hp < 0) hero.hp = 0;
                std::cout << "  💥 " << enemy.name << " 반격: -" << edm << "\n";
            }

            if (!enemy.alive()) {
                std::cout << "  🏆 " << enemy.name << " 처치!\n";
                ++kills;
                hero.mana = std::min(hero.mana + 15, 50);  // 마나 회복
                std::cout << "  💫 마나 +15 회복\n";
            }
        }
        next_enemy:;
    }

    // 결과
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "  ▶ 모험 결과\n";
    std::cout << "  처치한 적:   " << kills << " / " << enemies.size() << "\n";
    std::cout << "  총 피해:     " << total_dmg << "\n";
    std::cout << "  최종 HP:     " << std::max(0, hero.hp) << "\n";

    if (kills == static_cast<int>(enemies.size()))
        std::cout << "  🏆 모든 적을 처치했다! 전설의 영웅!\n";
    else if (kills > 0)
        std::cout << "  🌟 " << kills << "마리 처치. 다음엔 더 강해지자!\n";
    else
        std::cout << "  😢 전투 없이 끝났다...\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";

    return 0;
}
