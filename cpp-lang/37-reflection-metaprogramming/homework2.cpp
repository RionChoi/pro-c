#include <iostream>
#include <concepts>
#include <string>
#include <vector>

template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template<typename T>
concept Stringlike = std::same_as<T, std::string> || std::same_as<T, std::string_view>;

template<typename T>
concept Printable = requires(T t, std::ostream& os) {
    { os << t } -> std::same_as<std::ostream&>;
};

template<typename T>
concept Container = !Stringlike<T> && requires(T c) {
    { c.size() } -> std::integral;
    { c.begin() };
    { c.end() };
};

template<Numeric T>
void handle(T value) {
    std::cout << "숫자: " << value << '\n';
}

template<Stringlike T>
void handle(const T& value) {
    std::cout << "문자열: " << value << '\n';
}

template<Container C>
void handle(const C& container) {
    std::cout << "컨테이너 (크기: " << container.size() << ")\n";
}

template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::same_as<T>;
};

template<Addable T>
T add_values(T a, T b) {
    return a + b;
}

template<typename T>
concept HasSize = requires(T obj) {
    { obj.size() } -> std::integral;
};

template<HasSize T>
std::size_t get_size(const T& obj) {
    return obj.size();
}

int main() {
    std::cout << "=== Concepts 데모 ===\n";

    handle(42);
    handle(3.14);
    handle(std::string("Hello World"));

    std::vector<int> vec{1, 2, 3, 4};
    handle(vec);

    std::cout << "\n=== Addable Concept ===\n";
    std::cout << "5 + 3 = " << add_values(5, 3) << '\n';
    std::cout << "2.5 + 1.5 = " << add_values(2.5, 1.5) << '\n';

    std::string str = "Hello";
    std::cout << "문자열 + 문자열: " << add_values(str, std::string(" C++")) << '\n';

    std::cout << "\n=== HasSize Concept ===\n";
    std::cout << "벡터 크기: " << get_size(vec) << '\n';
    std::cout << "문자열 크기: " << get_size(std::string("Concept")) << '\n';

    std::cout << "\n=== Printable Concept ===\n";
    std::cout << "정수 (Printable): " << 99 << '\n';
    std::cout << "문자열 (Printable): " << std::string("Good") << '\n';

    return 0;
}
