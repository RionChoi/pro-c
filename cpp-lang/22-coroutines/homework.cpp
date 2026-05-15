// 세션 22 — 핵심 과제: Generator (co_yield)
// 적용 개념: coroutine_handle, promise_type, co_yield, 지연 시퀀스
#include <iostream>
#include <coroutine>
#include <stdexcept>
#include <vector>

// ── 범용 Generator<T> ─────────────────────────────────────────────────────────
template <typename T>
struct Generator {
    struct promise_type {
        T current;

        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend()   noexcept { return {}; }
        std::suspend_always yield_value(T v) noexcept {
            current = v;
            return {};
        }
        void return_void() noexcept {}
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> handle;

    explicit Generator(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~Generator() { if (handle) handle.destroy(); }

    Generator(const Generator&)            = delete;
    Generator& operator=(const Generator&) = delete;
    Generator(Generator&& o) noexcept : handle(o.handle) { o.handle = nullptr; }

    // 다음 값으로 이동: 남은 값이 있으면 true
    bool next() {
        handle.resume();
        return !handle.done();
    }

    T value() const { return handle.promise().current; }
};

// ── 코루틴 함수들 ─────────────────────────────────────────────────────────────

// 1. 피보나치 수열 (무한 생성기)
Generator<long long> fibonacci() {
    long long a = 0, b = 1;
    while (true) {
        co_yield a;
        auto tmp = a + b;
        a = b;
        b = tmp;
    }
}

// 2. 정수 범위 생성기 [start, end)
Generator<int> range(int start, int end, int step = 1) {
    for (int i = start; i < end; i += step)
        co_yield i;
}

// 3. 벡터 요소 생성기 (순차 순회)
Generator<int> from_vector(const std::vector<int>& v) {
    for (int x : v)
        co_yield x;
}

// 4. 필터 생성기 (짝수만)
Generator<int> filter_even(Generator<int> source, int limit) {
    int count = 0;
    while (source.next() && count < limit) {
        int v = source.value();
        if (v % 2 == 0) {
            co_yield v;
            ++count;
        }
    }
}

// 5. FizzBuzz 생성기 (1~n 중 3 또는 5의 배수 인덱스 반환)
Generator<int> fizzbuzz_indices(int n) {
    for (int i = 1; i <= n; ++i)
        if (i % 3 == 0 || i % 5 == 0)
            co_yield i;
}

// ── main ──────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "=== Generator (co_yield) 과제 ===\n\n";

    // 1. 피보나치 수열 처음 10개
    std::cout << "[1] 피보나치 수열 (처음 10개)\n  ";
    auto fib = fibonacci();
    for (int i = 0; i < 10; ++i) {
        fib.next();
        std::cout << fib.value();
        if (i < 9) std::cout << ", ";
    }
    std::cout << "\n\n";

    // 2. range 생성기
    std::cout << "[2] range(0, 10, 2) — 짝수\n  ";
    auto r = range(0, 10, 2);
    while (r.next())
        std::cout << r.value() << " ";
    std::cout << "\n\n";

    std::cout << "  range(1, 8) — 1~7\n  ";
    auto r2 = range(1, 8);
    while (r2.next())
        std::cout << r2.value() << " ";
    std::cout << "\n\n";

    // 3. 벡터 순회
    std::cout << "[3] from_vector({5,3,8,1,9,2})\n  ";
    std::vector<int> v = {5, 3, 8, 1, 9, 2};
    auto vg = from_vector(v);
    while (vg.next())
        std::cout << vg.value() << " ";
    std::cout << "\n\n";

    // 4. 필터 생성기
    std::cout << "[4] range(1,20) 중 짝수 5개\n  ";
    auto evens = filter_even(range(1, 20), 5);
    while (evens.next())
        std::cout << evens.value() << " ";
    std::cout << "\n\n";

    // 5. FizzBuzz 인덱스 생성기
    std::cout << "[5] fizzbuzz_indices(20) — 3 또는 5의 배수\n  ";
    auto fb = fizzbuzz_indices(20);
    while (fb.next())
        std::cout << fb.value() << " ";
    std::cout << "\n\n";

    // 6. 피보나치 중 100 미만 수집
    std::cout << "[6] 피보나치 중 100 미만\n  ";
    auto fib2 = fibonacci();
    while (fib2.next()) {
        long long val = fib2.value();
        if (val >= 100) break;
        std::cout << val << " ";
    }
    std::cout << "\n";

    return 0;
}
