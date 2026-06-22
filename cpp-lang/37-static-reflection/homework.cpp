#include <iostream>
#include <type_traits>

// 커스텀 타입 특성 구현

// 1. is_pointer - 포인터인지 확인
template <typename T>
struct is_pointer {
    static constexpr bool value = false;
};

template <typename T>
struct is_pointer<T*> {
    static constexpr bool value = true;
};

// 2. is_const - const 타입인지 확인
template <typename T>
struct is_const {
    static constexpr bool value = false;
};

template <typename T>
struct is_const<const T> {
    static constexpr bool value = true;
};

// 3. is_integral - 정수형인지 확인
template <typename T>
struct is_integral {
    static constexpr bool value = false;
};

template <>
struct is_integral<int> {
    static constexpr bool value = true;
};

template <>
struct is_integral<short> {
    static constexpr bool value = true;
};

template <>
struct is_integral<long> {
    static constexpr bool value = true;
};

template <>
struct is_integral<char> {
    static constexpr bool value = true;
};

// 4. remove_const - const 제거
template <typename T>
struct remove_const {
    using type = T;
};

template <typename T>
struct remove_const<const T> {
    using type = T;
};

// 5. decay - 배열/함수를 포인터로 변환
template <typename T>
struct decay {
    using type = typename remove_const<T>::type;
};

template <typename T>
struct decay<T[]> {
    using type = T*;
};

template <typename R, typename... Args>
struct decay<R(Args...)> {
    using type = R(*)(Args...);
};

// 타입 정보 출력 함수
template <typename T>
void analyze_type() {
    std::cout << "=== 타입 분석 ===" << std::endl;
    std::cout << "  is_pointer: " << (is_pointer<T>::value ? "true" : "false")
              << std::endl;
    std::cout << "  is_const: " << (is_const<T>::value ? "true" : "false")
              << std::endl;
    std::cout << "  is_integral: " << (is_integral<T>::value ? "true" : "false")
              << std::endl;
    std::cout << std::endl;
}

// SFINAE 활용 - 포인터에만 작동하는 함수
template <typename T>
typename std::enable_if<is_pointer<T>::value, void>::type print_as_pointer(T ptr) {
    std::cout << "[포인터] 주소: " << ptr << std::endl;
}

template <typename T>
typename std::enable_if<!is_pointer<T>::value, void>::type print_as_pointer(T val) {
    std::cout << "[일반값] 값: " << val << std::endl;
}

// 정수형에만 작동하는 함수
template <typename T>
typename std::enable_if<is_integral<T>::value, void>::type is_even(T num) {
    std::cout << (num % 2 == 0 ? "짝수" : "홀수") << std::endl;
}

int main() {
    std::cout << "=== 타입 특성(Traits) 실습 ===" << std::endl << std::endl;

    // 1. is_pointer 테스트
    std::cout << "1. is_pointer 테스트:" << std::endl;
    analyze_type<int>();
    analyze_type<int*>();
    analyze_type<int**>();

    // 2. is_const 테스트
    std::cout << "2. is_const 테스트:" << std::endl;
    int normal = 42;
    const int constant = 100;
    std::cout << "  int: " << (is_const<decltype(normal)>::value ? "const" : "non-const")
              << std::endl;
    std::cout << "  const int: "
              << (is_const<decltype(constant)>::value ? "const" : "non-const")
              << std::endl << std::endl;

    // 3. is_integral 테스트
    std::cout << "3. is_integral 테스트:" << std::endl;
    std::cout << "  int: " << (is_integral<int>::value ? "integral" : "non-integral")
              << std::endl;
    std::cout << "  double: " << (is_integral<double>::value ? "integral" : "non-integral")
              << std::endl;
    std::cout << "  char: " << (is_integral<char>::value ? "integral" : "non-integral")
              << std::endl << std::endl;

    // 4. remove_const 테스트
    std::cout << "4. remove_const 테스트:" << std::endl;
    std::cout << "  const int → ";
    if (std::is_same<typename remove_const<const int>::type, int>::value) {
        std::cout << "int (성공)" << std::endl;
    }
    std::cout << "  int → ";
    if (std::is_same<typename remove_const<int>::type, int>::value) {
        std::cout << "int (성공)" << std::endl;
    }
    std::cout << std::endl;

    // 5. SFINAE enable_if 테스트
    std::cout << "5. SFINAE (enable_if) 테스트:" << std::endl;
    int x = 42;
    int* ptr = &x;
    print_as_pointer(x);
    print_as_pointer(ptr);
    std::cout << std::endl;

    // 6. 정수형 함수 테스트
    std::cout << "6. 정수형 함수 테스트 (is_even):" << std::endl;
    std::cout << "  10은 ";
    is_even(10);
    std::cout << "  7은 ";
    is_even(7);
    std::cout << std::endl;

    // 7. decay 테스트
    std::cout << "7. decay 테스트:" << std::endl;
    int arr[10];
    std::cout << "  int[10] → ";
    if (std::is_same<typename decay<decltype(arr)>::type, int*>::value) {
        std::cout << "int* (성공)" << std::endl;
    }

    return 0;
}
