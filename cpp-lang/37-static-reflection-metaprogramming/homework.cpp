#include <iostream>
#include <type_traits>
#include <concepts>
#include <vector>

template <typename T>
typename std::enable_if<std::is_integral_v<T>, void>::type
print_type_info(T value) {
    std::cout << "정수형: " << value << std::endl;
}

template <typename T>
typename std::enable_if<std::is_floating_point_v<T>, void>::type
print_type_info(T value) {
    std::cout << "부동소수점형: " << value << std::endl;
}

template <typename T>
typename std::enable_if<std::is_same_v<T, std::string>, void>::type
print_type_info(const T& value) {
    std::cout << "문자열: " << value << std::endl;
}

template <typename T>
typename std::enable_if<std::is_integral_v<T>, T>::type
add(T a, T b) {
    return a + b;
}

template <typename T>
typename std::enable_if<std::is_floating_point_v<T>, T>::type
add(T a, T b) {
    return a + b;
}

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
bool is_even(T value) {
    return value % 2 == 0;
}

struct TypeInfo {
    const char* name;
    bool is_integral;
    bool is_floating;
    size_t size;
};

template <typename T>
TypeInfo get_type_info() {
    return {
        typeid(T).name(),
        std::is_integral_v<T>,
        std::is_floating_point_v<T>,
        sizeof(T)
    };
}

int main() {
    std::cout << "=== Type Traits와 SFINAE ===" << std::endl;

    std::cout << "\n1. enable_if를 이용한 타입별 함수 오버로드:" << std::endl;
    print_type_info(42);
    print_type_info(3.14);
    print_type_info(std::string("Hello"));

    std::cout << "\n2. 정수/부동소수점 별 연산:" << std::endl;
    int int_result = add(10, 20);
    double float_result = add(1.5, 2.5);
    std::cout << "정수 덧셈: " << int_result << std::endl;
    std::cout << "실수 덧셈: " << float_result << std::endl;

    std::cout << "\n3. 짝수 판정 (정수만 가능):" << std::endl;
    std::cout << "10은 짝수: " << (is_even(10) ? "참" : "거짓") << std::endl;
    std::cout << "15는 짝수: " << (is_even(15) ? "참" : "거짓") << std::endl;

    std::cout << "\n4. 타입 정보 추출:" << std::endl;
    {
        auto info = get_type_info<int>();
        std::cout << "int: 정수형=" << (info.is_integral ? "O" : "X")
                  << ", 크기=" << info.size << " bytes" << std::endl;

        auto info2 = get_type_info<double>();
        std::cout << "double: 부동소수점=" << (info2.is_floating ? "O" : "X")
                  << ", 크기=" << info2.size << " bytes" << std::endl;
    }

    std::cout << "\n5. is_same을 이용한 타입 비교:" << std::endl;
    std::cout << "int == int: " << (std::is_same_v<int, int> ? "참" : "거짓") << std::endl;
    std::cout << "int == double: " << (std::is_same_v<int, double> ? "참" : "거짓") << std::endl;
    std::cout << "double == double: " << (std::is_same_v<double, double> ? "참" : "거짓") << std::endl;

    std::cout << "\n6. is_integral을 이용한 타입 검사:" << std::endl;
    std::cout << "int는 정수형: " << (std::is_integral_v<int> ? "참" : "거짓") << std::endl;
    std::cout << "double은 정수형: " << (std::is_integral_v<double> ? "참" : "거짓") << std::endl;
    std::cout << "char은 정수형: " << (std::is_integral_v<char> ? "참" : "거짓") << std::endl;

    return 0;
}
