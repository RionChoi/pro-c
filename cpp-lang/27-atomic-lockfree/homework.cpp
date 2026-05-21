// 세션 27 과제 1: 원자 연산 — 공유 카운터 & 은행 계좌
//
// 목표:
//  1. std::atomic<int>으로 데이터 경쟁 없이 카운터를 증가시킨다.
//  2. 은행 계좌에서 여러 스레드가 동시에 입금/출금해도 잔액이 정확히 유지됨을 확인한다.
//  3. memory_order_relaxed vs memory_order_seq_cst 성능 차이를 측정한다.

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

// ── 1. 공유 카운터 ───────────────────────────────────────────────────────────

constexpr int THREADS     = 8;
constexpr int OPS_PER_THR = 100'000;

// seq_cst 카운터
std::atomic<long long> counter_seq{0};

// relaxed 카운터 (순서 보장 없음 — 최종 합산은 정확)
std::atomic<long long> counter_rel{0};

void increment_seq() {
    for (int i = 0; i < OPS_PER_THR; ++i)
        counter_seq.fetch_add(1);  // 기본값 = seq_cst
}

void increment_rel() {
    for (int i = 0; i < OPS_PER_THR; ++i)
        counter_rel.fetch_add(1, std::memory_order_relaxed);
}

void run_counter_benchmark() {
    std::cout << "=== 공유 카운터 벤치마크 ===\n";

    // seq_cst
    {
        std::vector<std::thread> threads;
        auto t0 = std::chrono::steady_clock::now();
        for (int i = 0; i < THREADS; ++i)
            threads.emplace_back(increment_seq);
        for (auto& t : threads) t.join();
        auto t1 = std::chrono::steady_clock::now();
        long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        std::cout << "seq_cst  결과: " << counter_seq.load()
                  << "  (기대값: " << (long long)THREADS * OPS_PER_THR << ")"
                  << "  시간: " << ms << "ms\n";
    }

    // relaxed
    {
        std::vector<std::thread> threads;
        auto t0 = std::chrono::steady_clock::now();
        for (int i = 0; i < THREADS; ++i)
            threads.emplace_back(increment_rel);
        for (auto& t : threads) t.join();
        auto t1 = std::chrono::steady_clock::now();
        long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        std::cout << "relaxed  결과: " << counter_rel.load()
                  << "  (기대값: " << (long long)THREADS * OPS_PER_THR << ")"
                  << "  시간: " << ms << "ms\n";
    }
}

// ── 2. 원자 연산 기반 은행 계좌 ─────────────────────────────────────────────

class AtomicBankAccount {
public:
    explicit AtomicBankAccount(long long init) : balance_{init} {}

    // 입금 (항상 성공)
    void deposit(long long amount) {
        balance_.fetch_add(amount, std::memory_order_acq_rel);
    }

    // 출금 — 잔액 부족 시 실패 반환
    bool withdraw(long long amount) {
        long long cur = balance_.load(std::memory_order_acquire);
        while (cur >= amount) {
            if (balance_.compare_exchange_weak(cur, cur - amount,
                    std::memory_order_acq_rel, std::memory_order_acquire))
                return true;
            // cur는 현재 실제 값으로 업데이트됨 — 루프 재시도
        }
        return false;  // 잔액 부족
    }

    long long get() const {
        return balance_.load(std::memory_order_acquire);
    }

private:
    std::atomic<long long> balance_;
};

void run_bank_simulation() {
    std::cout << "\n=== 은행 계좌 시뮬레이션 ===\n";

    AtomicBankAccount account{10'000};
    std::cout << "초기 잔액: " << account.get() << "원\n";

    constexpr int DEPOSITORS  = 4;
    constexpr int WITHDRAWERS = 4;
    constexpr long long DEP_AMOUNT = 500;
    constexpr long long WIT_AMOUNT = 300;
    constexpr int ROUNDS = 50;

    std::atomic<int> success_dep{0}, success_wit{0}, fail_wit{0};

    auto depositor = [&]() {
        for (int i = 0; i < ROUNDS; ++i) {
            account.deposit(DEP_AMOUNT);
            success_dep.fetch_add(1, std::memory_order_relaxed);
        }
    };

    auto withdrawer = [&]() {
        for (int i = 0; i < ROUNDS; ++i) {
            if (account.withdraw(WIT_AMOUNT))
                success_wit.fetch_add(1, std::memory_order_relaxed);
            else
                fail_wit.fetch_add(1, std::memory_order_relaxed);
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < DEPOSITORS;  ++i) threads.emplace_back(depositor);
    for (int i = 0; i < WITHDRAWERS; ++i) threads.emplace_back(withdrawer);
    for (auto& t : threads) t.join();

    long long expected = 10'000LL
        + (long long)success_dep.load() * DEP_AMOUNT
        - (long long)success_wit.load() * WIT_AMOUNT;

    std::cout << "입금 성공: " << success_dep.load() << "회\n";
    std::cout << "출금 성공: " << success_wit.load() << "회\n";
    std::cout << "출금 실패(잔액 부족): " << fail_wit.load() << "회\n";
    std::cout << "최종 잔액: " << account.get() << "원\n";
    std::cout << "기대 잔액: " << expected << "원\n";
    std::cout << (account.get() == expected ? "✓ 정확함\n" : "✗ 불일치!\n");
}

// ── 3. atomic_flag 스핀락 ───────────────────────────────────────────────────

class Spinlock {
public:
    void lock() {
        // test_and_set이 false를 반환할 때까지(= 락을 획득할 때까지) 바쁜 대기
        while (flag_.test_and_set(std::memory_order_acquire))
            ;
    }
    void unlock() {
        flag_.clear(std::memory_order_release);
    }
private:
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
};

void run_spinlock_demo() {
    std::cout << "\n=== atomic_flag 스핀락 데모 ===\n";

    Spinlock spin;
    long long shared = 0;

    auto worker = [&]() {
        for (int i = 0; i < 10'000; ++i) {
            spin.lock();
            ++shared;
            spin.unlock();
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) threads.emplace_back(worker);
    for (auto& t : threads) t.join();

    std::cout << "shared = " << shared << "  (기대: 40000)\n";
    std::cout << (shared == 40000 ? "✓ 정확함\n" : "✗ 불일치!\n");
}

int main() {
    run_counter_benchmark();
    run_bank_simulation();
    run_spinlock_demo();
    return 0;
}
