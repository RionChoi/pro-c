#include <iostream>
#include <string>
#include <sstream>
#include <tuple>
#include <type_traits>

template <typename T>
struct TypeName {
    static const char* value() { return "unknown"; }
};

template <>
struct TypeName<int> {
    static const char* value() { return "int"; }
};

template <>
struct TypeName<double> {
    static const char* value() { return "double"; }
};

template <>
struct TypeName<std::string> {
    static const char* value() { return "string"; }
};

template <>
struct TypeName<bool> {
    static const char* value() { return "bool"; }
};

class TypeSafeFormatter {
public:
    template <typename... Args>
    static std::string format(const std::string& fmt, Args... args) {
        std::ostringstream oss;
        format_impl(oss, fmt, 0, args...);
        return oss.str();
    }

private:
    template <typename... Args>
    static void format_impl(std::ostringstream& oss,
                           const std::string& fmt, size_t idx) {
        oss << fmt.substr(idx);
    }

    template <typename T, typename... Rest>
    static void format_impl(std::ostringstream& oss,
                           const std::string& fmt, size_t idx,
                           T value, Rest... rest) {
        size_t pos = fmt.find("{}", idx);
        if (pos == std::string::npos) {
            oss << fmt.substr(idx);
            return;
        }

        oss << fmt.substr(idx, pos - idx) << value;
        format_impl(oss, fmt, pos + 2, rest...);
    }
};

template <typename... Args>
std::string sprintf_safe(const std::string& fmt, Args... args) {
    return TypeSafeFormatter::format(fmt, args...);
}

template <typename T>
void print_types_impl(T value);

template <typename T, typename... Rest>
void print_types_impl(T value, Rest... rest);

template <typename... Args>
void print_types(Args... args) {
    std::cout << "타입 정보: ";
    print_types_impl(args...);
    std::cout << '\n';
}

template <typename T>
void print_types_impl(T value) {
    std::cout << TypeName<T>::value();
}

template <typename T, typename... Rest>
void print_types_impl(T value, Rest... rest) {
    std::cout << TypeName<T>::value() << ", ";
    print_types_impl(rest...);
}

template <typename... Args>
class Tuple {
    std::tuple<Args...> data_;

public:
    Tuple(Args... args) : data_(args...) {}

    template <size_t I>
    auto get() {
        return std::get<I>(data_);
    }

    void print_all() {
        print_impl(std::index_sequence_for<Args...>{});
    }

private:
    template <size_t... I>
    void print_impl(std::index_sequence<I...>) {
        ((std::cout << std::get<I>(data_) << " "), ...);
    }
};

int main() {
    std::cout << "=== 타입-안전 포맷팅 ===\n";

    std::string s1 = sprintf_safe("값: {}, {}, {}", 42, 3.14, "Hello");
    std::cout << s1 << '\n';

    std::string s2 = sprintf_safe("합계: {} + {} = {}", 5, 7, 12);
    std::cout << s2 << '\n';

    std::cout << "\n=== 타입 정보 출력 ===\n";
    print_types(42, 3.14, "World", true);
    print_types(100, 200);
    print_types("ABC", "DEF", "GHI");

    std::cout << "\n=== 가변 튜플 ===\n";
    Tuple<int, double, std::string> t(42, 2.71, "C++");
    std::cout << "튜플 값: ";
    t.print_all();
    std::cout << '\n';

    Tuple<bool, int, bool> flags(true, 100, false);
    std::cout << "플래그: ";
    flags.print_all();
    std::cout << '\n';

    return 0;
}
