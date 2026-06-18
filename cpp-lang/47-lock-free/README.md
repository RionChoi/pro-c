# 세션 47: Lock-Free 자료구조 심화

## 학습 목표
1. Treiber Stack — `compare_exchange_weak`로 lock-free push/pop 구현
2. ABA 문제의 원인과 단순 포인터 CAS의 취약점을 이해한다
3. Tagged Pointer — `uintptr_t` 상위 16비트에 버전 태그 패킹으로 ABA 방지
4. Michael-Scott Queue — sentinel 노드 기반 lock-free MPMC 큐 구현
5. 해저드 포인터 — scan-and-reclaim으로 lock-free 안전 메모리 회수
6. MPSC Queue — 단방향 링크드 리스트 기반 Multi-Producer Single-Consumer 큐

## 핵심 개념

| 개념 | 설명 |
|------|------|
| `compare_exchange_weak` | CAS — 실패 시 재시도, 루프에서 사용 |
| ABA 문제 | A→B→A 순서로 변경 시 CAS가 변화를 감지 못함 |
| Tagged Pointer | 포인터 상위 16비트에 버전 카운터 패킹 → ABA 방지 |
| Treiber Stack | 단방향 연결 리스트 + head CAS — lock-free 스택 |
| Michael-Scott Queue | sentinel + head/tail CAS — lock-free FIFO |
| 해저드 포인터 | 스레드가 접근 중인 포인터를 전역 테이블에 등록 → 안전 delete |
| MPSC Queue | head에 원자적 swap, tail은 단일 consumer만 접근 |

## 핵심 패턴

```cpp
// 1. Treiber Stack — CAS 루프
void push(T val) {
    Node* node = new Node(val);
    node->next = head_.load(relaxed);
    while (!head_.compare_exchange_weak(node->next, node, release, relaxed)) {}
}

// 2. Tagged Pointer — uintptr_t 패킹 (ABA 방지)
static uintptr_t pack(Node* p, uint16_t tag) noexcept {
    return (uintptr_t(tag) << 48) | (reinterpret_cast<uintptr_t>(p) & PTR_MASK);
}
// CAS 시 태그가 다르면 A=A'여도 실패 → ABA 방지

// 3. 해저드 포인터 보호
HazardGuard<Node> guard;
Node* old = guard.protect(head_);  // HP 등록
if (head_.compare_exchange_weak(...)) {
    guard.clear();
    g_retire_list.retire(old);      // 즉시 delete 대신 retire
    g_retire_list.scan_and_reclaim(); // HP와 대조 후 안전하면 delete
}
```

## 컴파일 명령어

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic homework.cpp  -o hw
g++ -std=c++20 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
g++ -std=c++20 -Wall -Wextra -Wpedantic game1.cpp     -o game
```
