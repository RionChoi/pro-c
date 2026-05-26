// 세션 31 게임: 던전 탐험가 — std::variant 상태 머신
// 게임 상태(MainMenu / Exploring / InCombat / GameOver)를 variant로 표현하고
// std::visit으로 전환한다.

#include <algorithm>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <variant>

static std::mt19937 rng{std::random_device{}()};

static int randInt(int lo, int hi) {
    return std::uniform_int_distribution<int>{lo, hi}(rng);
}

// ── 게임 상태 타입 ─────────────────────────────────────────────────────────
struct MainMenu {};

struct Exploring {
    int floor{1};
    int hp{30};
    int atk{5};
};

struct InCombat {
    int  floor;
    int  playerHP;
    int  playerAtk;
    std::string enemyName;
    int  enemyHP;
    int  enemyMaxHP;
    int  enemyAtk;
};

struct GameOver {
    bool won;
    int  floorsCleared;
};

using GameState = std::variant<MainMenu, Exploring, InCombat, GameOver>;

// ── 입력 유틸 ──────────────────────────────────────────────────────────────
static void clearInput() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static int getChoice(int lo, int hi) {
    int choice{};
    while (true) {
        std::cout << "선택 (" << lo << "-" << hi << "): ";
        if (std::cin >> choice && choice >= lo && choice <= hi) {
            clearInput();
            return choice;
        }
        std::cin.clear();
        clearInput();
        std::cout << "잘못된 입력입니다.\n";
    }
}

// ── 상태 전이 함수 ─────────────────────────────────────────────────────────
static GameState handleMainMenu(const MainMenu&) {
    std::cout << "\n+==============================+\n";
    std::cout << "|      던전 탐험가 v1.0        |\n";
    std::cout << "+==============================+\n";
    std::cout << "1. 게임 시작\n";
    std::cout << "2. 종료\n";
    if (getChoice(1, 2) == 2)
        return GameOver{false, 0};
    std::cout << "\n모험을 시작합니다! HP=30, ATK=5\n";
    std::cout << "목표: 5층을 클리어하라!\n";
    return Exploring{};
}

static GameState handleExploring(const Exploring& e) {
    std::cout << "\n-- " << e.floor << "층 탐험 중 "
              << " | HP: " << e.hp << " | ATK: " << e.atk << " --\n";
    std::cout << "1. 앞으로 전진\n";
    std::cout << "2. 포기하고 돌아가기\n";
    if (getChoice(1, 2) == 2)
        return GameOver{false, e.floor - 1};

    if (randInt(1, 3) <= 2) {
        static const char* names[] = {
            "슬라임", "고블린", "오크", "스켈레톤", "드래곤"
        };
        int idx     = std::min(e.floor - 1, 4);
        int enmHP   = 8  + e.floor * 4;
        int enmAtk  = 2  + e.floor;
        std::cout << "몬스터 조우: " << names[idx] << "!\n";
        return InCombat{e.floor, e.hp, e.atk,
                        names[idx], enmHP, enmHP, enmAtk};
    }

    int heal  = randInt(3, 8);
    int newHP = e.hp + heal;
    std::cout << "회복 포션 발견! +" << heal << " HP -> " << newHP << "\n";
    return Exploring{e.floor, newHP, e.atk};
}

static GameState handleCombat(const InCombat& c) {
    std::cout << "\n[전투] " << c.enemyName
              << " HP: " << c.enemyHP << "/" << c.enemyMaxHP
              << "  |  나의 HP: " << c.playerHP << "\n";
    std::cout << "1. 공격\n";
    std::cout << "2. 도망\n";

    if (getChoice(1, 2) == 2) {
        std::cout << "도망쳤습니다!\n";
        return Exploring{c.floor, c.playerHP, c.playerAtk};
    }

    int dmg        = c.playerAtk + randInt(0, 2);
    int newEnemyHP = c.enemyHP - dmg;
    std::cout << c.enemyName << "에게 " << dmg << " 데미지!\n";

    if (newEnemyHP <= 0) {
        int bonus  = randInt(1, 2);
        int newAtk = c.playerAtk + bonus;
        std::cout << c.enemyName << " 처치! ATK +" << bonus
                  << " -> " << newAtk << "\n";
        if (c.floor >= 5)
            return GameOver{true, c.floor};
        return Exploring{c.floor + 1, c.playerHP, newAtk};
    }

    int eDmg      = c.enemyAtk + randInt(0, 1);
    int newPlrHP  = c.playerHP - eDmg;
    std::cout << c.enemyName << "의 반격! " << eDmg << " 데미지 받음.\n";

    if (newPlrHP <= 0) {
        std::cout << "쓰러졌습니다...\n";
        return GameOver{false, c.floor - 1};
    }

    return InCombat{c.floor, newPlrHP, c.playerAtk,
                    c.enemyName, newEnemyHP, c.enemyMaxHP, c.enemyAtk};
}

static void handleGameOver(const GameOver& g) {
    std::cout << "\n+==============================+\n";
    if (g.won)
        std::cout << "|   축하합니다! 던전 클리어!   |\n";
    else
        std::cout << "|        게임 오버...          |\n";
    std::cout << "+==============================+\n";
    std::cout << "클리어한 층수: " << g.floorsCleared << "층\n";
}

int main() {
    GameState state = MainMenu{};
    bool running    = true;

    while (running) {
        state = std::visit([&running](const auto& s) -> GameState {
            using T = std::decay_t<decltype(s)>;
            if constexpr (std::is_same_v<T, MainMenu>)
                return handleMainMenu(s);
            else if constexpr (std::is_same_v<T, Exploring>)
                return handleExploring(s);
            else if constexpr (std::is_same_v<T, InCombat>)
                return handleCombat(s);
            else {
                handleGameOver(s);
                running = false;
                return s;
            }
        }, state);
    }

    return 0;
}
