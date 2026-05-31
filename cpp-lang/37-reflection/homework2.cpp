#include <iostream>
#include <tuple>
#include <string>
#include <sstream>
#include <type_traits>

// 변수 템플릿으로 타입 체크
template <typename T>
inline constexpr bool is_integral_v = std::is_integral_v<T>;

template <typename T>
inline constexpr bool is_floating_point_v = std::is_floating_point_v<T>;

template <typename T>
inline constexpr bool is_string_v = std::is_same_v<T, std::string>;

// 타입별 포맷
template <typename T>
std::string formatValue(const T& val) {
    if constexpr (is_integral_v<T>) {
        return std::to_string(val);
    } else if constexpr (is_floating_point_v<T>) {
        std::ostringstream oss;
        oss << val;
        return oss.str();
    } else if constexpr (is_string_v<T>) {
        return "\"" + val + "\"";
    } else {
        return "<value>";
    }
}

// 구조체 정의
struct Person {
    std::string name;
    int age;
    float height;
};

// Person의 reflection 메타데이터
template <>
struct std::tuple_size<Person> : std::integral_constant<size_t, 3> {};

// Person을 tuple로 변환
auto reflect(const Person& p) {
    return std::make_tuple(p.name, p.age, p.height);
}

// 필드 이름
template <size_t Index>
struct PersonFieldName;

template <>
struct PersonFieldName<0> {
    static constexpr const char* value = "name";
};

template <>
struct PersonFieldName<1> {
    static constexpr const char* value = "age";
};

template <>
struct PersonFieldName<2> {
    static constexpr const char* value = "height";
};

// 제너릭 구조체 프린터 (한 필드씩)
template <typename T, size_t I>
void printField(const T& t) {
    const auto& field = std::get<I>(t);
    std::cout << PersonFieldName<I>::value << ": " << formatValue(field);
}

// 전체 필드 출력 (index_sequence 활용)
template <typename T, size_t... I>
void printAllFields(const T& t, std::index_sequence<I...>) {
    std::cout << "{\n";
    ((std::cout << "  ", printField<decltype(reflect(std::declval<Person>())), I>(t),
                 std::cout << "\n"), ...);
    std::cout << "}";
}

void printPerson(const Person& p) {
    auto t = reflect(p);
    printAllFields(t, std::index_sequence_for<std::string, int, float>{});
}

int main() {
    Person p = {"Alice", 28, 165.5f};

    std::cout << "Person 구조체:\n";
    printPerson(p);
    std::cout << "\n";

    Person p2 = {"Bob", 35, 180.0f};
    std::cout << "\nAnother Person:\n";
    printPerson(p2);
    std::cout << "\n";

    return 0;
}
