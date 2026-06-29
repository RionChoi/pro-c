#include "mini_exec.hpp"

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

struct QuoteRequest {
    std::string item;
    int quantity;
    int unitPrice;
};

QuoteRequest validate(QuoteRequest request)
{
    if (request.item.empty()) {
        throw std::runtime_error("item name is required");
    }
    if (request.quantity <= 0 || request.unitPrice <= 0) {
        throw std::runtime_error("quantity and price must be positive");
    }
    return request;
}

QuoteRequest applyDiscount(QuoteRequest request)
{
    if (request.quantity >= 10) {
        request.unitPrice -= 200;
    }
    return request;
}

std::string buildQuote(const QuoteRequest& request)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(40));

    std::ostringstream out;
    out << request.item << " x" << request.quantity
        << " total=" << request.quantity * request.unitPrice
        << " worker=" << std::this_thread::get_id();
    return out.str();
}

int main()
{
    mini_exec::ThreadPool pool(2);

    auto quote = mini_exec::just(QuoteRequest{"NVMe SSD", 12, 1500})
               | mini_exec::then(validate)
               | mini_exec::then(applyDiscount)
               | mini_exec::then(buildQuote);

    auto offloaded = mini_exec::on(pool, std::move(quote));

    std::cout << "main worker=" << std::this_thread::get_id() << '\n';
    std::cout << mini_exec::sync_wait(std::move(offloaded)) << '\n';
    return 0;
}
