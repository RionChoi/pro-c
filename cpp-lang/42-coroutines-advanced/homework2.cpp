// Session 42: C++20 Coroutines 심화 - Task<T>와 커스텀 promise_type
#include <coroutine>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

template <typename T>
class Task {
public:
    struct promise_type {
        std::optional<T> result;
        std::exception_ptr error;

        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(T value) noexcept { result = std::move(value); }
        void unhandled_exception() noexcept { error = std::current_exception(); }
    };

    explicit Task(std::coroutine_handle<promise_type> handle) : handle_(handle) {}
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&& other) noexcept : handle_(std::exchange(other.handle_, {})) {}
    ~Task() {
        if (handle_) {
            handle_.destroy();
        }
    }

    T get() {
        if (!handle_.done()) {
            handle_.resume();
        }
        if (handle_.promise().error) {
            std::rethrow_exception(handle_.promise().error);
        }
        return *handle_.promise().result;
    }

private:
    std::coroutine_handle<promise_type> handle_;
};

struct ManualDelay {
    std::string label;

    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> /*unused*/) const {
        std::cout << "  await: " << label << " 작업 대기 지점\n";
    }
    std::string await_resume() const {
        return label + " 완료";
    }
};

Task<std::string> fetch_profile(int user_id) {
    const std::string ready = co_await ManualDelay{"profile fetch"};
    co_return ready + " for user#" + std::to_string(user_id);
}

Task<int> calculate_bonus(std::vector<int> scores) {
    if (scores.empty()) {
        throw std::runtime_error("score list is empty");
    }

    int sum = 0;
    for (int score : scores) {
        sum += score;
    }
    co_return sum / static_cast<int>(scores.size());
}

Task<std::string> build_report(int user_id, std::vector<int> scores) {
    const std::string profile = co_await ManualDelay{"report header"};
    int average = 0;
    for (int score : scores) {
        average += score;
    }
    average /= static_cast<int>(scores.size());
    co_return profile + " user#" + std::to_string(user_id)
            + " average=" + std::to_string(average);
}

int main() {
    std::cout << "=== Task<T> demo ===\n";

    auto profile = fetch_profile(42);
    std::cout << profile.get() << '\n';

    auto bonus = calculate_bonus({92, 88, 95});
    std::cout << "bonus score = " << bonus.get() << '\n';

    auto report = build_report(7, {80, 90, 100});
    std::cout << report.get() << '\n';

    try {
        auto broken = calculate_bonus({});
        std::cout << broken.get() << '\n';
    } catch (const std::exception& e) {
        std::cout << "caught error: " << e.what() << '\n';
    }
}
