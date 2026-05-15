// 세션 22 — 확장 과제: Task (co_return / co_await)
// 적용 개념: Lazy<T>, EagerTask<T>, co_return, 커스텀 Awaitable, 지연 계산
#include <iostream>
#include <coroutine>
#include <string>
#include <vector>

// ── Lazy<T>: 처음 .get() 호출 시 계산되는 지연 값 ────────────────────────────
// T는 기본 생성 가능해야 함 (int, bool, double, long long 등)
template <typename T>
struct Lazy {
    struct promise_type {
        T    stored{};   // 결과 저장
        bool ready = false;

        Lazy get_return_object() {
            return Lazy{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept { return {}; }  // 즉시 중단
        std::suspend_always final_suspend()   noexcept { return {}; }
        void return_value(T v) { stored = v; ready = true; }
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> handle;

    explicit Lazy(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~Lazy() { if (handle) handle.destroy(); }
    Lazy(const Lazy&)            = delete;
    Lazy& operator=(const Lazy&) = delete;
    Lazy(Lazy&& o) noexcept : handle(o.handle) { o.handle = nullptr; }

    T get() {
        if (!handle.promise().ready)
            handle.resume();
        return handle.promise().stored;
    }
};

// ── EagerTask<T>: 생성 즉시 실행, co_return으로 결과 반환 ────────────────────
template <typename T>
struct EagerTask {
    struct promise_type {
        T    stored{};
        bool ready = false;

        EagerTask get_return_object() {
            return EagerTask{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never  initial_suspend() noexcept { return {}; }  // 즉시 실행
        std::suspend_always final_suspend()   noexcept { return {}; }
        void return_value(T v) { stored = v; ready = true; }
        void unhandled_exception() { std::terminate(); }
    };

    std::coroutine_handle<promise_type> handle;

    explicit EagerTask(std::coroutine_handle<promise_type> h) : handle(h) {}
    ~EagerTask() { if (handle) handle.destroy(); }
    EagerTask(const EagerTask&)            = delete;
    EagerTask& operator=(const EagerTask&) = delete;

    T get() const { return handle.promise().stored; }
};

// ── 커스텀 Awaitable: 즉시 값을 제공 ─────────────────────────────────────────
template <typename T>
struct Immediate {
    T val;
    bool await_ready()  const noexcept { return true; }
    void await_suspend(std::coroutine_handle<>) const noexcept {}
    T    await_resume() const noexcept { return val; }
};

template <typename T>
Immediate<T> make_ready(T v) { return {std::move(v)}; }

// ── 코루틴 함수들 ─────────────────────────────────────────────────────────────

// 지연 계산: n 팩토리얼
Lazy<long long> factorial(int n) {
    long long product = 1;
    for (int i = 2; i <= n; ++i)
        product *= i;
    co_return product;
}

// 지연 계산: 소수 판별
Lazy<bool> is_prime(int n) {
    if (n < 2) {
        co_return false;
    }
    bool prime = true;
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0) { prime = false; break; }
    }
    co_return prime;
}

// 지연 계산: 벡터 합
Lazy<int> sum_vector(std::vector<int> nums) {
    int total = 0;
    for (int x : nums) total += x;
    co_return total;
}

// EagerTask: co_await Immediate 사용
EagerTask<int> compute(int a, int b) {
    int x = co_await make_ready(a * 2);
    int y = co_await make_ready(b * 3);
    co_return x + y;
}

// EagerTask: 최솟값/최댓값
EagerTask<int> range_diff(int lo, int hi) {
    int mn = co_await make_ready(lo);
    int mx = co_await make_ready(hi);
    co_return mx - mn;
}

// ── main ──────────────────────────────────────────────────────────────────────
int main() {
    std::cout << "=== Task (co_return / co_await) 과제 ===\n\n";

    // 1. Lazy<long long> — 팩토리얼 지연 계산
    std::cout << "[1] Lazy<long long> — 팩토리얼\n";
    for (int n : {0, 1, 5, 10, 12}) {
        auto f = factorial(n);
        std::cout << "  " << n << "! = " << f.get() << "\n";
    }

    // 2. Lazy<bool> — 소수 판별
    std::cout << "\n[2] Lazy<bool> — 소수 판별\n";
    for (int n : {2, 7, 15, 17, 100, 97}) {
        auto p = is_prime(n);
        std::cout << "  is_prime(" << n << ") = "
                  << (p.get() ? "소수" : "합성수") << "\n";
    }

    // 3. Lazy<int> — 벡터 합
    std::cout << "\n[3] Lazy<int> — 벡터 합 (지연 계산)\n";
    {
        std::cout << "  Lazy 생성 (아직 계산 안 함)...\n";
        auto s = sum_vector({10, 20, 30, 40, 50});
        std::cout << "  .get() 호출 → 계산\n";
        std::cout << "  합 = " << s.get() << "\n";
    }

    // 4. 여러 Lazy 독립 인스턴스
    std::cout << "\n[4] 여러 Lazy 독립 인스턴스\n";
    {
        auto f5  = factorial(5);
        auto f8  = factorial(8);
        auto f10 = factorial(10);
        // 아직 아무것도 계산되지 않음
        std::cout << "  5!  = " << f5.get()  << "\n";
        std::cout << "  8!  = " << f8.get()  << "\n";
        std::cout << "  10! = " << f10.get() << "\n";
    }

    // 5. EagerTask — 즉시 실행 + co_await Immediate
    std::cout << "\n[5] EagerTask — 즉시 실행 (co_await Immediate)\n";
    {
        auto t1 = compute(3, 4);    // 3*2 + 4*3 = 6+12 = 18
        auto t2 = compute(10, 5);   // 10*2 + 5*3 = 20+15 = 35
        std::cout << "  compute(3, 4)   = " << t1.get() << "  (기대: 18)\n";
        std::cout << "  compute(10, 5)  = " << t2.get() << "  (기대: 35)\n";
    }

    // 6. EagerTask — range_diff
    std::cout << "\n[6] EagerTask — range_diff\n";
    {
        auto d1 = range_diff(5, 20);
        auto d2 = range_diff(100, 999);
        std::cout << "  range_diff(5, 20)    = " << d1.get() << "\n";
        std::cout << "  range_diff(100, 999) = " << d2.get() << "\n";
    }

    return 0;
}
