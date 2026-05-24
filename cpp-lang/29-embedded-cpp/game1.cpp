// 신호등 상태 머신 시뮬레이터
// 임베디드 C++: constexpr 상태 테이블, 상태 머신, 터미널 UI
//
// 컴파일: g++ -std=c++23 -Wall -Wextra -Wpedantic game1.cpp -o game1

#include <array>
#include <cstdint>
#include <iostream>
#include <string_view>

// ─── 상태 정의 ────────────────────────────────────────────────────────────────

enum class State : uint8_t {
    Red,
    RedYellow,
    Green,
    Yellow,
    COUNT,
};

struct StateInfo {
    std::string_view name_ko;
    std::string_view lights;  // 신호등 표시 (R Y G)
    std::string_view color;   // ANSI 색상 코드
    uint8_t          duration; // 표시 단계 수
    State            next;
};

// 모든 상태 전이를 컴파일 타임에 정의
static constexpr std::array<StateInfo, 4> STATES{{
    {
        "빨간불",
        " [R]  o   o ",
        "\033[31m",  // 빨간색
        3,
        State::RedYellow,
    },
    {
        "빨간불+노란불",
        " [R] [Y]  o ",
        "\033[33m",  // 노란색
        1,
        State::Green,
    },
    {
        "초록불",
        "  o   o  [G]",
        "\033[32m",  // 초록색
        2,
        State::Yellow,
    },
    {
        "노란불",
        "  o  [Y]  o ",
        "\033[33m",  // 노란색
        1,
        State::Red,
    },
}};

static constexpr std::string_view RESET_COLOR = "\033[0m";

// ─── 신호등 표시 ──────────────────────────────────────────────────────────────

static void print_light(const StateInfo& s, int remaining) {
    std::cout << "\n";
    std::cout << "  ┌──────────────┐\n";
    std::cout << "  │  " << s.color << s.lights << RESET_COLOR << "  │\n";
    std::cout << "  └──────────────┘\n";
    std::cout << "  상태: " << s.color << s.name_ko << RESET_COLOR << "\n";
    std::cout << "  남은 단계: " << remaining << "\n";
}

static void print_stats(int total_steps, int cycles) {
    std::cout << "\n─────────────────────────\n";
    std::cout << "  총 진행 단계: " << total_steps << "\n";
    std::cout << "  완료 사이클:  " << cycles << "\n";
    std::cout << "─────────────────────────\n";
}

// ─── 신호등 컨트롤러 ─────────────────────────────────────────────────────────

class TrafficController {
    State   current_state_{State::Red};
    uint8_t steps_in_state_{0};
    int     total_steps_{0};
    int     cycles_{0};

    [[nodiscard]] const StateInfo& info() const noexcept {
        return STATES[static_cast<uint8_t>(current_state_)];
    }

public:
    void advance() noexcept {
        ++steps_in_state_;
        ++total_steps_;

        if (steps_in_state_ >= info().duration) {
            steps_in_state_ = 0;
            if (info().next == State::Red)
                ++cycles_;
            current_state_ = info().next;
        }
    }

    void display() const {
        const StateInfo& s = info();
        int remaining = static_cast<int>(s.duration) - static_cast<int>(steps_in_state_);
        print_light(s, remaining);
    }

    void show_stats() const { print_stats(total_steps_, cycles_); }

    [[nodiscard]] int total_steps() const noexcept { return total_steps_; }
    [[nodiscard]] int cycles()      const noexcept { return cycles_;      }
};

// ─── 메뉴 ────────────────────────────────────────────────────────────────────

static void print_menu() {
    std::cout << "\n  [Enter] 다음 단계  [a] 자동 진행  [q] 종료 > ";
}

static bool run_auto(TrafficController& ctrl, int steps) {
    for (int i = 0; i < steps; ++i) {
        std::cout << "\033[2J\033[H";  // 화면 지우기
        std::cout << "=== 신호등 시뮬레이터 (자동 모드) ===";
        ctrl.display();
        ctrl.show_stats();
        ctrl.advance();
        std::cout << "\n  [Enter] 정지 > ";
        std::string line;
        std::getline(std::cin, line);
        if (!line.empty()) break;
    }
    return false;
}

// ─── main ─────────────────────────────────────────────────────────────────────

int main() {
    std::cout << "=== 신호등 상태 머신 시뮬레이터 ===\n";
    std::cout << "임베디드 C++ 상태 머신 패턴 데모\n";
    std::cout << "\n신호등 표시: [R]=빨간불  [Y]=노란불  [G]=초록불  o=꺼짐\n";

    TrafficController ctrl;
    bool running = true;

    while (running) {
        std::cout << "\033[2J\033[H";  // 화면 지우기
        std::cout << "=== 신호등 시뮬레이터 ===";
        ctrl.display();
        ctrl.show_stats();
        print_menu();

        std::string line;
        if (!std::getline(std::cin, line)) break;

        if (line.empty()) {
            ctrl.advance();
        } else if (line == "a" || line == "A") {
            run_auto(ctrl, 20);
        } else if (line == "q" || line == "Q") {
            running = false;
        }
    }

    std::cout << "\n최종 결과:\n";
    ctrl.display();
    ctrl.show_stats();
    std::cout << "\n시뮬레이터 종료.\n";
    return 0;
}
