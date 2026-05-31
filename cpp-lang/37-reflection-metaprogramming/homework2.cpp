#include <iostream>
#include <tuple>
#include <vector>
#include <type_traits>

template<typename T>
struct tuple_size {};

template<typename... Args>
struct tuple_size<std::tuple<Args...>> {
    static constexpr size_t value = sizeof...(Args);
};

template<typename T>
inline constexpr size_t tuple_size_v = tuple_size<T>::value;

template<size_t Index, typename Tuple>
struct tuple_element {};

template<typename First, typename... Rest>
struct tuple_element<0, std::tuple<First, Rest...>> {
    using type = First;
};

template<size_t Index, typename First, typename... Rest>
struct tuple_element<Index, std::tuple<First, Rest...>> {
    using type = typename tuple_element<Index - 1, std::tuple<Rest...>>::type;
};

template<typename Tuple>
void print_tuple_types(Tuple t);

template<typename... Args, size_t... I>
void print_tuple_impl(std::tuple<Args...> t, std::index_sequence<I...>) {
    std::cout << "(";
    ((std::cout << (I == 0 ? "" : ", ") << std::get<I>(t)), ...);
    std::cout << ")\n";
}

template<typename... Args>
void print_tuple(const std::tuple<Args...>& t) {
    print_tuple_impl(t, std::index_sequence_for<Args...>{});
}

template<typename Tuple, size_t... I>
void print_tuple_types_impl(std::index_sequence<I...>) {
    std::cout << "Tuple size: " << sizeof...(I) << "\n";
    std::cout << "Element types: ";
    ([](auto idx) {
        using ElemType = typename tuple_element<idx, Tuple>::type;
        if constexpr (std::is_integral_v<ElemType>) {
            std::cout << "int ";
        } else if constexpr (std::is_floating_point_v<ElemType>) {
            std::cout << "double ";
        } else if constexpr (std::is_same_v<ElemType, std::string>) {
            std::cout << "string ";
        } else {
            std::cout << "unknown ";
        }
    }(std::integral_constant<size_t, I>{}), ...);
    std::cout << "\n";
}

template<typename Tuple>
void print_tuple_types() {
    print_tuple_types_impl<Tuple>(
        std::make_index_sequence<tuple_size_v<Tuple>>{}
    );
}

template<typename T>
struct is_container : std::false_type {};

template<typename T>
struct is_container<std::vector<T>> : std::true_type {};

template<typename... Args>
struct is_container<std::tuple<Args...>> : std::true_type {};

template<typename T>
constexpr bool is_container_v = is_container<T>::value;

template<typename T>
struct remove_all_pointers {
    using type = T;
};

template<typename T>
struct remove_all_pointers<T*> {
    using type = typename remove_all_pointers<T>::type;
};

template<typename T>
using remove_all_pointers_t = typename remove_all_pointers<T>::type;

int main() {
    std::cout << "=== 고급 메타프로그래밍 ===\n\n";

    std::cout << "1. 튜플 크기:\n";
    using MyTuple = std::tuple<int, double, std::string>;
    std::cout << "std::tuple<int, double, string> 크기: "
              << tuple_size_v<MyTuple> << '\n';

    std::cout << "\n2. 튜플 요소 타입:\n";
    print_tuple_types<MyTuple>();

    std::cout << "\n3. 튜플 출력 (fold expression):\n";
    MyTuple t1(42, 3.14, "Hello");
    std::cout << "Tuple: ";
    print_tuple(t1);

    std::cout << "\n4. 컨테이너 감지:\n";
    std::cout << "std::vector<int>는 컨테이너? "
              << (is_container_v<std::vector<int>> ? "Yes" : "No") << '\n';
    std::cout << "std::tuple<int, double>는 컨테이너? "
              << (is_container_v<std::tuple<int, double>> ? "Yes" : "No") << '\n';
    std::cout << "int는 컨테이너? "
              << (is_container_v<int> ? "Yes" : "No") << '\n';

    std::cout << "\n5. 모든 포인터 제거:\n";
    std::cout << "int***를 제거하면: ";
    if (std::is_same_v<remove_all_pointers_t<int***>, int>) {
        std::cout << "int\n";
    }

    std::cout << "double*를 제거하면: ";
    if (std::is_same_v<remove_all_pointers_t<double*>, double>) {
        std::cout << "double\n";
    }

    std::cout << "\n6. 기타 유형 특성:\n";
    std::cout << "std::is_reference<int&>: "
              << (std::is_reference_v<int&> ? "true" : "false") << '\n';
    std::cout << "std::is_reference<int>: "
              << (std::is_reference_v<int> ? "true" : "false") << '\n';

    std::cout << "std::is_array<int[10]>: "
              << (std::is_array_v<int[10]> ? "true" : "false") << '\n';
    std::cout << "std::is_array<int*>: "
              << (std::is_array_v<int*> ? "true" : "false") << '\n';

    return 0;
}
