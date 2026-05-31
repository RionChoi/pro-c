#include <iostream>
#include <type_traits>
#include <string>

template<typename T>
void print_type_info(T value) {
    std::cout << "Value: " << value << " | ";

    if constexpr (std::is_integral_v<T>) {
        std::cout << "Type: Integer";
        if constexpr (std::is_signed_v<T>) {
            std::cout << " (signed)";
        } else {
            std::cout << " (unsigned)";
        }
    } else if constexpr (std::is_floating_point_v<T>) {
        std::cout << "Type: Floating-point";
    } else if constexpr (std::is_same_v<T, std::string>) {
        std::cout << "Type: String";
    } else if constexpr (std::is_pointer_v<T>) {
        std::cout << "Type: Pointer";
    } else {
        std::cout << "Type: Unknown";
    }
    std::cout << '\n';
}

template<typename T>
T safe_add(T a, T b) {
    if constexpr (std::is_integral_v<T>) {
        std::cout << "정수 덧셈: ";
        return a + b;
    } else if constexpr (std::is_floating_point_v<T>) {
        std::cout << "부동소수점 덧셈: ";
        return a + b;
    }
}

template<typename T>
struct is_pair : std::false_type {};

template<typename T, typename U>
struct is_pair<std::pair<T, U>> : std::true_type {};

template<typename T>
inline constexpr bool is_pair_v = is_pair<T>::value;

template<typename T>
struct pointer_depth : std::integral_constant<int, 0> {};

template<typename T>
struct pointer_depth<T*> : std::integral_constant<int, pointer_depth<T>::value + 1> {};

template<typename T>
inline constexpr int pointer_depth_v = pointer_depth<T>::value;

int main() {
    std::cout << "=== if constexpr와 Type Traits ===\n\n";

    std::cout << "1. 타입 분류:\n";
    print_type_info(42);
    print_type_info(3.14);
    print_type_info(std::string("hello"));

    int* ptr = nullptr;
    print_type_info(ptr);

    std::cout << "\n2. 타입별 연산:\n";
    std::cout << "결과: " << safe_add(10, 20) << '\n';
    std::cout << "결과: " << safe_add(1.5, 2.5) << '\n';

    std::cout << "\n3. 커스텀 Type Traits:\n";

    std::cout << "std::pair<int, double>는 pair? "
              << (is_pair_v<std::pair<int, double>> ? "Yes" : "No") << '\n';
    std::cout << "int는 pair? " << (is_pair_v<int> ? "Yes" : "No") << '\n';

    std::cout << "\nPointer depth:\n";
    std::cout << "int 포인터 깊이: " << pointer_depth_v<int*> << '\n';
    std::cout << "int 이중 포인터 깊이: " << pointer_depth_v<int**> << '\n';
    std::cout << "int 삼중 포인터 깊이: " << pointer_depth_v<int***> << '\n';
    std::cout << "int (포인터 아님) 깊이: " << pointer_depth_v<int> << '\n';

    std::cout << "\n4. 표준 Type Traits:\n";
    std::cout << "std::is_same<int, int>: "
              << (std::is_same_v<int, int> ? "true" : "false") << '\n';
    std::cout << "std::is_same<int, double>: "
              << (std::is_same_v<int, double> ? "true" : "false") << '\n';

    std::cout << "std::is_integral<int>: "
              << (std::is_integral_v<int> ? "true" : "false") << '\n';
    std::cout << "std::is_integral<double>: "
              << (std::is_integral_v<double> ? "true" : "false") << '\n';

    std::cout << "std::is_const<const int>: "
              << (std::is_const_v<const int> ? "true" : "false") << '\n';

    return 0;
}
