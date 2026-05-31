#include <iostream>
#include <type_traits>
#include <vector>
#include <string>

template<typename T>
typename std::enable_if<std::is_integral_v<T>, void>::type
process(T value) {
    std::cout << "정수: " << value << " (크기: " << sizeof(T) << " bytes)\n";
}

template<typename T>
typename std::enable_if<std::is_floating_point_v<T>, void>::type
process(T value) {
    std::cout << "부동소수점: " << value << " (크기: " << sizeof(T) << " bytes)\n";
}

template<typename T>
typename std::enable_if<std::is_same_v<T, std::string>, void>::type
process(const T& value) {
    std::cout << "문자열: " << value << " (길이: " << value.length() << ")\n";
}

template<typename Container>
typename std::enable_if<std::is_same_v<Container, std::vector<typename Container::value_type>>, void>::type
process(const Container& container) {
    std::cout << "벡터 (크기: " << container.size() << "): ";
    for (const auto& item : container) {
        std::cout << item << " ";
    }
    std::cout << '\n';
}

template<typename T>
struct array_traits;

template<typename T, std::size_t N>
struct array_traits<T[N]> {
    using element_type = T;
    static constexpr std::size_t size = N;
};

template<typename T>
typename std::enable_if<std::is_array_v<T>, void>::type
print_array_info() {
    using traits = array_traits<T>;
    std::cout << "배열 정보: 요소 타입 크기 = " << sizeof(typename traits::element_type)
              << ", 배열 크기 = " << traits::size << '\n';
}

int main() {
    process(42);
    process(3.14);
    process(std::string("Hello"));

    std::vector<int> vec{1, 2, 3, 4, 5};
    process(vec);

    int arr[10];
    print_array_info<decltype(arr)>();

    std::cout << "\n=== 타입 검사 결과 ===\n";
    std::cout << "int는 정수 타입? " << std::is_integral_v<int> << '\n';
    std::cout << "double은 부동소수점? " << std::is_floating_point_v<double> << '\n';
    std::cout << "std::string은 배열? " << std::is_array_v<std::string> << '\n';
    std::cout << "int*는 포인터? " << std::is_pointer_v<int*> << '\n';

    return 0;
}
