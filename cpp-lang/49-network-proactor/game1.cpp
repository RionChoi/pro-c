// Session 49: C++ 네트워크 & Proactor 패턴 - 코루틴 I/O 미니 시뮬레이션
#include <coroutine>
#include <functional>
#include <iostream>
#include <optional>
#include <queue>
#include <string>
#include <utility>

class EventLoop {
public:
    void post(std::function<void()> fn) {
        queue_.push(std::move(fn));
    }

    void run() {
        while (!queue_.empty()) {
            auto fn = std::move(queue_.front());
            queue_.pop();
            fn();
        }
    }

private:
    std::queue<std::function<void()>> queue_;
};

template <typename T>
class Task {
public:
    struct promise_type {
        std::optional<T> result;

        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(T value) { result = std::move(value); }
        void unhandled_exception() { std::terminate(); }
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

    const T& result() const {
        return *handle_.promise().result;
    }

private:
    std::coroutine_handle<promise_type> handle_;
};

struct AsyncRead {
    EventLoop& loop;
    std::string payload;

    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> handle) {
        loop.post([handle]() mutable { handle.resume(); });
    }
    std::string await_resume() const {
        return payload;
    }
};

Task<std::string> handle_request(EventLoop& loop, std::string input) {
    std::string request = co_await AsyncRead{loop, std::move(input)};
    co_return "response(" + request + ")";
}

int main() {
    EventLoop loop;
    auto task = handle_request(loop, "GET /metrics");
    loop.run();
    std::cout << task.result() << '\n';
}
