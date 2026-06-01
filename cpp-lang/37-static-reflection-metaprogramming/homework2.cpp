#include <iostream>
#include <string>
#include <sstream>
#include <tuple>
#include <iomanip>
#include <type_traits>

struct Employee {
    int id;
    std::string department;
    double salary;
};

struct Location {
    std::string city;
    int zipCode;
};

template <typename T>
std::string toJson(const T& obj);

template <>
std::string toJson<Employee>(const Employee& emp) {
    std::ostringstream oss;
    oss << "{"
        << "\"id\": " << emp.id
        << ", \"department\": \"" << emp.department << "\""
        << ", \"salary\": " << std::fixed << std::setprecision(2) << emp.salary
        << "}";
    return oss.str();
}

template <>
std::string toJson<Location>(const Location& loc) {
    std::ostringstream oss;
    oss << "{"
        << "\"city\": \"" << loc.city << "\""
        << ", \"zipCode\": " << loc.zipCode
        << "}";
    return oss.str();
}

template <typename T>
void reflectAndSerialize(const T& obj, const std::string& name) {
    std::cout << "\n=== " << name << " 직렬화 ===\n";
    std::cout << "JSON: " << toJson(obj) << "\n";
}

int main() {
    std::cout << "=== 메타프로그래밍 직렬화 테스트 ===\n";

    Employee emp{1001, "Engineering", 95000.50};
    Location loc{"Seoul", 06000};

    reflectAndSerialize(emp, "Employee");
    reflectAndSerialize(loc, "Location");

    std::cout << "\n=== 타입 정보 ===\n";
    std::cout << "Employee 멤버 타입:\n";
    std::cout << "  id: " << typeid(decltype(emp.id)).name() << "\n";
    std::cout << "  department: " << typeid(decltype(emp.department)).name() << "\n";
    std::cout << "  salary: " << typeid(decltype(emp.salary)).name() << "\n";

    std::cout << "\nLocation 멤버 타입:\n";
    std::cout << "  city: " << typeid(decltype(loc.city)).name() << "\n";
    std::cout << "  zipCode: " << typeid(decltype(loc.zipCode)).name() << "\n";

    return 0;
}
