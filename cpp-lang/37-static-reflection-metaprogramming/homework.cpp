#include <iostream>
#include <string>
#include <typeinfo>
#include <tuple>

struct Person {
    int id;
    std::string name;
    double height;
    bool active;
};

struct Product {
    int sku;
    std::string title;
    double price;
};

template <typename T>
void reflectStructure(const T& obj) {
    std::cout << "\n=== Reflecting: " << typeid(T).name() << " ===\n";
}

template <>
void reflectStructure<Person>(const Person& obj) {
    std::cout << "\n=== Reflecting: Person ===\n";

    auto [id, name, height, active] = obj;

    std::cout << "Member count: 4\n";
    std::cout << "1. id (int) - size: " << sizeof(int) << " bytes\n";
    std::cout << "2. name (std::string) - size: " << sizeof(std::string) << " bytes\n";
    std::cout << "3. height (double) - size: " << sizeof(double) << " bytes\n";
    std::cout << "4. active (bool) - size: " << sizeof(bool) << " bytes\n";

    std::cout << "\nTotal struct size: " << sizeof(Person) << " bytes\n";

    std::cout << "\nMember values:\n";
    std::cout << "  id: " << id << "\n";
    std::cout << "  name: \"" << name << "\"\n";
    std::cout << "  height: " << height << " cm\n";
    std::cout << "  active: " << (active ? "true" : "false") << "\n";
}

template <>
void reflectStructure<Product>(const Product& obj) {
    std::cout << "\n=== Reflecting: Product ===\n";

    auto [sku, title, price] = obj;

    std::cout << "Member count: 3\n";
    std::cout << "1. sku (int) - size: " << sizeof(int) << " bytes\n";
    std::cout << "2. title (std::string) - size: " << sizeof(std::string) << " bytes\n";
    std::cout << "3. price (double) - size: " << sizeof(double) << " bytes\n";

    std::cout << "\nTotal struct size: " << sizeof(Product) << " bytes\n";

    std::cout << "\nMember values:\n";
    std::cout << "  sku: " << sku << "\n";
    std::cout << "  title: \"" << title << "\"\n";
    std::cout << "  price: $" << price << "\n";
}

int main() {
    std::cout << "=== 정적 리플렉션 테스트 ===\n";

    Person person{42, "Alice", 172.5, true};
    Product product{101, "Gaming Mouse", 49.99};

    reflectStructure(person);
    reflectStructure(product);

    std::cout << "\n=== 메모리 레이아웃 비교 ===\n";
    std::cout << "Person 구조체:\n";
    std::cout << "  declared members size: "
              << (sizeof(int) + sizeof(std::string) + sizeof(double) + sizeof(bool))
              << " bytes\n";
    std::cout << "  actual struct size: " << sizeof(Person) << " bytes\n";
    std::cout << "  padding bytes: " << (sizeof(Person) - (sizeof(int) + sizeof(std::string) + sizeof(double) + sizeof(bool)))
              << " bytes\n";

    return 0;
}
