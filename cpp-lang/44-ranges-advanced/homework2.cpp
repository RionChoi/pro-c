// Session 44: C++20 Ranges 심화 - range adaptor와 파이프라인 최적화
#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>

struct Order {
    int id;
    std::string region;
    int amount;
    bool paid;
};

template <typename Predicate>
auto paid_orders_in(Predicate region_predicate) {
    return std::views::filter([=](const Order& order) {
        return order.paid && region_predicate(order.region);
    });
}

auto amount_view() {
    return std::views::transform([](const Order& order) {
        return order.amount;
    });
}

template <std::ranges::input_range R>
int sum_range(R&& range) {
    return std::accumulate(std::ranges::begin(range), std::ranges::end(range), 0);
}

int main() {
    std::vector<Order> orders{
        {101, "KR", 42000, true},
        {102, "US", 38000, true},
        {103, "KR", 12000, false},
        {104, "JP", 51000, true},
        {105, "KR", 70000, true},
        {106, "US", 15000, false},
        {107, "KR", 18000, true}
    };

    auto is_korea = [](const std::string& region) {
        return region == "KR";
    };

    auto korea_paid_amounts = orders
        | paid_orders_in(is_korea)
        | amount_view();

    std::cout << "KR paid amounts: ";
    for (int amount : korea_paid_amounts) {
        std::cout << amount << ' ';
    }
    std::cout << "\nKR paid total: " << sum_range(korea_paid_amounts) << '\n';

    std::ranges::sort(orders, std::ranges::greater{}, &Order::amount);
    std::cout << "\nTop paid orders:\n";
    for (const auto& order : orders
        | std::views::filter(&Order::paid)
        | std::views::take(3)) {
        std::cout << "  #" << order.id << " " << order.region
                  << " " << order.amount << '\n';
    }
}
