// Session 45: 컴파일 타임 정규 표현식 & 파서
// constexpr FSM + 기본 파서 컴비네이터
#include <string_view>
#include <optional>
#include <array>
#include <iostream>
#include <string>
#include <charconv>

// ─────────────────────────────────────────────
// [1] 기본 ParseResult 타입
// ─────────────────────────────────────────────

template <typename T>
struct ParseResult {
    bool             ok{false};
    T                value{};
    std::string_view rest{};

    static constexpr ParseResult fail(std::string_view in) {
        return {false, {}, in};
    }
    static constexpr ParseResult success(T v, std::string_view rest_) {
        return {true, v, rest_};
    }
};

// ─────────────────────────────────────────────
// [2] 기본 문자 분류 (constexpr)
// ─────────────────────────────────────────────

constexpr bool is_digit(char c)  { return c >= '0' && c <= '9'; }
constexpr bool is_alpha(char c)  { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
constexpr bool is_alnum(char c)  { return is_digit(c) || is_alpha(c); }
constexpr bool is_space(char c)  { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
constexpr bool is_lower(char c)  { return c >= 'a' && c <= 'z'; }
constexpr bool is_upper(char c)  { return c >= 'A' && c <= 'Z'; }

// ─────────────────────────────────────────────
// [3] 원자 파서 함수
// ─────────────────────────────────────────────

// 단일 문자 매칭
constexpr ParseResult<char> parse_char(std::string_view in, char expected) {
    if (!in.empty() && in[0] == expected)
        return ParseResult<char>::success(expected, in.substr(1));
    return ParseResult<char>::fail(in);
}

// 조건 문자 매칭
constexpr ParseResult<char> parse_if(std::string_view in, bool(*pred)(char)) {
    if (!in.empty() && pred(in[0]))
        return ParseResult<char>::success(in[0], in.substr(1));
    return ParseResult<char>::fail(in);
}

// 공백 스킵
constexpr std::string_view skip_spaces(std::string_view in) {
    while (!in.empty() && is_space(in[0])) in.remove_prefix(1);
    return in;
}

// 정수 파싱 (부호 포함)
constexpr ParseResult<long long> parse_integer(std::string_view in) {
    in = skip_spaces(in);
    if (in.empty()) return ParseResult<long long>::fail(in);

    bool neg = false;
    std::string_view cur = in;
    if (cur[0] == '-') { neg = true; cur.remove_prefix(1); }
    else if (cur[0] == '+') { cur.remove_prefix(1); }

    if (cur.empty() || !is_digit(cur[0])) return ParseResult<long long>::fail(in);

    long long val = 0;
    while (!cur.empty() && is_digit(cur[0])) {
        val = val * 10 + static_cast<long long>(cur[0] - '0');
        cur.remove_prefix(1);
    }
    return ParseResult<long long>::success(neg ? -val : val, cur);
}

// 식별자 파싱 (alpha + alnum*)
constexpr ParseResult<std::string_view> parse_ident(std::string_view in) {
    in = skip_spaces(in);
    if (in.empty() || !is_alpha(in[0])) return ParseResult<std::string_view>::fail(in);
    std::size_t len = 0;
    while (len < in.size() && is_alnum(in[len])) ++len;
    return ParseResult<std::string_view>::success(in.substr(0, len), in.substr(len));
}

// 리터럴 문자열 파싱
constexpr ParseResult<std::string_view> parse_literal(
    std::string_view in, std::string_view lit)
{
    in = skip_spaces(in);
    if (in.starts_with(lit))
        return ParseResult<std::string_view>::success(lit, in.substr(lit.size()));
    return ParseResult<std::string_view>::fail(in);
}

// compile-time 검증
static_assert(parse_integer("42rest").ok    == true);
static_assert(parse_integer("42rest").value == 42);
static_assert(parse_integer("-7 ").ok       == true);
static_assert(parse_integer("-7 ").value    == -7);
static_assert(parse_integer("abc").ok       == false);
static_assert(parse_ident("hello world").ok == true);
static_assert(parse_ident("123").ok         == false);

// ─────────────────────────────────────────────
// [4] constexpr FSM — 간단한 정규 표현식 인식기
// ─────────────────────────────────────────────

// 패턴: [0-9]+  (하나 이상의 숫자)
constexpr bool match_digits(std::string_view sv) {
    if (sv.empty() || !is_digit(sv[0])) return false;
    for (char c : sv) if (!is_digit(c)) return false;
    return true;
}

// 패턴: [a-zA-Z][a-zA-Z0-9]*  (식별자)
constexpr bool match_ident(std::string_view sv) {
    if (sv.empty() || !is_alpha(sv[0])) return false;
    for (std::size_t i = 1; i < sv.size(); ++i)
        if (!is_alnum(sv[i])) return false;
    return true;
}

// 패턴: [0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}  (간단한 IPv4)
constexpr bool match_ipv4(std::string_view sv) {
    int octets = 0;
    std::string_view cur = sv;
    while (!cur.empty()) {
        // 숫자 부분
        int digits = 0;
        int val = 0;
        while (!cur.empty() && is_digit(cur[0])) {
            val = val * 10 + static_cast<int>(cur[0] - '0');
            cur.remove_prefix(1);
            ++digits;
        }
        if (digits == 0 || digits > 3 || val > 255) return false;
        ++octets;
        if (cur.empty()) break;
        if (cur[0] != '.') return false;
        cur.remove_prefix(1);
    }
    return octets == 4;
}

// 패턴: \d{4}-\d{2}-\d{2}  (날짜 YYYY-MM-DD)
constexpr bool match_date(std::string_view sv) {
    if (sv.size() != 10) return false;
    for (int i : {0,1,2,3,5,6,8,9})
        if (!is_digit(sv[static_cast<std::size_t>(i)])) return false;
    return sv[4] == '-' && sv[7] == '-';
}

// 패턴: [A-Z]{2,3}[0-9]{2,4}  (항공편 코드, 예: KE001, OZ123)
constexpr bool match_flight(std::string_view sv) {
    std::size_t i = 0;
    while (i < sv.size() && is_upper(sv[i])) ++i;
    if (i < 2 || i > 3) return false;
    std::size_t d = 0;
    while (i < sv.size() && is_digit(sv[i])) { ++i; ++d; }
    return d >= 2 && d <= 4 && i == sv.size();
}

static_assert(match_digits("12345")    == true);
static_assert(match_digits("12a45")    == false);
static_assert(match_ident("hello")     == true);
static_assert(match_ident("h3llo")     == true);
static_assert(match_ident("3hello")    == false);
static_assert(match_ipv4("192.168.1.1") == true);
static_assert(match_ipv4("256.0.0.1")   == false);
static_assert(match_date("2026-06-14") == true);
static_assert(match_date("2026-6-14")  == false);
static_assert(match_flight("KE001")    == true);
static_assert(match_flight("OZ1234")   == true);
static_assert(match_flight("K1")       == false);

// ─────────────────────────────────────────────
// [5] 간단한 수식 파서 (재귀 하강)
// ─────────────────────────────────────────────
// 문법: expr = term (('+' | '-') term)*
//       term = factor (('*' | '/') factor)*
//       factor = integer | '(' expr ')'

struct ExprParser {
    std::string_view input;
    std::size_t pos{0};

    char peek() const {
        auto s = skip_spaces(input.substr(pos));
        return s.empty() ? '\0' : s[0];
    }
    void advance_past_spaces() {
        while (pos < input.size() && is_space(input[pos])) ++pos;
    }
    char consume() {
        advance_past_spaces();
        return pos < input.size() ? input[pos++] : '\0';
    }

    long long parse_factor() {
        advance_past_spaces();
        if (pos < input.size() && input[pos] == '(') {
            ++pos;
            long long v = parse_expr();
            advance_past_spaces();
            if (pos < input.size() && input[pos] == ')') ++pos;
            return v;
        }
        auto res = parse_integer(input.substr(pos));
        if (!res.ok) return 0;
        pos = input.size() - res.rest.size();
        return res.value;
    }

    long long parse_term() {
        long long v = parse_factor();
        while (true) {
            advance_past_spaces();
            if (pos >= input.size()) break;
            char op = input[pos];
            if (op != '*' && op != '/') break;
            ++pos;
            long long r = parse_factor();
            v = (op == '*') ? v * r : (r != 0 ? v / r : 0);
        }
        return v;
    }

    long long parse_expr() {
        long long v = parse_term();
        while (true) {
            advance_past_spaces();
            if (pos >= input.size()) break;
            char op = input[pos];
            if (op != '+' && op != '-') break;
            ++pos;
            long long r = parse_term();
            v = (op == '+') ? v + r : v - r;
        }
        return v;
    }
};

long long eval(std::string_view expr) {
    ExprParser p{expr};
    return p.parse_expr();
}

// ─────────────────────────────────────────────
// main
// ─────────────────────────────────────────────

int main() {
    std::cout << "=== [1-3] 원자 파서 (static_assert 통과) ===\n";
    auto r1 = parse_integer("  -42 rest");
    std::cout << "  parse_integer(\"-42 rest\"): ok=" << r1.ok
              << " val=" << r1.value << " rest=\"" << r1.rest << "\"\n";

    auto r2 = parse_ident("hello123 world");
    std::cout << "  parse_ident(\"hello123 world\"): ok=" << r2.ok
              << " val=\"" << r2.value << "\" rest=\"" << r2.rest << "\"\n";

    auto r3 = parse_literal("  +  rest", "+");
    std::cout << "  parse_literal(\"+  rest\", \"+\"): ok=" << r3.ok
              << " rest=\"" << r3.rest << "\"\n\n";

    std::cout << "=== [4] constexpr FSM 패턴 매칭 ===\n";
    auto test = [](std::string_view label, bool res) {
        std::cout << "  " << label << ": " << (res ? "✅" : "❌") << '\n';
    };
    test("match_digits(\"12345\")",    match_digits("12345"));
    test("match_digits(\"12a45\")",    !match_digits("12a45"));
    test("match_ident(\"hello\")",     match_ident("hello"));
    test("match_ipv4(\"192.168.1.1\")", match_ipv4("192.168.1.1"));
    test("match_ipv4(\"256.0.0.1\")",  !match_ipv4("256.0.0.1"));
    test("match_date(\"2026-06-14\")", match_date("2026-06-14"));
    test("match_date(\"2026-6-14\")",  !match_date("2026-6-14"));
    test("match_flight(\"KE001\")",    match_flight("KE001"));
    test("match_flight(\"OZ1234\")",   match_flight("OZ1234"));
    test("match_flight(\"K1\")",       !match_flight("K1"));
    std::cout << '\n';

    std::cout << "=== [5] 수식 파서 (재귀 하강) ===\n";
    auto exprs = {
        std::pair<std::string_view, long long>{"1 + 2",           3},
        {"3 * 4 + 5",                                             17},
        {"(3 + 4) * 5",                                           35},
        {"100 - 20 * 3",                                          40},
        {"(10 + 2) / (2 + 1)",                                     4},
        {"2 * (3 + (4 - 1))",                                     12},
    };
    for (auto [expr, expected] : exprs) {
        long long got = eval(expr);
        std::cout << "  \"" << expr << "\" = " << got
                  << (got == expected ? "  ✅" : "  ❌") << '\n';
    }

    std::cout << "\n=== 세션 45 완료: 컴파일 타임 파서 기초 ===\n";
    return 0;
}
