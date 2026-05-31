#include <iostream>
#include <tuple>
#include <string>
#include <typeinfo>
#include <type_traits>

// 구조체를 튜플로 변환하는 예제
struct Employee {
    int id;
    std::string name;
    double salary;
};

// 튜플 요소 타입 출력 (일반화)
template <typename Tuple, size_t... Is>
void print_types(std::index_sequence<Is...>) {
    std::cout << "튜플 멤버 타입: ";
    int count = 0;
    (..., (std::cout << (count++ > 0 ? ", " : "")
                     << typeid(std::tuple_element_t<Is, Tuple>).name()));
    std::cout << "\n";
}

// 튜플 값 출력
template <typename Tuple, size_t... Is>
void print_values(const Tuple& t, std::index_sequence<Is...>) {
    std::cout << "튜플 값: ";
    int count = 0;
    (..., (std::cout << (count++ > 0 ? ", " : "") << std::get<Is>(t)));
    std::cout << "\n";
}

// 구조체 필드 접근을 위한 포장 함수
template <typename T>
auto to_tuple(const T& obj) {
    // 일반화되지 않은 구조체: 직접 구현
    if constexpr (std::is_same_v<T, Employee>) {
        return std::make_tuple(obj.id, obj.name, obj.salary);
    }
}

int main() {
    Employee emp{101, "Alice", 75000.50};

    std::cout << "=== 구조체를 튜플로 분석 ===\n\n";

    // 튜플로 변환
    auto emp_tuple = to_tuple(emp);

    std::cout << "원본 구조체:\n";
    std::cout << "ID: " << emp.id << "\n";
    std::cout << "이름: " << emp.name << "\n";
    std::cout << "급여: " << emp.salary << "\n\n";

    std::cout << "튜플 크기: " << std::tuple_size_v<decltype(emp_tuple)> << "\n";

    // 타입 정보 출력
    using TupleType = decltype(emp_tuple);
    print_types<TupleType>(std::make_index_sequence<std::tuple_size_v<TupleType>>{});

    // 튜플 값 출력
    print_values(emp_tuple, std::make_index_sequence<std::tuple_size_v<TupleType>>{});

    std::cout << "\n개별 요소 접근:\n";
    std::cout << "요소 0: " << std::get<0>(emp_tuple) << "\n";
    std::cout << "요소 1: " << std::get<1>(emp_tuple) << "\n";
    std::cout << "요소 2: " << std::get<2>(emp_tuple) << "\n";

    return 0;
}
