#include <iostream>
#include <type_traits>
#include <vector>
#include <string>

template <typename T>
struct has_print {
    template <typename U>
    static auto test(int) -> decltype(std::declval<U>().print(), std::true_type{});

    template <typename U>
    static std::false_type test(...);

    static constexpr bool value = std::is_same_v<decltype(test<T>(0)), std::true_type>;
};

template <typename T>
struct is_vector : std::false_type {};

template <typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> : std::true_type {
    using element_type = T;
};

template <typename T>
inline constexpr bool is_vector_v = is_vector<T>::value;

template <typename T>
struct type_name;

template <>
struct type_name<int> {
    static constexpr const char* value = "int";
};

template <>
struct type_name<double> {
    static constexpr const char* value = "double";
};

template <>
struct type_name<std::string> {
    static constexpr const char* value = "std::string";
};

template <>
struct type_name<bool> {
    static constexpr const char* value = "bool";
};

template <typename T, typename Alloc>
struct type_name<std::vector<T, Alloc>> {
    static constexpr const char* value = "std::vector";
};

struct Point;
template <>
struct type_name<Point> {
    static constexpr const char* value = "Point";
};

template <typename T>
struct TypeInfo {
    static void print_info(const std::string& var_name) {
        std::cout << var_name << " 타입 정보:\n";
        std::cout << "  타입: " << type_name<T>::value << "\n";
        std::cout << "  크기: " << sizeof(T) << " 바이트\n";
        std::cout << "  정수형: " << (std::is_integral_v<T> ? "예" : "아니오") << "\n";
        std::cout << "  부동소수: " << (std::is_floating_point_v<T> ? "예" : "아니오") << "\n";
        std::cout << "  포인터: " << (std::is_pointer_v<T> ? "예" : "아니오") << "\n";
        std::cout << "  벡터: " << (is_vector_v<T> ? "예" : "아니오") << "\n";
        std::cout << "  print() 메서드: " << (has_print<T>::value ? "예" : "아니오") << "\n";
    }
};

struct Point {
    int x, y;

    void print() const {
        std::cout << "(" << x << ", " << y << ")";
    }
};

template <typename T>
void process_value(const T& value) {
    if constexpr (std::is_integral_v<T>) {
        std::cout << "정수 값: " << value << "\n";
    } else if constexpr (std::is_floating_point_v<T>) {
        std::cout << "부동소수 값: " << value << "\n";
    } else if constexpr (is_vector_v<T>) {
        std::cout << "벡터 (" << value.size() << "개 원소)\n";
    }
}

int main() {
    std::cout << "=== 타입 특성 및 SFINAE 데모 ===\n\n";

    std::cout << "타입 정보 출력:\n";
    TypeInfo<int>::print_info("int");
    std::cout << "\n";

    TypeInfo<double>::print_info("double");
    std::cout << "\n";

    TypeInfo<std::vector<int>>::print_info("std::vector<int>");
    std::cout << "\n";

    TypeInfo<Point>::print_info("Point");
    std::cout << "\n";

    std::cout << "조건부 처리 (if constexpr):\n";
    process_value(42);
    process_value(3.14);
    process_value(std::vector<int>{1, 2, 3});
    std::cout << "\n";

    std::cout << "멤버 감지:\n";
    std::cout << "int has print(): " << (has_print<int>::value ? "예" : "아니오") << "\n";
    std::cout << "Point has print(): " << (has_print<Point>::value ? "예" : "아니오") << "\n";

    Point p{10, 20};
    if constexpr (has_print<Point>::value) {
        std::cout << "Point 값: ";
        p.print();
        std::cout << "\n";
    }

    return 0;
}
