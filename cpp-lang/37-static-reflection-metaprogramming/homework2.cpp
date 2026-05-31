#include <iostream>
#include <tuple>
#include <string>
#include <type_traits>

template<typename T>
struct MemberTraits;

template<>
struct MemberTraits<int> {
    static void print(int val) { std::cout << val; }
    static const char *type_name() { return "int"; }
};

template<>
struct MemberTraits<double> {
    static void print(double val) { std::cout << val; }
    static const char *type_name() { return "double"; }
};

template<>
struct MemberTraits<std::string> {
    static void print(const std::string &val) { std::cout << val; }
    static const char *type_name() { return "string"; }
};

struct Employee {
    int id;
    std::string name;
    double salary;

    using Members = std::tuple<int, std::string, double>;

    Members to_tuple() const {
        return std::make_tuple(id, name, salary);
    }
};

template<typename Tuple, std::size_t... Is>
void print_members_impl(const Tuple &t, const char *names[], std::index_sequence<Is...>) {
    std::cout << "{ ";
    (..., (std::cout << (Is == 0 ? "" : ", ") << names[Is] << ": "
           << std::get<Is>(t)));
    std::cout << " }\n";
}

template<typename Tuple, std::size_t... Is>
void print_types_impl(std::index_sequence<Is...>) {
    std::cout << "Types: ";
    (..., (std::cout << (Is == 0 ? "" : ", ") << MemberTraits<std::tuple_element_t<Is, Tuple>>::type_name()));
    std::cout << "\n";
}

template<typename T>
struct IsPrimitive : std::false_type {};

template<>
struct IsPrimitive<int> : std::true_type {};

template<>
struct IsPrimitive<double> : std::true_type {};

template<>
struct IsPrimitive<std::string> : std::true_type {};

template<typename T, typename = void>
struct IsSerializable : std::false_type {};

template<typename T>
struct IsSerializable<T, std::void_t<decltype(std::declval<T>().to_tuple())>> : std::true_type {};

template<typename T>
struct CountMembers {
    static constexpr std::size_t value = std::tuple_size_v<typename T::Members>;
};

int main() {
    std::cout << "=== 고급 정적 리플렉션 ===\n\n";

    std::cout << "1️⃣  멤버 트레이츠와 타입 정보:\n";
    MemberTraits<int>::print(42);
    std::cout << " (타입: " << MemberTraits<int>::type_name() << ")\n";
    MemberTraits<double>::print(3.14);
    std::cout << " (타입: " << MemberTraits<double>::type_name() << ")\n";
    MemberTraits<std::string>::print("Hello");
    std::cout << " (타입: " << MemberTraits<std::string>::type_name() << ")\n\n";

    std::cout << "2️⃣  구조체 멤버 개수 추출:\n";
    Employee emp{1001, "Alice", 50000.0};
    std::cout << "Employee 멤버 개수: " << CountMembers<Employee>::value << "\n";
    std::cout << "Employee 멤버 타입: int, string, double\n\n";

    std::cout << "3️⃣  타입 검사:\n";
    std::cout << "IsPrimitive<int>: " << (IsPrimitive<int>::value ? "true" : "false") << "\n";
    std::cout << "IsSerializable<Employee>: " << (IsSerializable<Employee>::value ? "true" : "false") << "\n\n";

    std::cout << "4️⃣  튜플 기반 멤버 출력:\n";
    Employee emp2{1002, "Bob", 60000.0};
    auto members = emp2.to_tuple();
    const char *member_names[] = {"id", "name", "salary"};
    std::cout << "Employee: ";
    print_members_impl(members, member_names, std::index_sequence_for<int, std::string, double>{});

    std::cout << "\n5️⃣  컴파일 타임 멤버 분석:\n";
    print_types_impl<typename Employee::Members>(std::index_sequence_for<int, std::string, double>{});
    std::cout << "멤버 개수: " << std::tuple_size_v<typename Employee::Members> << "\n";

    std::cout << "\n✨ 컴파일 타임에 구조체 내성 달성!\n";

    return 0;
}
