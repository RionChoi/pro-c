// Session 42: C++20 Coroutines 심화 - Generator<T>와 co_yield
#include <coroutine>
#include <exception>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

template <typename T>
class Generator {
public:
    struct promise_type {
        std::optional<T> current;

        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T value) noexcept {
            current = std::move(value);
            return {};
        }
        void return_void() noexcept {}
        void unhandled_exception() { std::terminate(); }
    };

    explicit Generator(std::coroutine_handle<promise_type> handle) : handle_(handle) {}
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;
    Generator(Generator&& other) noexcept : handle_(std::exchange(other.handle_, {})) {}
    Generator& operator=(Generator&& other) noexcept {
        if (this != &other) {
            if (handle_) {
                handle_.destroy();
            }
            handle_ = std::exchange(other.handle_, {});
        }
        return *this;
    }
    ~Generator() {
        if (handle_) {
            handle_.destroy();
        }
    }

    bool next() {
        if (!handle_ || handle_.done()) {
            return false;
        }
        handle_.resume();
        return !handle_.done();
    }

    const T& value() const {
        return *handle_.promise().current;
    }

private:
    std::coroutine_handle<promise_type> handle_;
};

Generator<int> fibonacci(int count) {
    int a = 0;
    int b = 1;
    for (int i = 0; i < count; ++i) {
        co_yield a;
        const int next = a + b;
        a = b;
        b = next;
    }
}

Generator<double> running_average(const std::vector<int>& values) {
    int sum = 0;
    for (std::size_t i = 0; i < values.size(); ++i) {
        sum += values[i];
        co_yield static_cast<double>(sum) / static_cast<double>(i + 1);
    }
}

Generator<std::string> log_events() {
    co_yield "connect";
    co_yield "authenticate";
    co_yield "load profile";
    co_yield "ready";
}

int main() {
    std::cout << "=== Generator<int>: Fibonacci ===\n";
    auto fib = fibonacci(10);
    while (fib.next()) {
        std::cout << fib.value() << ' ';
    }
    std::cout << "\n\n";

    std::cout << "=== Generator<double>: running average ===\n";
    std::vector<int> scores{80, 75, 92, 88, 95};
    auto averages = running_average(scores);
    while (averages.next()) {
        std::cout << averages.value() << '\n';
    }
    std::cout << '\n';

    std::cout << "=== Generator<string>: event stream ===\n";
    auto events = log_events();
    while (events.next()) {
        std::cout << "- " << events.value() << '\n';
    }
}
