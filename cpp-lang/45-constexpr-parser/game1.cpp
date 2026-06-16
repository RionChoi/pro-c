// Session 45: 컴파일 타임 파서 — JSON-lite 파서 게임
// 미니 JSON 파서로 간단한 설정 파일을 파싱하고 쿼리하는 게임
#include <string_view>
#include <string>
#include <vector>
#include <variant>
#include <map>
#include <iostream>
#include <sstream>
#include <optional>

// ─────────────────────────────────────────────
// JSON-lite 값 타입
// ─────────────────────────────────────────────

struct JsonNull {};
struct JsonValue;

using JsonArray  = std::vector<JsonValue>;
using JsonObject = std::map<std::string, JsonValue>;

struct JsonValue {
    std::variant<JsonNull, bool, long long, double, std::string, JsonArray, JsonObject> data;

    bool   is_null()   const { return std::holds_alternative<JsonNull>(data); }
    bool   is_bool()   const { return std::holds_alternative<bool>(data); }
    bool   is_int()    const { return std::holds_alternative<long long>(data); }
    bool   is_double() const { return std::holds_alternative<double>(data); }
    bool   is_string() const { return std::holds_alternative<std::string>(data); }
    bool   is_array()  const { return std::holds_alternative<JsonArray>(data); }
    bool   is_object() const { return std::holds_alternative<JsonObject>(data); }

    std::string type_name() const {
        if (is_null())   return "null";
        if (is_bool())   return "bool";
        if (is_int())    return "int";
        if (is_double()) return "double";
        if (is_string()) return "string";
        if (is_array())  return "array";
        return "object";
    }

    std::string to_string() const;
};

std::string JsonValue::to_string() const {
    if (is_null())   return "null";
    if (is_bool())   return std::get<bool>(data) ? "true" : "false";
    if (is_int())    return std::to_string(std::get<long long>(data));
    if (is_double()) {
        std::ostringstream os;
        os << std::get<double>(data);
        return os.str();
    }
    if (is_string()) return "\"" + std::get<std::string>(data) + "\"";
    if (is_array()) {
        std::string s = "[";
        const auto& arr = std::get<JsonArray>(data);
        for (std::size_t i = 0; i < arr.size(); ++i) {
            s += arr[i].to_string();
            if (i + 1 < arr.size()) s += ", ";
        }
        return s + "]";
    }
    if (is_object()) {
        std::string s = "{";
        const auto& obj = std::get<JsonObject>(data);
        std::size_t i = 0;
        for (auto& [k, v] : obj) {
            s += "\"" + k + "\": " + v.to_string();
            if (++i < obj.size()) s += ", ";
        }
        return s + "}";
    }
    return "?";
}

// ─────────────────────────────────────────────
// JSON-lite 파서
// ─────────────────────────────────────────────

constexpr bool is_space(char c)  { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
constexpr bool is_digit(char c)  { return c >= '0' && c <= '9'; }

struct Parser {
    std::string_view src;
    std::size_t pos{0};

    void skip_ws() {
        while (pos < src.size() && is_space(src[pos])) ++pos;
    }

    char peek() { skip_ws(); return pos < src.size() ? src[pos] : '\0'; }
    char consume() { skip_ws(); return pos < src.size() ? src[pos++] : '\0'; }
    bool expect(char c) {
        if (peek() == c) { ++pos; return true; }
        return false;
    }

    std::optional<JsonValue> parse_value();
    std::optional<JsonValue> parse_string_val();
    std::optional<JsonValue> parse_number();
    std::optional<JsonValue> parse_array();
    std::optional<JsonValue> parse_object();
    std::optional<std::string> parse_string_raw();

    std::optional<JsonValue> parse() {
        auto v = parse_value();
        skip_ws();
        return v;
    }
};

std::optional<std::string> Parser::parse_string_raw() {
    if (peek() != '"') return std::nullopt;
    ++pos;  // consume '"'
    std::string s;
    while (pos < src.size() && src[pos] != '"') {
        if (src[pos] == '\\' && pos + 1 < src.size()) {
            ++pos;
            switch (src[pos]) {
            case '"': s += '"'; break;
            case '\\': s += '\\'; break;
            case 'n': s += '\n'; break;
            case 't': s += '\t'; break;
            default: s += src[pos]; break;
            }
        } else {
            s += src[pos];
        }
        ++pos;
    }
    if (pos < src.size()) ++pos;  // consume closing '"'
    return s;
}

std::optional<JsonValue> Parser::parse_string_val() {
    auto s = parse_string_raw();
    if (!s) return std::nullopt;
    return JsonValue{std::move(*s)};
}

std::optional<JsonValue> Parser::parse_number() {
    skip_ws();
    bool neg = false;
    if (pos < src.size() && src[pos] == '-') { neg = true; ++pos; }
    if (pos >= src.size() || !is_digit(src[pos])) return std::nullopt;

    long long ival = 0;
    while (pos < src.size() && is_digit(src[pos]))
        ival = ival * 10 + static_cast<long long>(src[pos++] - '0');

    if (pos < src.size() && src[pos] == '.') {
        ++pos;
        double dval = static_cast<double>(ival);
        double frac = 0.1;
        while (pos < src.size() && is_digit(src[pos])) {
            dval += static_cast<double>(src[pos++] - '0') * frac;
            frac *= 0.1;
        }
        return JsonValue{neg ? -dval : dval};
    }
    return JsonValue{neg ? -ival : ival};
}

std::optional<JsonValue> Parser::parse_array() {
    if (!expect('[')) return std::nullopt;
    JsonArray arr;
    skip_ws();
    if (peek() == ']') { ++pos; return JsonValue{std::move(arr)}; }
    while (true) {
        auto v = parse_value();
        if (!v) return std::nullopt;
        arr.push_back(std::move(*v));
        skip_ws();
        if (peek() == ']') { ++pos; break; }
        if (!expect(',')) return std::nullopt;
    }
    return JsonValue{std::move(arr)};
}

std::optional<JsonValue> Parser::parse_object() {
    if (!expect('{')) return std::nullopt;
    JsonObject obj;
    skip_ws();
    if (peek() == '}') { ++pos; return JsonValue{std::move(obj)}; }
    while (true) {
        auto key = parse_string_raw();
        if (!key) return std::nullopt;
        skip_ws();
        if (!expect(':')) return std::nullopt;
        auto val = parse_value();
        if (!val) return std::nullopt;
        obj[*key] = std::move(*val);
        skip_ws();
        if (peek() == '}') { ++pos; break; }
        if (!expect(',')) return std::nullopt;
    }
    return JsonValue{std::move(obj)};
}

std::optional<JsonValue> Parser::parse_value() {
    skip_ws();
    if (pos >= src.size()) return std::nullopt;
    char c = src[pos];
    if (c == '"') return parse_string_val();
    if (c == '[') return parse_array();
    if (c == '{') return parse_object();
    if (c == '-' || is_digit(c)) return parse_number();
    // true / false / null
    auto starts = [&](std::string_view lit) {
        return src.substr(pos).starts_with(lit);
    };
    if (starts("true"))  { pos += 4; return JsonValue{true}; }
    if (starts("false")) { pos += 5; return JsonValue{false}; }
    if (starts("null"))  { pos += 4; return JsonValue{JsonNull{}}; }
    return std::nullopt;
}

std::optional<JsonValue> json_parse(std::string_view src) {
    Parser p{src};
    return p.parse();
}

// ─────────────────────────────────────────────
// 쿼리 엔진 — 점 표기법으로 값 접근
// ─────────────────────────────────────────────

std::optional<JsonValue> query(const JsonValue& root, std::string_view path) {
    if (path.empty()) return root;

    std::size_t dot = path.find('.');
    std::string_view key = (dot == std::string_view::npos) ? path : path.substr(0, dot);
    std::string_view rest = (dot == std::string_view::npos) ? "" : path.substr(dot + 1);

    if (root.is_object()) {
        const auto& obj = std::get<JsonObject>(root.data);
        auto it = obj.find(std::string(key));
        if (it == obj.end()) return std::nullopt;
        return rest.empty() ? it->second : query(it->second, rest);
    }
    if (root.is_array()) {
        // 숫자 인덱스
        std::size_t idx = 0;
        for (char c : key) {
            if (!is_digit(c)) return std::nullopt;
            idx = idx * 10 + static_cast<std::size_t>(c - '0');
        }
        const auto& arr = std::get<JsonArray>(root.data);
        if (idx >= arr.size()) return std::nullopt;
        return rest.empty() ? arr[idx] : query(arr[idx], rest);
    }
    return std::nullopt;
}

// ─────────────────────────────────────────────
// main 게임
// ─────────────────────────────────────────────

int main() {
    std::cout << "╔════════════════════════════════════╗\n";
    std::cout << "║  JSON-lite 파서 & 쿼리 게임         ║\n";
    std::cout << "╚════════════════════════════════════╝\n\n";

    // 설정 데이터
    constexpr std::string_view config_json = R"({
  "server": {
    "host": "localhost",
    "port": 8080,
    "debug": true
  },
  "database": {
    "name": "mydb",
    "max_conn": 20,
    "replicas": ["replica1", "replica2", "replica3"]
  },
  "features": ["auth", "logging", "metrics"],
  "version": 2,
  "pi": 3.14
})";

    std::cout << "=== 파싱 대상 JSON ===\n" << config_json << "\n\n";

    auto result = json_parse(config_json);
    if (!result) {
        std::cout << "파싱 실패!\n";
        return 1;
    }

    std::cout << "=== 파싱 성공 — 쿼리 테스트 ===\n";
    auto queries = {
        std::pair<std::string_view, std::string_view>
        {"server.host",               "\"localhost\""},
        {"server.port",               "8080"},
        {"server.debug",              "true"},
        {"database.name",             "\"mydb\""},
        {"database.max_conn",         "20"},
        {"database.replicas.0",       "\"replica1\""},
        {"database.replicas.2",       "\"replica3\""},
        {"features.1",                "\"logging\""},
        {"version",                   "2"},
        {"pi",                        "3.14"},
        {"server.missing",            "(없음)"},
    };

    for (auto [path, expected] : queries) {
        auto v = query(*result, path);
        std::string got = v ? v->to_string() : "(없음)";
        bool ok = (got == expected);
        std::cout << "  " << path << " = " << got
                  << (ok ? " ✅" : " ❌ (예상: " + std::string(expected) + ")") << '\n';
    }

    std::cout << "\n=== 타입 검사 ===\n";
    auto check_type = [&](std::string_view path, std::string_view expected_type) {
        auto v = query(*result, path);
        std::string t = v ? v->type_name() : "missing";
        std::cout << "  " << path << " type=" << t
                  << (t == expected_type ? " ✅" : " ❌") << '\n';
    };
    check_type("server.host",  "string");
    check_type("server.port",  "int");
    check_type("server.debug", "bool");
    check_type("features",     "array");
    check_type("database",     "object");
    check_type("pi",           "double");

    std::cout << "\n=== JSON 재직렬화 ===\n";
    std::cout << result->to_string() << '\n';

    std::cout << "\n=== 게임 완료 ===\n";
    return 0;
}
