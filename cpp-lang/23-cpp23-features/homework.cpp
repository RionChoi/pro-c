// 세션 23 — 핵심 과제: std::expected
// 적용 개념: expected<T,E>, unexpected, and_then, or_else, transform
#include <iostream>
#include <expected>
#include <string>
#include <vector>
#include <cmath>
#include <charconv>

// ── 오류 타입 ─────────────────────────────────────────────────────────────────
enum class MathError  { DivisionByZero, NegativeSqrt, Overflow };
enum class ParseError { InvalidFormat, OutOfRange, EmptyInput };

std::string to_string(MathError e) {
    switch (e) {
    case MathError::DivisionByZero: return "0으로 나눌 수 없음";
    case MathError::NegativeSqrt:   return "음수의 제곱근 불가";
    case MathError::Overflow:       return "오버플로우";
    }
    return "알 수 없음";
}

std::string to_string(ParseError e) {
    switch (e) {
    case ParseError::InvalidFormat: return "잘못된 형식";
    case ParseError::OutOfRange:    return "범위 초과";
    case ParseError::EmptyInput:    return "빈 입력";
    }
    return "알 수 없음";
}

// ── expected 반환 함수들 ──────────────────────────────────────────────────────

std::expected<double, MathError> safe_divide(double a, double b) {
    if (b == 0.0) return std::unexpected(MathError::DivisionByZero);
    return a / b;
}

std::expected<double, MathError> safe_sqrt(double x) {
    if (x < 0.0) return std::unexpected(MathError::NegativeSqrt);
    return std::sqrt(x);
}

std::expected<long long, MathError> safe_multiply(long long a, long long b) {
    if (a != 0 && std::abs(b) > 9'000'000'000LL / std::abs(a))
        return std::unexpected(MathError::Overflow);
    return a * b;
}

std::expected<int, ParseError> parse_int(const std::string& s) {
    if (s.empty()) return std::unexpected(ParseError::EmptyInput);
    int result{};
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), result);
    if (ec == std::errc::invalid_argument)  return std::unexpected(ParseError::InvalidFormat);
    if (ec == std::errc::result_out_of_range) return std::unexpected(ParseError::OutOfRange);
    if (ptr != s.data() + s.size())         return std::unexpected(ParseError::InvalidFormat);
    return result;
}

// ── 결과 출력 헬퍼 ───────────────────────────────────────────────────────────
template <typename T, typename E>
void print_result(const std::string& expr,
                  const std::expected<T, E>& r,
                  std::string (*err_str)(E)) {
    std::cout << "  " << expr << " → ";
    if (r) std::cout << "✅ " << *r << "\n";
    else   std::cout << "❌ " << err_str(r.error()) << "\n";
}

// ── main ──────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "=== std::expected 과제 ===\n\n";

    // 1. 기본 사용
    std::cout << "[1] safe_divide\n";
    print_result("10 / 2",  safe_divide(10, 2),  to_string);
    print_result("7 / 0",   safe_divide(7, 0),   to_string);
    print_result("-9 / 3",  safe_divide(-9, 3),  to_string);

    std::cout << "\n[2] safe_sqrt\n";
    print_result("sqrt(25)",  safe_sqrt(25),   to_string);
    print_result("sqrt(2)",   safe_sqrt(2),    to_string);
    print_result("sqrt(-4)",  safe_sqrt(-4),   to_string);

    std::cout << "\n[3] safe_multiply\n";
    print_result("100 * 200",            safe_multiply(100, 200),                  to_string);
    print_result("9e9 * 2 (overflow)",   safe_multiply(9'000'000'000LL, 2),        to_string);

    // 2. value_or — 기본값 제공
    std::cout << "\n[4] value_or\n";
    auto r1 = safe_divide(10, 2);
    auto r2 = safe_divide(10, 0);
    std::cout << "  10/2 value_or(-1) = " << r1.value_or(-1) << "\n";
    std::cout << "  10/0 value_or(-1) = " << r2.value_or(-1) << "\n";

    // 3. and_then — 성공 시 체인
    std::cout << "\n[5] and_then 체인 (10/2 → sqrt)\n";
    auto chain = safe_divide(10.0, 2.0)
        .and_then([](double v) { return safe_sqrt(v); });
    print_result("sqrt(10/2)", chain, to_string);

    auto chain2 = safe_divide(10.0, 0.0)
        .and_then([](double v) { return safe_sqrt(v); });
    print_result("sqrt(10/0)", chain2, to_string);

    // 4. transform — 성공 값 변환
    std::cout << "\n[6] transform (값 변환)\n";
    auto t1 = safe_divide(10.0, 4.0)
        .transform([](double v) { return v * 100.0; });
    print_result("(10/4)*100", t1, to_string);

    auto t2 = safe_sqrt(-1.0)
        .transform([](double v) { return v * 2.0; });
    print_result("sqrt(-1)*2", t2, to_string);

    // 5. or_else — 실패 시 대체 처리
    std::cout << "\n[7] or_else (오류 복구)\n";
    auto recovered = safe_divide(10.0, 0.0)
        .or_else([](MathError) -> std::expected<double, MathError> {
            return 0.0;  // 0으로 나누면 0 반환
        });
    print_result("10/0 or_else(0)", recovered, to_string);

    // 6. 파싱 expected
    std::cout << "\n[8] parse_int\n";
    for (const std::string s : {"42", "-7", "3.14", "abc", "", "99999999999"}) {
        auto r = parse_int(s);
        std::cout << "  \"" << s << "\" → ";
        if (r) std::cout << "✅ " << *r << "\n";
        else   std::cout << "❌ " << to_string(r.error()) << "\n";
    }

    // 7. expected 조건 확인
    std::cout << "\n[9] has_value / error\n";
    auto ok  = safe_sqrt(9.0);
    auto err = safe_sqrt(-1.0);
    std::cout << "  sqrt(9) has_value: " << ok.has_value()  << "\n";
    std::cout << "  sqrt(-1) has_value: " << err.has_value() << "\n";
    std::cout << "  sqrt(-1) error: "
              << to_string(err.error()) << "\n";

    return 0;
}
