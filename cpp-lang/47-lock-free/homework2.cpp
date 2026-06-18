// Session 47: Michael-Scott Lock-Free Queue + 해저드 포인터
// [1] MS Queue — lock-free MPMC 큐
// [2] 해저드 포인터 — dangling pointer 없는 안전한 메모리 회수
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <array>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string_view>
#include <cassert>
#include <new>

// ─────────────────────────────────────────────
// [1] Michael-Scott Lock-Free Queue
// ─────────────────────────────────────────────

template <typename T>
class MSQueue {
    struct Node {
        std::atomic<Node*> next{nullptr};
        std::optional<T>   value{};  // sentinel는 nullopt
        explicit Node() = default;
        explicit Node(T v) : value(std::move(v)) {}
    };

public:
    MSQueue() {
        // sentinel 노드
        Node* sentinel = new Node();
        head_.store(sentinel, std::memory_order_relaxed);
        tail_.store(sentinel, std::memory_order_relaxed);
    }

    ~MSQueue() {
        Node* cur = head_.load(std::memory_order_relaxed);
        while (cur) {
            Node* nxt = cur->next.load(std::memory_order_relaxed);
            delete cur;
            cur = nxt;
        }
    }

    void enqueue(T val) {
        Node* node = new Node(std::move(val));
        while (true) {
            Node* tail = tail_.load(std::memory_order_acquire);
            Node* next = tail->next.load(std::memory_order_acquire);
            if (tail == tail_.load(std::memory_order_relaxed)) {
                if (next == nullptr) {
                    // tail이 마지막 노드 — CAS로 연결
                    Node* expected = nullptr;
                    if (tail->next.compare_exchange_weak(expected, node,
                                                         std::memory_order_release,
                                                         std::memory_order_relaxed)) {
                        // tail 업데이트 (실패해도 무방 — 다음 enqueue가 처리)
                        tail_.compare_exchange_weak(tail, node,
                                                    std::memory_order_release,
                                                    std::memory_order_relaxed);
                        return;
                    }
                } else {
                    // tail이 뒤처짐 — 앞으로 당김
                    tail_.compare_exchange_weak(tail, next,
                                                std::memory_order_release,
                                                std::memory_order_relaxed);
                }
            }
        }
    }

    std::optional<T> dequeue() {
        while (true) {
            Node* head = head_.load(std::memory_order_acquire);
            Node* tail = tail_.load(std::memory_order_acquire);
            Node* next = head->next.load(std::memory_order_acquire);

            if (head == head_.load(std::memory_order_relaxed)) {
                if (head == tail) {
                    if (next == nullptr) return std::nullopt;  // 비어 있음
                    // tail 뒤처짐
                    tail_.compare_exchange_weak(tail, next,
                                                std::memory_order_release,
                                                std::memory_order_relaxed);
                } else {
                    if (next == nullptr) continue;
                    T val = *next->value;
                    if (head_.compare_exchange_weak(head, next,
                                                    std::memory_order_release,
                                                    std::memory_order_relaxed)) {
                        delete head;  // 구 sentinel 해제
                        return val;
                    }
                }
            }
        }
    }

    bool empty() const noexcept {
        Node* head = head_.load(std::memory_order_acquire);
        Node* next = head->next.load(std::memory_order_acquire);
        return next == nullptr;
    }

    std::size_t size() const noexcept {
        std::size_t n = 0;
        Node* cur = head_.load(std::memory_order_relaxed)->next.load(std::memory_order_relaxed);
        while (cur) { ++n; cur = cur->next.load(std::memory_order_relaxed); }
        return n;
    }

private:
    alignas(64) std::atomic<Node*> head_{};
    alignas(64) std::atomic<Node*> tail_{};
};

// ─────────────────────────────────────────────
// [2] 해저드 포인터 구현
// ─────────────────────────────────────────────
// 목적: lock-free 자료구조에서 dequeue 후 다른 스레드가
//       같은 포인터를 읽는 중일 때 delete를 막음

static constexpr int HP_MAX_THREADS  = 8;
static constexpr int HP_PER_THREAD   = 2;
static constexpr int HP_RETIRE_LIMIT = 16;

struct HazardSlot {
    std::atomic<void*> ptr{nullptr};
    std::atomic<bool>  in_use{false};
};

// 전역 해저드 포인터 테이블
static std::array<HazardSlot, HP_MAX_THREADS * HP_PER_THREAD> g_hp_table{};

// 스레드별 retire list (단순화: 벡터)
struct RetireList {
    struct Entry { void* ptr; void(*deleter)(void*); };
    std::vector<Entry> items;

    template <typename T>
    void retire(T* p) {
        items.push_back({p, [](void* x){ delete static_cast<T*>(x); }});
    }

    void scan_and_reclaim() {
        // 현재 활성 해저드 포인터 수집
        std::vector<void*> hps;
        for (auto& slot : g_hp_table) {
            void* p = slot.ptr.load(std::memory_order_acquire);
            if (p) hps.push_back(p);
        }
        std::sort(hps.begin(), hps.end());

        // retire list에서 보호되지 않는 것만 delete
        std::vector<Entry> deferred;
        for (auto& e : items) {
            bool protected_ = std::binary_search(hps.begin(), hps.end(), e.ptr);
            if (!protected_) {
                e.deleter(e.ptr);
            } else {
                deferred.push_back(e);
            }
        }
        items = std::move(deferred);
    }
};

static thread_local RetireList g_retire_list;

// 해저드 슬롯 획득
static HazardSlot* acquire_hp_slot() {
    for (auto& slot : g_hp_table) {
        bool expected = false;
        if (slot.in_use.compare_exchange_strong(expected, true,
                                                std::memory_order_acquire,
                                                std::memory_order_relaxed)) {
            return &slot;
        }
    }
    return nullptr;  // 슬롯 부족
}

static void release_hp_slot(HazardSlot* slot) {
    if (!slot) return;
    slot->ptr.store(nullptr, std::memory_order_release);
    slot->in_use.store(false, std::memory_order_release);
}

// RAII 해저드 포인터 가드
template <typename T>
struct HazardGuard {
    HazardSlot* slot;

    explicit HazardGuard() : slot(acquire_hp_slot()) {}
    ~HazardGuard() { release_hp_slot(slot); }

    HazardGuard(const HazardGuard&)            = delete;
    HazardGuard& operator=(const HazardGuard&) = delete;

    T* protect(std::atomic<T*>& src) {
        T* p;
        do {
            p = src.load(std::memory_order_acquire);
            if (slot) slot->ptr.store(p, std::memory_order_release);
        } while (p != src.load(std::memory_order_acquire));
        return p;
    }

    void clear() {
        if (slot) slot->ptr.store(nullptr, std::memory_order_release);
    }
};

// HP 통합 lock-free 스택 (dequeue 후 HP로 안전 회수)
template <typename T>
class HPStack {
    struct Node {
        T     value;
        Node* next{};
        explicit Node(T v) : value(std::move(v)) {}
    };

public:
    ~HPStack() {
        Node* cur = head_.load(std::memory_order_relaxed);
        while (cur) { Node* n = cur->next; delete cur; cur = n; }
    }

    void push(T val) {
        Node* node = new Node(std::move(val));
        node->next = head_.load(std::memory_order_relaxed);
        while (!head_.compare_exchange_weak(node->next, node,
                                            std::memory_order_release,
                                            std::memory_order_relaxed)) {}
    }

    std::optional<T> pop() {
        HazardGuard<Node> guard;
        while (true) {
            Node* old = guard.protect(head_);
            if (!old) return std::nullopt;
            if (head_.compare_exchange_weak(old, old->next,
                                            std::memory_order_acquire,
                                            std::memory_order_relaxed)) {
                guard.clear();
                T val = std::move(old->value);
                g_retire_list.retire(old);
                if (g_retire_list.items.size() >= HP_RETIRE_LIMIT)
                    g_retire_list.scan_and_reclaim();
                return val;
            }
        }
    }

    bool empty() const noexcept {
        return head_.load(std::memory_order_relaxed) == nullptr;
    }

    void flush_retired() { g_retire_list.scan_and_reclaim(); }
    std::size_t pending_reclaim() const { return g_retire_list.items.size(); }

private:
    std::atomic<Node*> head_{nullptr};
};

// ─────────────────────────────────────────────
// main
// ─────────────────────────────────────────────

int main() {
    std::cout << "=== [1] Michael-Scott Lock-Free Queue ===\n";
    {
        MSQueue<int> q;
        std::cout << "빈 큐 dequeue: " << (q.dequeue() ? "값" : "nullopt ✅") << "\n";

        for (int i = 1; i <= 6; ++i) q.enqueue(i * 10);
        std::cout << "enqueue 10~60 후 size=" << q.size() << "\n";

        std::cout << "dequeue 순서: ";
        while (!q.empty()) {
            auto v = q.dequeue();
            if (v) std::cout << *v << " ";
        }
        std::cout << "\n";

        std::cout << "비었음: " << (q.empty() ? "✅" : "❌") << "\n";

        // 인터리브 시뮬레이션
        q.enqueue(100); q.enqueue(200);
        auto v1 = q.dequeue();
        q.enqueue(300);
        auto v2 = q.dequeue();
        auto v3 = q.dequeue();
        std::cout << "인터리브 결과: " << *v1 << " " << *v2 << " " << *v3
                  << " (예상: 100 200 300)\n";
    }

    std::cout << "\n=== [2] 해저드 포인터 통합 스택 ===\n";
    {
        HPStack<int> s;

        for (int i = 1; i <= 8; ++i) s.push(i * 7);
        std::cout << "push 7~56 완료\n";

        std::cout << "pop 순서: ";
        while (!s.empty()) {
            auto v = s.pop();
            if (v) std::cout << *v << " ";
        }
        std::cout << "\n";

        s.flush_retired();
        std::cout << "flush 후 pending_reclaim=" << s.pending_reclaim()
                  << " (예상: 0) " << (s.pending_reclaim() == 0 ? "✅" : "❌") << "\n";
    }

    std::cout << "\n=== [3] 해저드 포인터 보호 동작 확인 ===\n";
    {
        HPStack<std::string> s;
        s.push("apple");
        s.push("banana");
        s.push("cherry");

        auto v = s.pop();
        std::cout << "pop: " << (v ? *v : "nullopt") << "\n";
        std::cout << "pending (HP 보호 시 지연): " << s.pending_reclaim() << "\n";

        s.flush_retired();
        std::cout << "flush 후 pending: " << s.pending_reclaim() << " ✅\n";
    }

    std::cout << "\n=== 세션 47 Part 2 완료 ===\n";
    return 0;
}
