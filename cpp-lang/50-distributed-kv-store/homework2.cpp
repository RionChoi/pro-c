// Session 50: Distributed KV Store - arena, frame protocol, coroutine task
#include <coroutine>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <optional>
#include <queue>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

class LinearArena {
public:
    explicit LinearArena(std::size_t bytes) : buffer_(bytes) {}

    std::string_view copy(std::string_view text) {
        if (offset_ + text.size() > buffer_.size()) {
            throw std::bad_alloc{};
        }
        char* dst = reinterpret_cast<char*>(buffer_.data() + offset_);
        std::memcpy(dst, text.data(), text.size());
        offset_ += text.size();
        return {dst, text.size()};
    }

    void reset() { offset_ = 0; }
    std::size_t used() const { return offset_; }

private:
    std::vector<std::byte> buffer_;
    std::size_t offset_{0};
};

class CompletionQueue {
public:
    void push(std::function<void()> fn) {
        queue_.push(std::move(fn));
    }

    void drain() {
        while (!queue_.empty()) {
            auto fn = std::move(queue_.front());
            queue_.pop();
            fn();
        }
    }

private:
    std::queue<std::function<void()>> queue_;
};

enum class Op : std::uint8_t {
    Put = 1,
    Get = 2,
    Erase = 3
};

struct Request {
    Op op;
    std::string key;
    std::string value;
};

std::vector<std::uint8_t> encode(const Request& request) {
    std::vector<std::uint8_t> out;
    out.push_back(static_cast<std::uint8_t>(request.op));
    out.push_back(static_cast<std::uint8_t>(request.key.size()));
    out.push_back(static_cast<std::uint8_t>(request.value.size()));
    out.insert(out.end(), request.key.begin(), request.key.end());
    out.insert(out.end(), request.value.begin(), request.value.end());
    return out;
}

Request decode(std::span<const std::uint8_t> bytes) {
    if (bytes.size() < 3) {
        throw std::runtime_error("short frame");
    }

    const auto op = static_cast<Op>(bytes[0]);
    const std::size_t key_size = bytes[1];
    const std::size_t value_size = bytes[2];
    if (bytes.size() != 3 + key_size + value_size) {
        throw std::runtime_error("bad frame length");
    }

    std::string key(bytes.begin() + 3, bytes.begin() + 3 + static_cast<std::ptrdiff_t>(key_size));
    std::string value(bytes.begin() + 3 + static_cast<std::ptrdiff_t>(key_size), bytes.end());
    return {op, std::move(key), std::move(value)};
}

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

    const T& result() const { return *handle_.promise().result; }

private:
    std::coroutine_handle<promise_type> handle_;
};

struct QueueAwaiter {
    CompletionQueue& queue;

    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> handle) {
        queue.push([handle]() mutable { handle.resume(); });
    }
    void await_resume() const noexcept {}
};

class KvEngine {
public:
    Task<std::string> handle(CompletionQueue& queue, LinearArena& arena, std::vector<std::uint8_t> frame) {
        co_await QueueAwaiter{queue};
        Request request = decode(frame);
        const std::string_view key = arena.copy(request.key);

        if (request.op == Op::Put) {
            store_[std::string(key)] = request.value;
            co_return "OK put " + std::string(key);
        }
        if (request.op == Op::Get) {
            auto it = store_.find(std::string(key));
            co_return it == store_.end() ? "MISS " + std::string(key)
                                         : "VALUE " + std::string(key) + "=" + it->second;
        }
        store_.erase(std::string(key));
        co_return "OK erase " + std::string(key);
    }

private:
    std::unordered_map<std::string, std::string> store_;
};

int main() {
    CompletionQueue queue;
    LinearArena arena{1024};
    KvEngine engine;

    auto put = engine.handle(queue, arena, encode({Op::Put, "session", "active"}));
    queue.drain();
    std::cout << put.result() << '\n';

    arena.reset();
    auto get = engine.handle(queue, arena, encode({Op::Get, "session", ""}));
    queue.drain();
    std::cout << get.result() << '\n';

    std::cout << "arena used: " << arena.used() << " bytes\n";
}
