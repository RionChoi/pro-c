// Session 45: 컴파일 타임 정규 표현식 & 파서 - 작은 파서 컴비네이터
#include <charconv>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

struct ParseResult {
    bool ok;
    std::size_t next;
    int value;
    std::string_view error;
};

constexpr bool is_alpha(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

constexpr bool is_alnum(char ch) {
    return is_alpha(ch) || (ch >= '0' && ch <= '9');
}

constexpr ParseResult parse_identifier(std::string_view text, std::size_t pos) {
    if (pos >= text.size() || !is_alpha(text[pos])) {
        return {false, pos, 0, "expected identifier"};
    }
    ++pos;
    while (pos < text.size() && is_alnum(text[pos])) {
        ++pos;
    }
    return {true, pos, 0, {}};
}

ParseResult parse_int(std::string_view text, std::size_t pos) {
    int value = 0;
    const char* first = text.data() + pos;
    const char* last = text.data() + text.size();
    const auto [ptr, ec] = std::from_chars(first, last, value);
    if (ec != std::errc{}) {
        return {false, pos, 0, "expected integer"};
    }
    return {true, static_cast<std::size_t>(ptr - text.data()), value, {}};
}

ParseResult parse_assignment(std::string_view text) {
    auto key = parse_identifier(text, 0);
    if (!key.ok) {
        return key;
    }
    if (key.next >= text.size() || text[key.next] != '=') {
        return {false, key.next, 0, "expected '='"};
    }
    auto number = parse_int(text, key.next + 1);
    if (!number.ok) {
        return number;
    }
    if (number.next != text.size()) {
        return {false, number.next, number.value, "unexpected trailing input"};
    }
    return number;
}

int main() {
    std::vector<std::string_view> lines{
        "port=8080",
        "_workers=4",
        "9bad=1",
        "timeout=30ms"
    };

    for (std::string_view line : lines) {
        auto result = parse_assignment(line);
        if (result.ok) {
            std::cout << line << " -> value " << result.value << '\n';
        } else {
            std::cout << line << " -> error at " << result.next
                      << ": " << result.error << '\n';
        }
    }
}
