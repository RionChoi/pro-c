#include <iostream>
#include <tuple>
#include <string>
#include <type_traits>

// 타입 이름 출력용 헬퍼 (C++17)
template <typename T>
struct TypeName {
    static std::string get() {
        if (std::is_same_v<T, int>) return "int";
        if (std::is_same_v<T, float>) return "float";
        if (std::is_same_v<T, double>) return "double";
        if (std::is_same_v<T, bool>) return "bool";
        if (std::is_same_v<T, std::string>) return "string";
        return "unknown";
    }
};

// 집계 초기화를 이용한 구조체 변환
struct Point {
    int x;
    int y;
    float z;
};

// Point를 tuple로 변환하는 함수
constexpr auto reflectPoint(const Point& p) {
    return std::make_tuple(p.x, p.y, p.z);
}

// 메타 구조체 정의
template <typename T>
struct FieldCount;

template <>
struct FieldCount<Point> {
    static constexpr int count = 3;
};

// 필드 이름 조회
template <typename T, int Index>
struct FieldName;

template <>
struct FieldName<Point, 0> {
    static constexpr const char* value = "x";
};

template <>
struct FieldName<Point, 1> {
    static constexpr const char* value = "y";
};

template <>
struct FieldName<Point, 2> {
    static constexpr const char* value = "z";
};

// 제너릭 출력 함수 (tuple 기반)
template <typename... Args, size_t... I>
void printTuple(const std::tuple<Args...>& t, std::index_sequence<I...>) {
    std::cout << "{ ";
    ((std::cout << FieldName<Point, I>::value << ": " << std::get<I>(t)
               << (I + 1 < sizeof...(Args) ? ", " : "")), ...);
    std::cout << " }\n";
}

template <typename T>
void reflectAndPrint(const T& obj);

template <>
void reflectAndPrint<Point>(const Point& obj) {
    auto t = reflectPoint(obj);
    printTuple(t, std::index_sequence_for<int, int, float>{});
}

int main() {
    Point p = {10, 20, 3.5f};

    std::cout << "Point 구조체 리플렉션:\n";
    std::cout << "필드 개수: " << FieldCount<Point>::count << "\n";
    std::cout << "필드 0: " << FieldName<Point, 0>::value << "\n";
    std::cout << "필드 1: " << FieldName<Point, 1>::value << "\n";
    std::cout << "필드 2: " << FieldName<Point, 2>::value << "\n";

    std::cout << "\nPoint 객체: ";
    reflectAndPrint(p);

    return 0;
}
