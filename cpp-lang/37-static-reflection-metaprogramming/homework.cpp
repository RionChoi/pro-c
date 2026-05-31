#include <iostream>
#include <tuple>
#include <string>
#include <type_traits>
#include <memory>

template<typename T>
class Comparable : public std::enable_shared_from_this<Comparable<T>> {
public:
    bool operator<(const Comparable &other) const {
        return static_cast<const T &>(*this) < static_cast<const T &>(other);
    }

    bool operator==(const Comparable &other) const {
        return static_cast<const T &>(*this) == static_cast<const T &>(other);
    }

    bool operator<=(const Comparable &other) const {
        return *this < other || *this == other;
    }
};

struct Person : public Comparable<Person> {
    std::string name;
    int age;

    Person(const std::string &n, int a) : name(n), age(a) {}

    bool operator<(const Person &other) const {
        if (age != other.age) return age < other.age;
        return name < other.name;
    }

    bool operator==(const Person &other) const {
        return name == other.name && age == other.age;
    }

    void print() const {
        std::cout << name << " (" << age << ")\n";
    }
};

struct Box : public Comparable<Box> {
    double volume;

    Box(double v) : volume(v) {}

    bool operator<(const Box &other) const {
        return volume < other.volume;
    }

    bool operator==(const Box &other) const {
        return volume == other.volume;
    }

    void print() const {
        std::cout << "Box(volume=" << volume << ")\n";
    }
};

template<typename T>
struct TypeName {
    static const char *name() { return "unknown"; }
};

template<>
struct TypeName<int> {
    static const char *name() { return "int"; }
};

template<>
struct TypeName<double> {
    static const char *name() { return "double"; }
};

template<>
struct TypeName<std::string> {
    static const char *name() { return "string"; }
};

template<typename T>
void print_type() {
    std::cout << TypeName<T>::name() << "\n";
}

template<typename T, typename U>
struct IsSame {
    static const bool value = false;
};

template<typename T>
struct IsSame<T, T> {
    static const bool value = true;
};

template<typename Tuple, std::size_t... Is>
void print_tuple_impl(const Tuple &t, std::index_sequence<Is...>) {
    std::cout << "(";
    (..., (std::cout << (Is == 0 ? "" : ", ") << std::get<Is>(t)));
    std::cout << ")\n";
}

template<typename... Args>
void print_tuple(const std::tuple<Args...> &t) {
    print_tuple_impl(t, std::index_sequence_for<Args...>{});
}

int main() {
    std::cout << "=== 정적 리플렉션 & 메타프로그래밍 ===\n\n";

    std::cout << "1️⃣  타입 특성 기초:\n";
    print_type<int>();
    print_type<double>();
    print_type<std::string>();

    std::cout << "\n2️⃣  타입 비교:\n";
    std::cout << "IsSame<int, int>: " << IsSame<int, int>::value << "\n";
    std::cout << "IsSame<int, double>: " << IsSame<int, double>::value << "\n";

    std::cout << "\n3️⃣  튜플 기반 다형성:\n";
    std::tuple<int, double, std::string> t1(42, 3.14, "Hello");
    print_tuple(t1);

    std::tuple<int, int, int> t2(1, 2, 3);
    print_tuple(t2);

    std::cout << "\n4️⃣  CRTP 기반 비교:\n";
    Person p1("Alice", 30);
    Person p2("Bob", 25);
    std::cout << "p1 < p2: " << (p1 < p2) << "\n";
    std::cout << "p1 == p1: " << (p1 == p1) << "\n";
    std::cout << "p1 <= p2: " << (p1 <= p2) << "\n";

    std::cout << "\n5️⃣  다양한 타입의 CRTP:\n";
    Box b1(100.0);
    Box b2(150.0);
    std::cout << "b1 < b2: " << (b1 < b2) << "\n";
    std::cout << "b1 == b1: " << (b1 == b1) << "\n";

    std::cout << "\n✨ 컴파일 타임 정적 다형성으로 런타임 오버헤드 제거!\n";

    return 0;
}
