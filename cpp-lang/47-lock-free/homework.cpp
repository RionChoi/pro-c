// Session 47: Lock-Free 스택 & ABA 방지
// [1] Treiber Stack — 기본 lock-free 스택
// [2] ABA 문제 시뮬레이션
// [3] Tagged Pointer Stack — ABA 방지
#include <atomic>
#include <cstdint>
#include <cstddef>
#include <optional>
#include <iostream>
#include <string_view>
#include <cassert>
#include <new>

// ─────────────────────────────────────────────
// [1] Treiber Stack (단순 버전 — ABA 취약)
// ─────────────────────────────────────────────

template <typename T>
class TreiberStack {
    struct Node {
        T     value;
        Node* next{};
        explicit Node(T v) : value(std::move(v)) {}
    };

public:
    TreiberStack()  = default;
    ~TreiberStack() {
        Node* cur = head_.load(std::memory_order_relaxed);
        while (cur) { Node* nxt = cur->next; delete cur; cur = nxt; }
    }

    void push(T val) {
        Node* node = new Node(std::move(val));
        node->next = head_.load(std::memory_order_relaxed);
        while (!head_.compare_exchange_weak(node->next, node,
                                            std::memory_order_release,
                                            std::memory_order_relaxed)) {}
    }

    std::optional<T> pop() {
        Node* old = head_.load(std::memory_order_acquire);
        while (old) {
            if (head_.compare_exchange_weak(old, old->next,
                                            std::memory_order_acquire,
                                            std::memory_order_relaxed)) {
                T val = std::move(old->value);
                delete old;
                return val;
            }
        }
        return std::nullopt;
    }

    bool empty() const noexcept {
        return head_.load(std::memory_order_relaxed) == nullptr;
    }

    std::size_t size() const noexcept {
        std::size_t n = 0;
        Node* cur = head_.load(std::memory_order_relaxed);
        while (cur) { ++n; cur = cur->next; }
        return n;
    }

private:
    std::atomic<Node*> head_{nullptr};
};

static_assert(std::atomic<int*>::is_always_lock_free,
              "포인터 atomic은 lock-free여야 함");

// ─────────────────────────────────────────────
// [2] ABA 문제 설명 (시뮬레이션)
// ─────────────────────────────────────────────

// ABA 시나리오 (단일 스레드 재현):
// T1: head=A → old=A, next=B 읽음 → 선점당함
// T2: pop A, pop B, push A' (새 노드, 같은 주소 가능 — 재사용)
// T1: CAS(A, B) 성공 → head=B가 됨 (이미 해제된 B를 가리킴)
void explain_aba() {
    std::cout << "\n=== [2] ABA 문제 시뮬레이션 ===\n";
    std::cout << "시나리오: 단순 포인터 CAS 스택에서 발생\n";
    std::cout << "  1. 스레드1이 head=A, next=B를 읽고 선점당함\n";
    std::cout << "  2. 스레드2가 A를 pop, B를 pop, 동일 주소에 A'를 push\n";
    std::cout << "  3. 스레드1이 CAS(head==A, →B) 성공\n";
    std::cout << "  4. head=B(이미 해제됨) → Dangling Pointer!\n";
    std::cout << "해결책: 포인터에 버전 태그 결합 (Tagged Pointer)\n";
}

// ─────────────────────────────────────────────
// [3] Tagged Pointer Stack — ABA 방지
// ─────────────────────────────────────────────
// x86-64 canonical 주소는 47비트만 사용.
// uintptr_t 상위 16비트에 버전 태그를 패킹 → 단일 64비트 CAS로 ABA 방지
//   [63:48] tag(16비트)  |  [47:0] pointer(48비트)

template <typename T>
class TaggedStack {
    struct Node {
        T     value;
        Node* next{};
        explicit Node(T v) : value(std::move(v)) {}
    };

    // 태그(상위 16비트) + 포인터(하위 48비트) 패킹
    static constexpr uintptr_t PTR_MASK = (uintptr_t(1) << 48) - 1;
    static constexpr uintptr_t TAG_SHIFT = 48;

    static uintptr_t pack(Node* p, uint16_t tag) noexcept {
        return (uintptr_t(tag) << TAG_SHIFT) | (reinterpret_cast<uintptr_t>(p) & PTR_MASK);
    }
    static Node*    unpack_ptr(uintptr_t v) noexcept {
        // 부호 확장 — x86-64 canonical 주소 복원 (bit47 sign-extend)
        uintptr_t raw = v & PTR_MASK;
        // sign extend from bit 47
        if (raw & (uintptr_t(1) << 47)) raw |= ~PTR_MASK;
        return reinterpret_cast<Node*>(raw);
    }
    static uint16_t unpack_tag(uintptr_t v) noexcept {
        return uint16_t(v >> TAG_SHIFT);
    }

public:
    TaggedStack()  = default;
    ~TaggedStack() {
        Node* cur = unpack_ptr(head_.load(std::memory_order_relaxed));
        while (cur) { Node* nxt = cur->next; delete cur; cur = nxt; }
    }

    void push(T val) {
        Node* node = new Node(std::move(val));
        uintptr_t old = head_.load(std::memory_order_relaxed);
        uintptr_t nw;
        do {
            node->next = unpack_ptr(old);
            nw         = pack(node, uint16_t(unpack_tag(old) + 1u));
        } while (!head_.compare_exchange_weak(old, nw,
                                              std::memory_order_release,
                                              std::memory_order_relaxed));
    }

    std::optional<T> pop() {
        uintptr_t old = head_.load(std::memory_order_acquire);
        while (true) {
            Node* p = unpack_ptr(old);
            if (!p) return std::nullopt;
            uintptr_t nw = pack(p->next, uint16_t(unpack_tag(old) + 1u));
            if (head_.compare_exchange_weak(old, nw,
                                            std::memory_order_acquire,
                                            std::memory_order_relaxed)) {
                T val = std::move(p->value);
                delete p;
                return val;
            }
        }
    }

    bool empty() const noexcept {
        return unpack_ptr(head_.load(std::memory_order_relaxed)) == nullptr;
    }

    std::size_t size() const noexcept {
        std::size_t n = 0;
        Node* cur = unpack_ptr(head_.load(std::memory_order_relaxed));
        while (cur) { ++n; cur = cur->next; }
        return n;
    }

private:
    std::atomic<uintptr_t> head_{0};
};

static_assert(std::atomic<uintptr_t>::is_always_lock_free, "uintptr_t atomic은 lock-free여야 함");

// ─────────────────────────────────────────────
// 테스트 헬퍼
// ─────────────────────────────────────────────

template <typename Stack>
void test_stack(std::string_view name) {
    Stack s;
    std::cout << "\n--- " << name << " ---\n";

    // push 5개
    for (int i = 1; i <= 5; ++i) s.push(i * 10);
    std::cout << "push 10~50 후 size=" << s.size() << "\n";

    // pop 모두
    std::cout << "pop 순서: ";
    while (!s.empty()) {
        auto v = s.pop();
        if (v) std::cout << *v << " ";
    }
    std::cout << "\n";
    std::cout << "비었음: " << (s.empty() ? "✅" : "❌") << "\n";

    // 빈 스택 pop
    auto empty_pop = s.pop();
    std::cout << "빈 스택 pop: " << (empty_pop ? "값있음" : "nullopt ✅") << "\n";

    // 재사용
    s.push(99);
    s.push(100);
    std::cout << "재사용 후 size=" << s.size() << " top=" << *s.pop() << "\n";
}

// ─────────────────────────────────────────────
// main
// ─────────────────────────────────────────────

int main() {
    std::cout << "=== [1] Treiber Stack (기본 lock-free) ===";
    test_stack<TreiberStack<int>>("TreiberStack<int>");

    explain_aba();

    std::cout << "\n=== [3] Tagged Pointer Stack (ABA 방지) ===";
    test_stack<TaggedStack<int>>("TaggedStack<int>");

    std::cout << "\n=== atomic lock-free 여부 ===\n";
    std::atomic<int>      ai{};
    std::atomic<long>     al{};
    std::atomic<void*>    ap{};
    std::cout << "  atomic<int>:   " << (ai.is_lock_free() ? "lock-free ✅" : "not lock-free") << "\n";
    std::cout << "  atomic<long>:  " << (al.is_lock_free() ? "lock-free ✅" : "not lock-free") << "\n";
    std::cout << "  atomic<void*>: " << (ap.is_lock_free() ? "lock-free ✅" : "not lock-free") << "\n";

    // TaggedStack은 uintptr_t 단일 CAS → 항상 lock-free
    std::atomic<uintptr_t> aup{};
    std::cout << "  atomic<uintptr_t>: "
              << (aup.is_lock_free() ? "lock-free ✅" : "not lock-free") << "\n";

    std::cout << "\n=== 세션 47 Part 1 완료 ===\n";
    return 0;
}
