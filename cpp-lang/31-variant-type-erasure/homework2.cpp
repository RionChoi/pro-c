// 세션 31 과제 2: Result<T,E> 패턴 — 예외 없는 오류 전파
// std::variant를 이용해 Rust의 Result 타입을 구현한다.

#include <charconv>
#include <cmath>
#include <iostream>
#include <string>
#include <string_view>
#include <variant>

// ── 오류 타입 ──────────────────────────────────────────────────────────────
struct DivisionByZero { std::string msg; };
struct NegativeSqrt   { std::string msg; };
struct ParseError     { std::string msg; };

// ── Result 타입 별칭 ────────────────────────────────────────────────────────
template <typename T, typename E>
using Result = std::variant<T, E>;

// ── 안전한 연산 함수들 ──────────────────────────────────────────────────────
Result<double, DivisionByZero> safe_div(double a, double b) {
    if (b == 0.0)
        return DivisionByZero{"0으로 나눌 수 없습니다"};
    return a / b;
}

Result<double, NegativeSqrt> safe_sqrt(double x) {
    if (x < 0.0)
        return NegativeSqrt{"음수의 제곱근은 실수 범위에 없습니다"};
    return std::sqrt(x);
}

Result<int, ParseError> parse_int(std::string_view s) {
    int value{};
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
    if (ec != std::errc{})
        return ParseError{"정수 변환 실패: " + std::string(s)};
    return value;
}

// ── 결과 출력 헬퍼 ──────────────────────────────────────────────────────────
template <typename T, typename E>
void print_result(const std::string& label, const Result<T, E>& res) {
    std::cout << label << ": ";
    if (std::holds_alternative<T>(res)) {
        std::cout << "OK  → " << std::get<T>(res) << "\n";
    } else {
        std::cout << "ERR → " << std::get<E>(res).msg << "\n";
    }
}

int main() {
    std::cout << "=== 나눗셈 ===\n";
    print_result("10.0 / 3.0", safe_div(10.0, 3.0));
    print_result(" 5.0 / 0.0", safe_div(5.0, 0.0));

    std::cout << "\n=== 제곱근 ===\n";
    print_result("sqrt(16.0)", safe_sqrt(16.0));
    print_result("sqrt(-4.0)", safe_sqrt(-4.0));
    print_result("sqrt( 2.0)", safe_sqrt(2.0));

    std::cout << "\n=== 정수 파싱 ===\n";
    print_result(R"(parse "42"  )", parse_int("42"));
    print_result(R"(parse "-7"  )", parse_int("-7"));
    print_result(R"(parse "abc" )", parse_int("abc"));
    print_result(R"(parse ""    )", parse_int(""));

    // std::visit로 세밀한 처리
    std::cout << "\n=== std::visit 분기 처리 ===\n";
    auto r = safe_div(100.0, 4.0);
    std::visit([](const auto& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, double>) {
            std::cout << "결과값: " << v << " (성공)\n";
        } else {
            std::cout << "오류 처리: " << v.msg << "\n";
        }
    }, r);

    return 0;
}
