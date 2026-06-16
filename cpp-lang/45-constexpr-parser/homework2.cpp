// Session 45: 템플릿 기반 파서 컴비네이터 라이브러리
#include <string_view>
#include <optional>
#include <tuple>
#include <variant>
#include <iostream>
#include <string>
#include <vector>
#include <functional>

// ─────────────────────────────────────────────
// ParseResult<T> — 파싱 결과 타입
// ─────────────────────────────────────────────

template <typename T>
struct ParseResult {
    bool             ok{false};
    T                value{};
    std::string_view rest{};

    explicit operator bool() const { return ok; }

    static ParseResult fail(std::string_view in)         { return {false, {}, in}; }
    static ParseResult success(T v, std::string_view r)  { return {true, std::move(v), r}; }
};

// ─────────────────────────────────────────────
// 원자 파서 — 람다/함수 객체 래퍼
// ─────────────────────────────────────────────

constexpr bool is_digit(char c)  { return c >= '0' && c <= '9'; }
constexpr bool is_alpha(char c)  { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
constexpr bool is_alnum(char c)  { return is_digit(c) || is_alpha(c); }
constexpr bool is_space(char c)  { return c == ' ' || c == '\t' || c == '\n'; }

constexpr std::string_view trim(std::string_view sv) {
    while (!sv.empty() && is_space(sv[0])) sv.remove_prefix(1);
    return sv;
}

// ─────────────────────────────────────────────
// 파서 컴비네이터 (타입 + 함수 결합 스타일)
// ─────────────────────────────────────────────

// Char — 단일 문자 파서
struct Char {
    char expected;
    ParseResult<char> operator()(std::string_view in) const {
        in = trim(in);
        if (!in.empty() && in[0] == expected)
            return ParseResult<char>::success(expected, in.substr(1));
        return ParseResult<char>::fail(in);
    }
};

// Satisfy — 조건 문자 파서
struct Satisfy {
    bool(*pred)(char);
    std::string_view name;

    ParseResult<char> operator()(std::string_view in) const {
        in = trim(in);
        if (!in.empty() && pred(in[0]))
            return ParseResult<char>::success(in[0], in.substr(1));
        return ParseResult<char>::fail(in);
    }
};

// Literal — 리터럴 문자열 파서
struct Literal {
    std::string_view lit;
    ParseResult<std::string_view> operator()(std::string_view in) const {
        in = trim(in);
        if (in.starts_with(lit))
            return ParseResult<std::string_view>::success(lit, in.substr(lit.size()));
        return ParseResult<std::string_view>::fail(in);
    }
};

// ─────────────────────────────────────────────
// Many — 0회 이상 반복
// ─────────────────────────────────────────────

template <typename P>
auto many(P parser) {
    return [parser](std::string_view in) -> ParseResult<std::vector<decltype(parser(in).value)>> {
        using V = decltype(parser(in).value);
        std::vector<V> results;
        while (true) {
            auto r = parser(in);
            if (!r.ok) break;
            results.push_back(std::move(r.value));
            in = r.rest;
        }
        return ParseResult<std::vector<V>>::success(std::move(results), in);
    };
}

// Many1 — 1회 이상 반복
template <typename P>
auto many1(P parser) {
    return [parser](std::string_view in) -> ParseResult<std::vector<decltype(parser(in).value)>> {
        using V = decltype(parser(in).value);
        auto first = parser(in);
        if (!first.ok) return ParseResult<std::vector<V>>::fail(in);
        std::vector<V> results{std::move(first.value)};
        in = first.rest;
        while (true) {
            auto r = parser(in);
            if (!r.ok) break;
            results.push_back(std::move(r.value));
            in = r.rest;
        }
        return ParseResult<std::vector<V>>::success(std::move(results), in);
    };
}

// ─────────────────────────────────────────────
// Seq2 — 두 파서를 순서대로
// ─────────────────────────────────────────────

template <typename P1, typename P2>
auto seq2(P1 p1, P2 p2) {
    return [p1, p2](std::string_view in) {
        using T1 = decltype(p1(in).value);
        using T2 = decltype(p2(in).value);
        using R  = ParseResult<std::pair<T1, T2>>;
        auto r1 = p1(in);
        if (!r1.ok) return R::fail(in);
        auto r2 = p2(r1.rest);
        if (!r2.ok) return R::fail(in);
        return R::success({std::move(r1.value), std::move(r2.value)}, r2.rest);
    };
}

// ─────────────────────────────────────────────
// Alt2 — 두 파서 중 하나 선택
// ─────────────────────────────────────────────

template <typename P1, typename P2>
auto alt2(P1 p1, P2 p2) {
    return [p1, p2](std::string_view in) -> ParseResult<std::string_view> {
        auto r1 = p1(in);
        if (r1.ok) return {true, r1.rest.substr(0,0), r1.rest};  // 간소화
        return p2(in);
    };
}

// ─────────────────────────────────────────────
// Map — 파서 결과를 변환
// ─────────────────────────────────────────────

template <typename P, typename F>
auto map(P parser, F fn) {
    return [parser, fn](std::string_view in) {
        auto r = parser(in);
        using R = ParseResult<decltype(fn(r.value))>;
        if (!r.ok) return R::fail(in);
        return R::success(fn(std::move(r.value)), r.rest);
    };
}

// ─────────────────────────────────────────────
// 고수준 파서 조합
// ─────────────────────────────────────────────

// 정수 파서 조합: ['-'?] digit+
auto make_integer_parser() {
    return [](std::string_view in) -> ParseResult<long long> {
        in = trim(in);
        bool neg = false;
        if (!in.empty() && in[0] == '-') { neg = true; in.remove_prefix(1); }
        auto digits = many1(Satisfy{is_digit, "digit"})(in);
        if (!digits.ok) return ParseResult<long long>::fail(in);
        long long val = 0;
        for (char c : digits.value) val = val * 10 + static_cast<long long>(c - '0');
        return ParseResult<long long>::success(neg ? -val : val, digits.rest);
    };
}

// 식별자 파서 조합: alpha alnum*
auto make_ident_parser() {
    return [](std::string_view in) -> ParseResult<std::string> {
        in = trim(in);
        if (in.empty() || !is_alpha(in[0]))
            return ParseResult<std::string>::fail(in);
        auto first = Satisfy{is_alpha, "alpha"}(in);
        auto rest  = many(Satisfy{is_alnum, "alnum"})(first.rest);
        std::string ident(1, first.value);
        for (char c : rest.value) ident += c;
        return ParseResult<std::string>::success(std::move(ident), rest.rest);
    };
}

// CSV 한 행 파싱: ident (',' ident)*
auto make_csv_row_parser() {
    auto ident_p = make_ident_parser();
    return [ident_p](std::string_view in) -> ParseResult<std::vector<std::string>> {
        auto first = ident_p(in);
        if (!first.ok) return ParseResult<std::vector<std::string>>::fail(in);
        std::vector<std::string> items{std::move(first.value)};
        std::string_view cur = first.rest;
        while (true) {
            auto comma = Char{','}(cur);
            if (!comma.ok) break;
            auto item = ident_p(comma.rest);
            if (!item.ok) break;
            items.push_back(std::move(item.value));
            cur = item.rest;
        }
        return ParseResult<std::vector<std::string>>::success(std::move(items), cur);
    };
}

// 키=값 파서
struct KV { std::string key; std::string value; };

auto make_kv_parser() {
    auto ident_p = make_ident_parser();
    return [ident_p](std::string_view in) -> ParseResult<KV> {
        auto k = ident_p(in);
        if (!k.ok) return ParseResult<KV>::fail(in);
        auto eq = Char{'='}(k.rest);
        if (!eq.ok) return ParseResult<KV>::fail(in);
        auto v = ident_p(eq.rest);
        if (!v.ok) return ParseResult<KV>::fail(in);
        return ParseResult<KV>::success({std::move(k.value), std::move(v.value)}, v.rest);
    };
}

// ─────────────────────────────────────────────
// main
// ─────────────────────────────────────────────

int main() {
    std::cout << "=== 파서 컴비네이터 라이브러리 ===\n\n";

    // 정수 파서
    std::cout << "[1] 정수 파서:\n";
    auto int_parser = make_integer_parser();
    for (auto sv : {"42", "-17", "  100abc", "xyz"}) {
        auto r = int_parser(sv);
        std::cout << "  \"" << sv << "\" => ";
        if (r.ok) std::cout << "ok, val=" << r.value << ", rest=\"" << r.rest << "\"";
        else      std::cout << "fail";
        std::cout << '\n';
    }

    // 식별자 파서
    std::cout << "\n[2] 식별자 파서:\n";
    auto ident_parser = make_ident_parser();
    for (auto sv : {"hello", "abc123", "123abc", "  world!"}) {
        auto r = ident_parser(sv);
        std::cout << "  \"" << sv << "\" => ";
        if (r.ok) std::cout << "ok, val=\"" << r.value << "\", rest=\"" << r.rest << "\"";
        else      std::cout << "fail";
        std::cout << '\n';
    }

    // many 조합
    std::cout << "\n[3] many(digit) 파서:\n";
    auto digits_p = many1(Satisfy{is_digit, "digit"});
    auto mapped = map(digits_p, [](std::vector<char> chars) {
        long long v = 0;
        for (char c : chars) v = v * 10 + static_cast<long long>(c - '0');
        return v;
    });
    auto r3 = mapped("12345xyz");
    std::cout << "  \"12345xyz\" => val=" << r3.value << ", rest=\"" << r3.rest << "\"\n";

    // CSV 파서
    std::cout << "\n[4] CSV 행 파서:\n";
    auto csv_parser = make_csv_row_parser();
    for (auto sv : {"apple,banana,cherry", "one,two", "solo"}) {
        auto r = csv_parser(sv);
        std::cout << "  \"" << sv << "\" => [";
        for (std::size_t i = 0; i < r.value.size(); ++i) {
            std::cout << r.value[i];
            if (i + 1 < r.value.size()) std::cout << ", ";
        }
        std::cout << "]\n";
    }

    // KV 파서
    std::cout << "\n[5] 키=값 파서:\n";
    auto kv_parser = make_kv_parser();
    for (auto sv : {"name=Alice", "count=ten", "x=y", "=bad"}) {
        auto r = kv_parser(sv);
        std::cout << "  \"" << sv << "\" => ";
        if (r.ok) std::cout << "key=\"" << r.value.key << "\" val=\"" << r.value.value << "\"";
        else      std::cout << "fail";
        std::cout << '\n';
    }

    // seq2 조합
    std::cout << "\n[6] seq2(ident, ':', ident) 조합:\n";
    auto pair_p = seq2(make_ident_parser(),
                       map(seq2(Char{':'}, make_ident_parser()),
                           [](auto p){ return p.second; }));
    auto r6 = pair_p("key:value rest");
    if (r6.ok)
        std::cout << "  \"key:value rest\" => (\"" << r6.value.first
                  << "\", \"" << r6.value.second << "\") rest=\"" << r6.rest << "\"\n";

    std::cout << "\n=== 세션 45 파서 컴비네이터 완료 ===\n";
    return 0;
}
