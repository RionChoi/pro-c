# 세션 46: 고성능 메모리 아레나 & 풀

## 학습 목표
1. 선형 범프 포인터 할당자(Linear Arena)를 직접 구현한다
2. 마커 기반 LIFO 해제(StackArena)를 이해하고 구현한다
3. 고정 크기 오브젝트 풀(SlabAllocator)을 프리 리스트로 구현한다
4. `std::pmr::memory_resource` 인터페이스로 ArenaResource를 표준 PMR에 연동한다
5. 게임 루프의 퍼시스턴트/스크래치 이중 아레나 패턴을 이해한다

## 핵심 개념

| 개념 | 설명 |
|------|------|
| LinearArena | 범프 포인터 방식 — 할당 O(1), 일괄 해제 O(1) |
| StackArena | 마커(`save`/`restore`) 기반 LIFO 범위 해제 |
| SlabAllocator | 고정 크기 풀 — 프리 리스트, 할당/해제 O(1) |
| ArenaResource | `pmr::memory_resource` 어댑터 — PMR 컨테이너와 연동 |
| 정렬(Alignment) | `align_up()` 으로 모든 할당에 ABI 정렬 보장 |
| 스크래치 아레나 | 프레임 단위 임시 데이터용 — 매 프레임 reset() |

## 핵심 패턴

```cpp
// 1. LinearArena — 범프 포인터
constexpr std::size_t align_up(std::size_t n, std::size_t a) noexcept {
    return (n + a - 1) & ~(a - 1);
}
void* alloc(std::size_t size, std::size_t align) noexcept {
    std::size_t a = align_up(offset_, align);
    if (a + size > capacity_) return nullptr;
    void* p = base_ + a;
    offset_ = a + size;
    return p;
}

// 2. SlabAllocator — 프리 리스트 풀
union Slot { alignas(T) std::byte storage[sizeof(T)]; Slot* next; };
T* create() { Slot* s = free_head_; free_head_ = s->next; return new(s->storage) T(); }
void destroy(T* p) { p->~T(); reinterpret_cast<Slot*>(p)->next = free_head_; free_head_ = ...; }

// 3. PMR 통합
class ArenaResource : public std::pmr::memory_resource {
    void* do_allocate(std::size_t bytes, std::size_t align) override { ... }
    void  do_deallocate(void* p, std::size_t bytes, std::size_t align) override { ... }
};
std::pmr::vector<int> nums(&arena);  // ArenaResource를 할당자로 사용

// 4. 이중 아레나 패턴 (게임 루프)
LinearArena persistent_arena(...);  // 게임 오브젝트 — 게임 내내 유지
LinearArena scratch_arena(...);     // 프레임 임시 데이터 — 매 프레임 reset()
for (int frame = 0; frame < N; ++frame) {
    world.update(scratch_arena);
    scratch_arena.reset();  // O(1) 일괄 해제
}
```

## 컴파일 명령어

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic homework.cpp  -o hw
g++ -std=c++20 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
g++ -std=c++20 -Wall -Wextra -Wpedantic game1.cpp     -o game
```
