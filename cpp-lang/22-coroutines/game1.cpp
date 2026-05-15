// 세션 22 — 미니 게임: 던전 탐험
// Generator<Room>으로 방을 지연 생성하고, 플레이어가 한 방씩 탐험한다.
// 적용 개념: co_yield, Generator, 랜덤 방 생성, 코루틴 상태 보존
#include <iostream>
#include <coroutine>
#include <string>
#include <vector>
#include <random>
#include <limits>
#include <optional>
#include <chrono>

// ── Generator<T> ─────────────────────────────────────────────────────────────
template <typename T>
struct Generator {
    struct promise_type {
        T current;

        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend()   noexcept { return {}; }
        std::suspend_always yield_value(T v) noexcept {
            current = std::move(v);
            return {};
        }
        void return_void() noexcept {}
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> handle;

    explicit Generator(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~Generator() { if (handle) handle.destroy(); }
    Generator(const Generator&)            = delete;
    Generator& operator=(const Generator&) = delete;
    Generator(Generator&& o) noexcept : handle(o.handle) { o.handle = nullptr; }

    bool next()    { handle.resume(); return !handle.done(); }
    T&   value()   { return handle.promise().current; }
};

// ── 방 데이터 ────────────────────────────────────────────────────────────────
struct Room {
    int         number;
    std::string name;
    std::string description;
    int         monster_hp;   // 0 = 몬스터 없음
    std::string monster_name;
    int         treasure;     // 0 = 보물 없음
    bool        is_exit;
};

// ── 던전 방 Generator ─────────────────────────────────────────────────────────
Generator<Room> dungeon_generator(int seed, int total_rooms) {
    std::mt19937 rng(static_cast<unsigned>(seed));

    const std::vector<std::string> room_names = {
        "어둠의 복도", "고대 도서관", "보물 창고", "감옥 지하실",
        "마법사의 방", "무기고", "신전 전실", "지하 연못가"
    };
    const std::vector<std::string> monster_names = {
        "해골 기사", "독거미", "어둠 정령", "고블린 대장", "석상 골렘"
    };

    std::uniform_int_distribution<int> roomDist(0, static_cast<int>(room_names.size()) - 1);
    std::uniform_int_distribution<int> monDist(0, static_cast<int>(monster_names.size()) - 1);
    std::uniform_int_distribution<int> hpDist(10, 40);
    std::uniform_int_distribution<int> goldDist(5, 50);
    std::uniform_int_distribution<int> chanceDist(0, 99);

    for (int i = 1; i <= total_rooms; ++i) {
        Room r{};
        r.number      = i;
        r.name        = room_names[static_cast<std::size_t>(roomDist(rng))];
        r.is_exit     = (i == total_rooms);

        bool has_monster  = chanceDist(rng) < 60;  // 60% 확률
        bool has_treasure = chanceDist(rng) < 40;  // 40% 확률

        if (has_monster) {
            r.monster_name = monster_names[static_cast<std::size_t>(monDist(rng))];
            r.monster_hp   = hpDist(rng);
            r.description  = "으스스한 분위기 속에 적이 도사리고 있다.";
        } else if (has_treasure) {
            r.treasure    = goldDist(rng);
            r.description = "반짝이는 보물이 구석에 놓여 있다.";
        } else {
            r.description = "텅 빈 방이다. 먼지만 날린다.";
        }

        if (r.is_exit)
            r.description = "🚪 출구가 보인다!";

        co_yield r;
    }
}

// ── 입력 유틸리티 ─────────────────────────────────────────────────────────────
static int inputInt(const std::string& prompt) {
    int v;
    std::cout << prompt;
    while (!(std::cin >> v)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  숫자를 입력하세요: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return v;
}

// ── 전투 ─────────────────────────────────────────────────────────────────────
// 반환값: 전투 후 플레이어 HP
int battle(int player_hp, const Room& room) {
    std::cout << "\n  ⚔️  " << room.monster_name << " 출현! (HP: " << room.monster_hp << ")\n";

    int monster_hp = room.monster_hp;
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> atkDist(8, 18);
    std::uniform_int_distribution<int> monAtkDist(5, 12);

    while (player_hp > 0 && monster_hp > 0) {
        std::cout << "  [플레이어 HP: " << player_hp
                  << " | 몬스터 HP: " << monster_hp << "]\n";
        std::cout << "  1. 공격  2. 도망\n";
        int choice = inputInt("  선택: ");

        if (choice == 2) {
            std::cout << "  💨 도망쳤다!\n";
            return player_hp;
        }

        int atk = atkDist(rng);
        monster_hp -= atk;
        std::cout << "  🗡  플레이어 공격: -" << atk << " (몬스터 HP: "
                  << std::max(0, monster_hp) << ")\n";

        if (monster_hp <= 0) {
            std::cout << "  ✅ " << room.monster_name << " 처치!\n";
            break;
        }

        int matk = monAtkDist(rng);
        player_hp -= matk;
        std::cout << "  💥 몬스터 반격: -" << matk << " (플레이어 HP: "
                  << std::max(0, player_hp) << ")\n";
    }
    return player_hp;
}

// ── main ──────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "╔══════════════════════════════════════╗\n"
              << "║   🏰 코루틴 던전 탐험              ║\n"
              << "╚══════════════════════════════════════╝\n\n";

    std::cout << "던전 깊이를 선택하세요 (3~8): ";
    int depth = inputInt("");
    if (depth < 3) depth = 3;
    if (depth > 8) depth = 8;

    auto dungeon = dungeon_generator(
        static_cast<int>(std::chrono::steady_clock::now().time_since_epoch().count()),
        depth
    );

    int player_hp   = 100;
    int gold        = 0;
    int rooms_clear = 0;

    std::cout << "\n🧙 던전에 입장했다. (HP: " << player_hp << ")\n";

    while (dungeon.next()) {
        if (player_hp <= 0) break;

        const Room& room = dungeon.value();

        std::cout << "\n══════════════════════════════════════\n";
        std::cout << "  방 " << room.number << "/" << depth
                  << " — " << room.name << "\n";
        std::cout << "  " << room.description << "\n";

        if (room.is_exit) {
            std::cout << "\n  🎉 탈출 성공!\n";
            ++rooms_clear;
            break;
        }

        if (room.monster_hp > 0) {
            player_hp = battle(player_hp, room);
            if (player_hp <= 0) {
                std::cout << "\n  💀 플레이어 사망...\n";
                break;
            }
        } else if (room.treasure > 0) {
            std::cout << "  💰 금화 " << room.treasure << " 획득!\n";
            gold += room.treasure;
        }

        ++rooms_clear;

        if (!room.is_exit) {
            std::cout << "\n  다음 방으로 진행하시겠습니까? (1=예, 0=포기): ";
            int go = inputInt("");
            if (go == 0) {
                std::cout << "  🚶 던전을 빠져나왔다.\n";
                break;
            }
        }
    }

    // 결과
    std::cout << "\n══════════════════════════════════════\n";
    std::cout << "  ▶ 탐험 결과\n";
    std::cout << "  탐험한 방:  " << rooms_clear << " / " << depth << "\n";
    std::cout << "  최종 HP:    " << std::max(0, player_hp) << "\n";
    std::cout << "  획득 금화:  " << gold << "\n";

    if (player_hp > 0 && rooms_clear == depth)
        std::cout << "  🏆 완전 클리어!\n";
    else if (player_hp > 0)
        std::cout << "  🌟 부분 탈출.\n";
    else
        std::cout << "  💀 던전에서 쓰러졌다...\n";
    std::cout << "══════════════════════════════════════\n";

    return 0;
}
