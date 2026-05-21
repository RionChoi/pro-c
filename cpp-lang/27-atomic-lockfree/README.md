# 세션 27: 원자 연산 & Lock-free 프로그래밍

## 학습 목표

- `std::atomic<T>`을 사용하여 데이터 경쟁 없이 공유 변수를 안전하게 수정한다.
- 메모리 순서(memory_order)의 종류와 각각의 의미를 이해한다.
- `compare_exchange_weak` / `compare_exchange_strong`으로 lock-free 자료구조를 구현한다.
- ABA 문제가 무엇인지 이해하고 대응 방법을 안다.

---

## 핵심 개념

### std::atomic

```cpp
#include <atomic>

std::atomic<int> counter{0};
counter.fetch_add(1, std::memory_order_relaxed);  // 원자적 증가
int val = counter.load(std::memory_order_acquire); // 원자적 읽기
counter.store(42, std::memory_order_release);       // 원자적 쓰기
```

### memory_order

| 순서 | 의미 |
|------|------|
| `memory_order_relaxed` | 순서 보장 없음 — 가장 빠름 |
| `memory_order_acquire` | 이후 읽기/쓰기가 앞으로 재배열되지 않음 |
| `memory_order_release` | 이전 읽기/쓰기가 뒤로 재배열되지 않음 |
| `memory_order_acq_rel` | acquire + release (read-modify-write에 사용) |
| `memory_order_seq_cst` | 전체 순서 보장 — 가장 안전하지만 느림 (기본값) |

### compare_exchange (CAS — Compare-And-Swap)

```cpp
// expected 값과 현재 값이 같으면 desired로 교체하고 true 반환
// 다르면 expected에 현재 값을 저장하고 false 반환
T expected = old_val;
if (atomic_var.compare_exchange_weak(expected, new_val)) {
    // 교체 성공
} else {
    // 실패 — expected에는 현재 값이 들어 있음
}
```

### Lock-free 스택 (Treiber Stack)

```cpp
template<typename T>
class LockFreeStack {
    struct Node { T data; Node* next; };
    std::atomic<Node*> head{nullptr};
public:
    void push(T val) {
        Node* node = new Node{val, head.load()};
        while (!head.compare_exchange_weak(node->next, node));
    }
    bool pop(T& out) {
        Node* old = head.load();
        while (old && !head.compare_exchange_weak(old, old->next));
        if (!old) return false;
        out = old->data;
        delete old;
        return true;
    }
};
```

### ABA 문제

CAS 기반 lock-free 코드에서 값이 A→B→A로 변경되면 CAS는 변경을 감지하지 못한다.
해결책: 태그 포인터(버전 카운터), hazard pointer, RCU 등.

---

## 과제 구성

| 파일 | 내용 |
|------|------|
| `homework.cpp` | 원자 연산으로 안전한 공유 카운터 및 은행 계좌 구현 |
| `homework2.cpp` | Lock-free 스택 (Treiber Stack) 구현 및 테스트 |
| `game1.cpp` | 멀티스레드 반응속도 게임 — 원자 플래그로 동시 시작 제어 |

---

## 참고

- cppreference: `std::atomic`, `std::memory_order`
- Herb Sutter — "atomic Weapons" (CppCon)
- Anthony Williams — "C++ Concurrency in Action" (chapter 7)
