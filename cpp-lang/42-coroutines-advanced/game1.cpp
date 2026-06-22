// Session 42: C++20 Coroutines 심화 - 코루틴 이벤트 기반 전투 시뮬레이션
#include <coroutine>
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
    const T& value() const { return *handle_.promise().current; }

private:
    std::coroutine_handle<promise_type> handle_;
};

template <typename T>
class Task {
public:
    struct promise_type {
        std::optional<T> result;

        Task get_return_object() {
            return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(T value) noexcept { result = std::move(value); }
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

    T get() {
        if (!handle_.done()) {
            handle_.resume();
        }
        return *handle_.promise().result;
    }

private:
    std::coroutine_handle<promise_type> handle_;
};

struct CombatEvent {
    std::string actor;
    std::string action;
    int damage;
};

struct Character {
    std::string name;
    int hp;
    int attack;
};

Generator<CombatEvent> scripted_turns() {
    co_yield {"Knight", "slash", 18};
    co_yield {"Bandit", "counter", 7};
    co_yield {"Knight", "shield bash", 12};
    co_yield {"Bandit", "desperate strike", 10};
    co_yield {"Knight", "finisher", 25};
}

Task<std::string> resolve_battle(Character hero, Character enemy) {
    auto events = scripted_turns();
    while (events.next()) {
        const CombatEvent& event = events.value();
        if (event.actor == hero.name) {
            enemy.hp -= event.damage;
        } else {
            hero.hp -= event.damage;
        }

        std::cout << event.actor << " uses " << event.action
                  << " for " << event.damage << " damage\n";
        std::cout << "  " << hero.name << " HP=" << hero.hp
                  << ", " << enemy.name << " HP=" << enemy.hp << '\n';

        if (hero.hp <= 0 || enemy.hp <= 0) {
            break;
        }
    }

    if (hero.hp > 0 && enemy.hp <= 0) {
        co_return hero.name + " wins";
    }
    if (enemy.hp > 0 && hero.hp <= 0) {
        co_return enemy.name + " wins";
    }
    co_return "battle ends without a knockout";
}

int main() {
    Character hero{"Knight", 50, 20};
    Character enemy{"Bandit", 40, 9};

    std::cout << "=== Coroutine battle simulator ===\n";
    auto result = resolve_battle(hero, enemy);
    std::cout << "Result: " << result.get() << '\n';
}
