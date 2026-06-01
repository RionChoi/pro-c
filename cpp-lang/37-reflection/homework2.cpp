#include <iostream>
#include <type_traits>
#include <array>

template<typename T>
struct TypeInfo {
    static constexpr const char* name() {
        if (std::is_same_v<T, int>) return "int";
        if (std::is_same_v<T, double>) return "double";
        if (std::is_same_v<T, float>) return "float";
        if (std::is_same_v<T, char>) return "char";
        return "unknown";
    }

    static constexpr size_t size() {
        return sizeof(T);
    }

    static constexpr bool isIntegral() {
        return std::is_integral_v<T>;
    }

    static constexpr bool isFloating() {
        return std::is_floating_point_v<T>;
    }
};

constexpr int factorial(int n) {
    return (n <= 1) ? 1 : n * factorial(n - 1);
}

constexpr int fibonacci(int n) {
    return (n <= 1) ? n : fibonacci(n - 1) + fibonacci(n - 2);
}

constexpr int power(int base, int exp) {
    return (exp == 0) ? 1 : base * power(base, exp - 1);
}

struct Person {
    int age;
    double height;
    float weight;
};

template<typename T>
constexpr int getMemberCount() {
    if (std::is_same_v<T, Person>) return 3;
    return 0;
}

int main() {
    std::cout << "=== constexpr 리플렉션 ===\n\n";

    std::cout << "1. 타입 정보 조회 (컴파일 타임)\n";
    std::cout << "int 타입명: " << TypeInfo<int>::name() << "\n";
    std::cout << "int 크기: " << TypeInfo<int>::size() << "\n";
    std::cout << "int는 정수: " << std::boolalpha << TypeInfo<int>::isIntegral() << "\n";
    std::cout << "\n";

    std::cout << "double 타입명: " << TypeInfo<double>::name() << "\n";
    std::cout << "double 크기: " << TypeInfo<double>::size() << "\n";
    std::cout << "double은 부동소수: " << std::boolalpha << TypeInfo<double>::isFloating() << "\n";
    std::cout << "\n";

    std::cout << "2. constexpr 메타함수\n";
    constexpr int fact5 = factorial(5);
    std::cout << "5! = " << fact5 << "\n";

    constexpr int fib8 = fibonacci(8);
    std::cout << "fibonacci(8) = " << fib8 << "\n";

    constexpr int pow2_10 = power(2, 10);
    std::cout << "2^10 = " << pow2_10 << "\n";
    std::cout << "\n";

    std::cout << "3. 구조체 리플렉션\n";
    std::cout << "Person 멤버 개수: " << getMemberCount<Person>() << "\n";
    std::cout << "Person 크기: " << sizeof(Person) << "\n";
    std::cout << "\n";

    std::cout << "4. 컴파일 타임 배열 생성\n";
    constexpr std::array<int, 5> squares = {1, 4, 9, 16, 25};
    for (auto sq : squares) {
        std::cout << sq << " ";
    }
    std::cout << "\n\n";

    std::cout << "5. 조건부 컴파일\n";
    if constexpr (std::is_same_v<int, int>) {
        std::cout << "int == int (컴파일 타임 결정)\n";
    }

    if constexpr (sizeof(int) == 4) {
        std::cout << "32비트 시스템\n";
    }

    return 0;
}
