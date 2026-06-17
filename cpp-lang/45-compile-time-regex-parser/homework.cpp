// Session 45: 컴파일 타임 정규 표현식 & 파서 - constexpr FSM
#include <array>
#include <iostream>
#include <string_view>

constexpr bool is_digit(char ch) {
    return ch >= '0' && ch <= '9';
}

constexpr bool is_alpha(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

constexpr bool is_identifier(std::string_view text) {
    if (text.empty() || !(is_alpha(text[0]) || text[0] == '_')) {
        return false;
    }
    for (char ch : text.substr(1)) {
        if (!(is_alpha(ch) || is_digit(ch) || ch == '_')) {
            return false;
        }
    }
    return true;
}

constexpr bool is_date(std::string_view text) {
    if (text.size() != 10) {
        return false;
    }
    return is_digit(text[0]) && is_digit(text[1])
        && is_digit(text[2]) && is_digit(text[3])
        && text[4] == '-'
        && is_digit(text[5]) && is_digit(text[6])
        && text[7] == '-'
        && is_digit(text[8]) && is_digit(text[9]);
}

constexpr bool is_hex_color(std::string_view text) {
    auto is_hex = [](char ch) {
        return is_digit(ch)
            || (ch >= 'a' && ch <= 'f')
            || (ch >= 'A' && ch <= 'F');
    };

    if (text.size() != 7 || text[0] != '#') {
        return false;
    }
    for (char ch : text.substr(1)) {
        if (!is_hex(ch)) {
            return false;
        }
    }
    return true;
}

static_assert(is_identifier("_score42"));
static_assert(!is_identifier("42score"));
static_assert(is_date("2026-06-17"));
static_assert(!is_date("2026/06/17"));
static_assert(is_hex_color("#3A7f10"));
static_assert(!is_hex_color("#3A7f1Z"));

int main() {
    constexpr std::array<std::string_view, 5> samples{
        "_score42", "42score", "2026-06-17", "#3A7f10", "#3A7f1Z"
    };

    for (std::string_view sample : samples) {
        std::cout << sample
                  << " identifier=" << is_identifier(sample)
                  << " date=" << is_date(sample)
                  << " color=" << is_hex_color(sample)
                  << '\n';
    }
}
