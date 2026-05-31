#include <iostream>
#include <tuple>
#include <string>
#include <vector>
#include <sstream>
#include <type_traits>

struct Product {
    int product_id;
    std::string product_name;
    double price;
    int stock;
};

// 튜플 요소별 문자열 변환
template <typename T>
std::string to_string_impl(const T& value) {
    if constexpr (std::is_same_v<T, int>) {
        return std::to_string(value);
    } else if constexpr (std::is_same_v<T, double>) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    } else if constexpr (std::is_same_v<T, std::string>) {
        return value;
    } else {
        return "unknown";
    }
}

// CSV 인코딩: 튜플 → CSV 문자열
template <typename Tuple, size_t... Is>
std::string tuple_to_csv(const Tuple& t, std::index_sequence<Is...>) {
    std::ostringstream oss;
    int count = 0;
    (..., (oss << (count++ > 0 ? "," : "") << to_string_impl(std::get<Is>(t))));
    return oss.str();
}

// Product 객체를 튜플로 변환
auto product_to_tuple(const Product& p) {
    return std::make_tuple(p.product_id, p.product_name, p.price, p.stock);
}

// CSV 문자열을 Product로 복원 (간단한 파서)
Product csv_to_product(const std::string& csv) {
    Product p{};
    std::istringstream iss(csv);
    std::string token;

    // product_id
    if (std::getline(iss, token, ',')) p.product_id = std::stoi(token);

    // product_name
    if (std::getline(iss, token, ',')) p.product_name = token;

    // price
    if (std::getline(iss, token, ',')) p.price = std::stod(token);

    // stock
    if (std::getline(iss, token, ',')) p.stock = std::stoi(token);

    return p;
}

int main() {
    std::cout << "=== 범용 직렬화 & CSV 형식 ===\n\n";

    std::vector<Product> products = {
        {1, "Laptop", 999.99, 5},
        {2, "Mouse", 29.99, 50},
        {3, "Keyboard", 79.99, 20},
    };

    std::cout << "원본 데이터:\n";
    for (const auto& p : products) {
        std::cout << "ID: " << p.product_id << ", 이름: " << p.product_name
                  << ", 가격: $" << p.price << ", 재고: " << p.stock << "\n";
    }

    // 직렬화 (CSV 형식)
    std::cout << "\nCSV 형식으로 직렬화:\n";
    std::vector<std::string> csv_lines;
    for (const auto& p : products) {
        auto tuple = product_to_tuple(p);
        using TupleType = decltype(tuple);
        auto csv = tuple_to_csv(tuple, std::make_index_sequence<std::tuple_size_v<TupleType>>{});
        csv_lines.push_back(csv);
        std::cout << csv << "\n";
    }

    // 역직렬화 (CSV 문자열 → Product)
    std::cout << "\nCSV에서 복원:\n";
    std::vector<Product> restored;
    for (const auto& csv : csv_lines) {
        restored.push_back(csv_to_product(csv));
    }

    std::cout << "복원된 데이터:\n";
    for (const auto& p : restored) {
        std::cout << "ID: " << p.product_id << ", 이름: " << p.product_name
                  << ", 가격: $" << p.price << ", 재고: " << p.stock << "\n";
    }

    return 0;
}
