// 세션 27 과제 2: Lock-free 스택 (Treiber Stack) & 링 버퍼
//
// 목표:
//  1. compare_exchange_weak로 Treiber Stack(lock-free 스택)을 구현한다.
//  2. 생산자/소비자 시나리오로 정확성을 검증한다.
//  3. 단순 원자 인덱스를 이용한 Single-Producer Single-Consumer(SPSC) 링 버퍼를 구현한다.

#include <atomic>
#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <thread>
#include <vector>

// ── 1. Lock-free 스택 (Treiber Stack) ───────────────────────────────────────

template<typename T>
class TreiberStack {
    struct Node {
        T    data;
        Node* next;
        explicit Node(T val) : data(std::move(val)), next(nullptr) {}
    };

    std::atomic<Node*> head_{nullptr};

public:
    ~TreiberStack() {
        // 남은 노드 정리
        Node* cur = head_.load(std::memory_order_relaxed);
        while (cur) {
            Node* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
    }

    void push(T val) {
        Node* node = new Node(std::move(val));
        node->next = head_.load(std::memory_order_relaxed);
        // head_를 node->next로 기대하고 node로 교체 — 실패 시 node->next 갱신 후 재시도
        while (!head_.compare_exchange_weak(node->next, node,
                std::memory_order_release, std::memory_order_relaxed))
            ;
    }

    std::optional<T> pop() {
        Node* old = head_.load(std::memory_order_acquire);
        while (old) {
            if (head_.compare_exchange_weak(old, old->next,
                    std::memory_order_acquire, std::memory_order_relaxed)) {
                T val = std::move(old->data);
                delete old;
                return val;
            }
            // old는 현재 head로 갱신됨 — 루프 재시도
        }
        return std::nullopt;  // 스택 비어 있음
    }

    bool empty() const {
        return head_.load(std::memory_order_acquire) == nullptr;
    }
};

void test_treiber_stack() {
    std::cout << "=== Treiber Stack 테스트 ===\n";

    constexpr int PRODUCERS = 4;
    constexpr int ITEMS_PER = 1000;

    TreiberStack<int> stack;
    std::atomic<int>  total_pushed{0};
    std::atomic<int>  total_popped{0};
    std::atomic<long long> sum_pushed{0};
    std::atomic<long long> sum_popped{0};

    // 생산자: 각자 ITEMS_PER개의 값을 push
    auto producer = [&](int id) {
        int base = id * ITEMS_PER;
        for (int i = 0; i < ITEMS_PER; ++i) {
            stack.push(base + i);
            total_pushed.fetch_add(1, std::memory_order_relaxed);
            sum_pushed.fetch_add(base + i, std::memory_order_relaxed);
        }
    };

    // 소비자: 스택이 빌 때까지 pop
    auto consumer = [&]() {
        while (total_popped.load(std::memory_order_relaxed) < PRODUCERS * ITEMS_PER) {
            auto val = stack.pop();
            if (val) {
                total_popped.fetch_add(1, std::memory_order_relaxed);
                sum_popped.fetch_add(*val, std::memory_order_relaxed);
            }
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < PRODUCERS; ++i) threads.emplace_back(producer, i);
    threads.emplace_back(consumer);
    for (auto& t : threads) t.join();

    std::cout << "push 횟수: " << total_pushed.load() << "\n";
    std::cout << "pop  횟수: " << total_popped.load() << "\n";
    std::cout << "push 합계: " << sum_pushed.load() << "\n";
    std::cout << "pop  합계: " << sum_popped.load() << "\n";
    std::cout << (sum_pushed.load() == sum_popped.load() ? "✓ 합계 일치\n" : "✗ 합계 불일치!\n");
}

// ── 2. SPSC 링 버퍼 (Single-Producer Single-Consumer) ───────────────────────

template<typename T, std::size_t CAP>
class SPSCQueue {
    static_assert((CAP & (CAP - 1)) == 0, "CAP은 2의 거듭제곱이어야 합니다");

    T    buf_[CAP]{};
    std::atomic<std::size_t> head_{0};  // 생산자가 씀
    std::atomic<std::size_t> tail_{0};  // 소비자가 읽음

public:
    bool enqueue(T val) {
        std::size_t h = head_.load(std::memory_order_relaxed);
        std::size_t next = (h + 1) & (CAP - 1);
        if (next == tail_.load(std::memory_order_acquire))
            return false;  // 꽉 찼음
        buf_[h] = std::move(val);
        head_.store(next, std::memory_order_release);
        return true;
    }

    std::optional<T> dequeue() {
        std::size_t t = tail_.load(std::memory_order_relaxed);
        if (t == head_.load(std::memory_order_acquire))
            return std::nullopt;  // 비어 있음
        T val = std::move(buf_[t]);
        tail_.store((t + 1) & (CAP - 1), std::memory_order_release);
        return val;
    }
};

void test_spsc_queue() {
    std::cout << "\n=== SPSC 링 버퍼 테스트 ===\n";

    constexpr int ITEMS = 100'000;
    SPSCQueue<int, 1024> q;

    long long sum_in  = 0;
    long long sum_out = 0;

    std::thread producer([&]() {
        for (int i = 0; i < ITEMS; ++i) {
            sum_in += i;
            while (!q.enqueue(i))  // 꽉 차면 대기
                std::this_thread::yield();
        }
    });

    std::thread consumer([&]() {
        int received = 0;
        while (received < ITEMS) {
            auto val = q.dequeue();
            if (val) {
                sum_out += *val;
                ++received;
            }
        }
    });

    producer.join();
    consumer.join();

    std::cout << "송신 합계: " << sum_in  << "\n";
    std::cout << "수신 합계: " << sum_out << "\n";
    std::cout << (sum_in == sum_out ? "✓ 합계 일치\n" : "✗ 합계 불일치!\n");
}

// ── 3. ABA 문제 시연 (데모용, 실제 버그는 발생시키지 않음) ──────────────────

void demo_aba_explanation() {
    std::cout << "\n=== ABA 문제 설명 ===\n";
    std::cout << "ABA 문제란:\n";
    std::cout << "  1. 스레드 A가 head = X를 읽음\n";
    std::cout << "  2. 스레드 B가 X를 pop하고 다른 노드 Y를 push한 뒤 X를 다시 push\n";
    std::cout << "  3. head는 다시 X가 됨\n";
    std::cout << "  4. 스레드 A의 CAS(X, new_head)가 성공 — 하지만 내부 구조 손상 가능\n";
    std::cout << "대응책: 버전 태그 포인터, Hazard Pointer, RCU, std::shared_ptr CAS\n";
}

int main() {
    test_treiber_stack();
    test_spsc_queue();
    demo_aba_explanation();
    return 0;
}
