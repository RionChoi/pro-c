#include <iostream>
#include <type_traits>

template <typename T>
struct TypeTraits {
    static constexpr bool is_pointer = false;
    static constexpr bool is_array = false;
    static constexpr bool is_function = false;
    static constexpr bool is_integral = false;
    static constexpr bool is_floating = false;
};

template <typename T>
struct TypeTraits<T*> {
    static constexpr bool is_pointer = true;
    static constexpr bool is_array = false;
    static constexpr bool is_function = false;
    using element_type = T;
};

template <typename T, size_t N>
struct TypeTraits<T[N]> {
    static constexpr bool is_pointer = false;
    static constexpr bool is_array = true;
    static constexpr bool is_function = false;
    static constexpr size_t size = N;
    using element_type = T;
};

template <typename R, typename... Args>
struct TypeTraits<R(*)(Args...)> {
    static constexpr bool is_pointer = true;
    static constexpr bool is_function = true;
    static constexpr int arity = sizeof...(Args);
};

template <>
struct TypeTraits<int> {
    static constexpr bool is_pointer = false;
    static constexpr bool is_array = false;
    static constexpr bool is_function = false;
    static constexpr bool is_integral = true;
    static constexpr bool is_floating = false;
};

template <>
struct TypeTraits<double> {
    static constexpr bool is_pointer = false;
    static constexpr bool is_array = false;
    static constexpr bool is_function = false;
    static constexpr bool is_integral = false;
    static constexpr bool is_floating = true;
};

template <typename T>
struct RemovePointer {
    using type = T;
};

template <typename T>
struct RemovePointer<T*> {
    using type = T;
};

template <typename T>
struct AddPointer {
    using type = T*;
};

template <typename T>
struct AddPointer<T*> {
    using type = T*;
};

template <typename T>
struct RemoveArray {
    using type = T;
};

template <typename T, size_t N>
struct RemoveArray<T[N]> {
    using type = T;
};

template <typename T>
struct IsIntegral : std::false_type {};

template <>
struct IsIntegral<int> : std::true_type {};

template <>
struct IsIntegral<char> : std::true_type {};

template <>
struct IsIntegral<long> : std::true_type {};

template <typename T>
inline constexpr bool is_integral_v = IsIntegral<T>::value;

void print_type_info() {
    std::cout << "=== 타입 특성 예제 ===\n\n";

    std::cout << "int 특성:\n";
    std::cout << "  is_integral: " << TypeTraits<int>::is_integral << "\n";
    std::cout << "  is_floating: " << TypeTraits<int>::is_floating << "\n\n";

    std::cout << "double 특성:\n";
    std::cout << "  is_floating: " << TypeTraits<double>::is_floating << "\n\n";

    std::cout << "int* 특성:\n";
    std::cout << "  is_pointer: " << TypeTraits<int*>::is_pointer << "\n\n";

    std::cout << "int[10] 특성:\n";
    std::cout << "  is_array: " << TypeTraits<int[10]>::is_array << "\n";
    std::cout << "  size: " << TypeTraits<int[10]>::size << "\n\n";

    std::cout << "함수 포인터 int(*)(int, double) 특성:\n";
    std::cout << "  is_function: " << TypeTraits<int(*)(int, double)>::is_function << "\n";
    std::cout << "  arity: " << TypeTraits<int(*)(int, double)>::arity << "\n\n";

    std::cout << "포인터 제거:\n";
    std::cout << "  RemovePointer<int*>::type 은 int\n";
    std::cout << "  RemovePointer<int>::type 은 int\n\n";

    std::cout << "포인터 추가:\n";
    std::cout << "  AddPointer<int>::type 은 int*\n";
    std::cout << "  AddPointer<int*>::type 은 int*\n\n";

    std::cout << "배열 제거:\n";
    std::cout << "  RemoveArray<int[5]>::type 은 int\n";
    std::cout << "  RemoveArray<int>::type 은 int\n\n";

    std::cout << "Integral 판정:\n";
    std::cout << "  is_integral_v<int>: " << is_integral_v<int> << "\n";
    std::cout << "  is_integral_v<char>: " << is_integral_v<char> << "\n";
    std::cout << "  is_integral_v<double>: " << is_integral_v<double> << "\n";
}

template <typename T>
void enable_if_example(T val) {
    if constexpr (std::is_integral_v<T>) {
        std::cout << "정수 값: " << val << "\n";
    } else if constexpr (std::is_floating_point_v<T>) {
        std::cout << "부동소수점 값: " << val << "\n";
    }
}

int main() {
    print_type_info();

    std::cout << "\n=== if constexpr 예제 ===\n";
    enable_if_example(42);
    enable_if_example(3.14);

    return 0;
}
