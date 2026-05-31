// game1.cpp — 메모리 던전
// 각 던전 층이 독립적인 monotonic_buffer_resource 아레나를 소유한다.
// 층을 클리어하면 DungeonFloor 객체가 소멸되어 아레나 전체가 O(1)로 해제된다.
// 플레이어는 아이템을 수거하며 골드를 쌓는다.

#include <iostream>
#include <memory_resource>
#include <vector>
#include <string>
#include <array>
#include <string_view>
#include <limits>
#include <cstddef>

// 아이템: PMR string 과 int 값으로 구성
struct Item {
    std::pmr::string name;
    int              gold;

    Item(std::string_view n, int g, std::pmr::memory_resource* res)
        : name(n, res), gold(g) {}

    // 이 타입은 move 전용으로 사용 (arena 내 string 소유권 보존)
    Item(const Item&)            = delete;
    Item& operator=(const Item&) = delete;
    Item(Item&&)                 = default;
    Item& operator=(Item&&)      = default;
};

// 던전 한 층 — 아레나 소유, 아이템/몬스터를 PMR 컨테이너로 관리
class DungeonFloor {
public:
    static constexpr std::size_t ARENA_BYTES = 1024;

    explicit DungeonFloor(int floor_num)
        : floor_(floor_num)
        , buf_{}
        , arena_(buf_.data(), ARENA_BYTES, std::pmr::null_memory_resource())
        , items_(&arena_)
        , monster_("", &arena_)
    {
        populate();
    }

    // 아레나 내 포인터를 보유하므로 복사·이동 모두 금지
    DungeonFloor(const DungeonFloor&)            = delete;
    DungeonFloor& operator=(const DungeonFloor&) = delete;
    DungeonFloor(DungeonFloor&&)                 = delete;
    DungeonFloor& operator=(DungeonFloor&&)      = delete;

    int  floor_number() const noexcept { return floor_; }
    bool has_items()    const noexcept { return !items_.empty(); }

    const std::pmr::string& monster_name() const noexcept { return monster_; }

    void show_items() const {
        if (items_.empty()) {
            std::cout << "  (아이템 없음)\n";
            return;
        }
        for (std::size_t i = 0; i < items_.size(); ++i)
            std::cout << "  [" << i + 1 << "] "
                      << items_[i].name << "  (" << items_[i].gold << " 골드)\n";
    }

    int collect_all() {
        int total = 0;
        for (const auto& it : items_) total += it.gold;
        items_.clear();
        return total;
    }

    static constexpr std::size_t arena_size() noexcept { return ARENA_BYTES; }

private:
    void populate() {
        struct Entry { std::string_view name; int gold; };

        static const Entry TABLE[5][4] = {
            {{"낡은 검",10},{"동전",5},{"빵",2},{"횃불",3}},
            {{"마법 지팡이",50},{"포션",20},{"실크 망토",40},{"금화",25}},
            {{"드래곤 비늘",200},{"보석",150},{"고대 유물",300},{"비밀 지도",80}},
            {{"신성한 검",500},{"천상의 갑옷",400},{"엘릭서",350},{"왕관",800}},
            {{"전설의 반지",1000},{"마스터 지팡이",900},{"고대 주문서",750},{"궁극의 갑옷",1200}},
        };
        static const char* const MONSTERS[5] = {
            "슬라임", "오크 전사", "드래곤", "천사 전사", "마신왕"
        };

        const std::size_t idx = static_cast<std::size_t>((floor_ - 1) % 5);
        monster_ = MONSTERS[idx];
        for (std::size_t i = 0; i < 4; ++i)
            items_.emplace_back(TABLE[idx][i].name, TABLE[idx][i].gold, &arena_);
    }

    int                                        floor_;
    std::array<std::byte, ARENA_BYTES>         buf_;
    std::pmr::monotonic_buffer_resource        arena_;
    std::pmr::vector<Item>                     items_;
    std::pmr::string                           monster_;
};

// ---- 게임 루프 -----------------------------------------------------------

static void clear_input() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static int prompt_choice(int lo, int hi) {
    int ch = 0;
    while (!(std::cin >> ch) || ch < lo || ch > hi) {
        clear_input();
        std::cout << lo << "~" << hi << " 중 선택: ";
    }
    clear_input();
    return ch;
}

int main() {
    std::cout << "================================================\n";
    std::cout << "    메모리 던전 — PMR 아레나 탐험 게임\n";
    std::cout << "================================================\n";
    std::cout << "각 층은 " << DungeonFloor::arena_size()
              << " 바이트 아레나를 독립적으로 소유합니다.\n";
    std::cout << "층을 벗어나면 아레나 전체가 즉시 해제됩니다.\n\n";

    int  gold    = 0;
    int  floor_n = 1;
    bool playing = true;

    while (playing) {
        std::cout << "════ " << floor_n << "층 ════\n";

        // 블록을 벗어나면 DungeonFloor 소멸 → arena_ 소멸 → buf_ 스택 해제
        {
            DungeonFloor floor(floor_n);

            std::cout << "몬스터: " << floor.monster_name() << "\n";
            std::cout << "발견된 아이템:\n";
            floor.show_items();

            std::cout << "\n[1] 아이템 전부 수거  [2] 그냥 통과  [3] 던전 탈출\n";
            std::cout << "선택: ";
            const int ch = prompt_choice(1, 3);

            if (ch == 1) {
                const int earned = floor.collect_all();
                gold += earned;
                std::cout << "+" << earned << " 골드 획득!  (누적 " << gold << " 골드)\n";
                std::cout << "[" << DungeonFloor::arena_size()
                          << " 바이트 아레나가 블록 탈출 시 소멸됩니다]\n";
                ++floor_n;
            } else if (ch == 2) {
                std::cout << "[아이템 없이 다음 층으로 이동]\n";
                ++floor_n;
            } else {
                playing = false;
            }
        } // ← DungeonFloor 소멸: monster_, items_, arena_, buf_ 순서로 정리
        std::cout << "\n";
    }

    std::cout << "══════════════════════════\n";
    std::cout << " 게임 종료!\n";
    std::cout << " 탐험 층수:  " << (floor_n - 1) << " 층\n";
    std::cout << " 획득 골드:  " << gold           << " G\n";
    std::cout << "══════════════════════════\n";

    return 0;
}
