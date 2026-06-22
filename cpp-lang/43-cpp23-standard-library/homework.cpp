// Session 43: C++23 표준 라이브러리 - expected 스타일 에러 처리
#include <charconv>
#include <iostream>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#if __has_include(<expected>)
#include <expected>
#endif

#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202202L
template <typename T, typename E>
using Expected = std::expected<T, E>;

template <typename E>
auto unexpected(E error) {
    return std::unexpected<E>{std::move(error)};
}
#else
template <typename T, typename E>
class Expected {
public:
    Expected(T value) : storage_(std::move(value)) {}
    Expected(E error) : storage_(std::move(error)) {}

    bool has_value() const { return std::holds_alternative<T>(storage_); }
    explicit operator bool() const { return has_value(); }
    T& value() { return std::get<T>(storage_); }
    const T& value() const { return std::get<T>(storage_); }
    E& error() { return std::get<E>(storage_); }
    const E& error() const { return std::get<E>(storage_); }

private:
    std::variant<T, E> storage_;
};

template <typename E>
E unexpected(E error) {
    return error;
}
#endif

Expected<int, std::string> parse_int(std::string_view text) {
    int value = 0;
    const char* first = text.data();
    const char* last = text.data() + text.size();
    const auto [ptr, ec] = std::from_chars(first, last, value);

    if (ec != std::errc{} || ptr != last) {
        return unexpected(std::string{"invalid integer: "} + std::string{text});
    }
    return value;
}

Expected<double, std::string> divide(double left, double right) {
    if (right == 0.0) {
        return unexpected(std::string{"division by zero"});
    }
    return left / right;
}

Expected<int, std::string> parse_score(std::string_view text) {
    auto parsed = parse_int(text);
    if (!parsed) {
        return unexpected(parsed.error());
    }
    if (parsed.value() < 0 || parsed.value() > 100) {
        return unexpected(std::string{"score out of range: "} + std::to_string(parsed.value()));
    }
    return parsed.value();
}

int main() {
    std::cout << "=== expected-style parsing ===\n";
    std::vector<std::string> inputs{"95", "72", "oops", "101", "88"};

    int sum = 0;
    int count = 0;
    for (const auto& input : inputs) {
        auto score = parse_score(input);
        if (score) {
            std::cout << input << " -> " << score.value() << '\n';
            sum += score.value();
            ++count;
        } else {
            std::cout << input << " -> error: " << score.error() << '\n';
        }
    }

    auto average = divide(static_cast<double>(sum), static_cast<double>(count));
    if (average) {
        std::cout << "average = " << average.value() << '\n';
    } else {
        std::cout << "average error: " << average.error() << '\n';
    }
}
