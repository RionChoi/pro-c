#include <iostream>
#include <type_traits>
#include <string>

template<typename T>
void printType(T value) {
    std::cout << "값: " << value << " | ";

    if (std::is_integral_v<T>) {
        std::cout << "타입: 정수";
    } else if (std::is_floating_point_v<T>) {
        std::cout << "타입: 부동소수";
    } else if (std::is_same_v<T, std::string>) {
        std::cout << "타입: 문자열";
    } else {
        std::cout << "타입: 기타";
    }
    std::cout << "\n";
}

template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
int doubleValue(T value) {
    return value * 2;
}

template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
double doubleValue(T value) {
    return value * 2.0;
}

template<typename T>
bool isSameType(T a, T b) {
    return std::is_same_v<decltype(a), decltype(b)>;
}

int main() {
    std::cout << "=== Type Traits 기초 ===\n\n";

    std::cout << "1. 타입 식별\n";
    printType(42);
    printType(3.14);
    printType(std::string("hello"));
    std::cout << "\n";

    std::cout << "2. SFINAE를 이용한 오버로드\n";
    std::cout << "doubleValue(10) = " << doubleValue(10) << "\n";
    std::cout << "doubleValue(5.5) = " << doubleValue(5.5) << "\n";
    std::cout << "\n";

    std::cout << "3. 타입 비교\n";
    std::cout << "isSameType(1, 2): " << std::boolalpha << isSameType(1, 2) << "\n";
    std::cout << "isSameType(1, 2.0): " << std::boolalpha << isSameType(1, 2.0) << "\n";
    std::cout << "\n";

    std::cout << "4. Type Traits 쿼리\n";
    std::cout << "is_integral<int>: " << std::is_integral_v<int> << "\n";
    std::cout << "is_integral<double>: " << std::is_integral_v<double> << "\n";
    std::cout << "is_pointer<int*>: " << std::is_pointer_v<int*> << "\n";
    std::cout << "is_const<const int>: " << std::is_const_v<const int> << "\n";
    std::cout << "\n";

    std::cout << "5. 크기 정보\n";
    std::cout << "sizeof(char): " << sizeof(char) << "\n";
    std::cout << "sizeof(int): " << sizeof(int) << "\n";
    std::cout << "sizeof(double): " << sizeof(double) << "\n";
    std::cout << "std::is_same_v<int, int>: " << std::is_same_v<int, int> << "\n";

    return 0;
}
