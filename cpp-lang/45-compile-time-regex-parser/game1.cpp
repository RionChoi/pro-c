// Session 45: 컴파일 타임 정규 표현식 & 파서 - 명령어 검증 미니 게임
#include <array>
#include <iostream>
#include <string_view>

constexpr bool is_digit(char ch) {
    return ch >= '0' && ch <= '9';
}

constexpr bool is_direction(std::string_view text) {
    return text == "north" || text == "south" || text == "east" || text == "west";
}

constexpr bool is_move_command(std::string_view text) {
    constexpr std::string_view prefix = "move ";
    if (!text.starts_with(prefix)) {
        return false;
    }
    return is_direction(text.substr(prefix.size()));
}

constexpr bool is_wait_command(std::string_view text) {
    constexpr std::string_view prefix = "wait ";
    if (!text.starts_with(prefix) || text.size() == prefix.size()) {
        return false;
    }
    for (char ch : text.substr(prefix.size())) {
        if (!is_digit(ch)) {
            return false;
        }
    }
    return true;
}

constexpr bool is_command(std::string_view text) {
    return text == "scan" || is_move_command(text) || is_wait_command(text);
}

static_assert(is_command("scan"));
static_assert(is_command("move north"));
static_assert(is_command("wait 3"));
static_assert(!is_command("move up"));
static_assert(!is_command("wait soon"));

int main() {
    constexpr std::array<std::string_view, 6> script{
        "scan",
        "move north",
        "wait 2",
        "move east",
        "move up",
        "wait soon"
    };

    int accepted = 0;
    std::cout << "=== command validator ===\n";
    for (std::string_view command : script) {
        const bool ok = is_command(command);
        accepted += ok ? 1 : 0;
        std::cout << command << " -> " << (ok ? "accepted" : "rejected") << '\n';
    }
    std::cout << "accepted commands: " << accepted << '/' << script.size() << '\n';
}
