# 세션 34: 커스텀 할당자 & std::pmr

## 학습 목표

- C++ 할당자(Allocator) Named Requirements를 이해하고 직접 구현한다.
- `std::pmr::memory_resource` 추상 인터페이스를 이해하고 커스텀 구현을 작성한다.
- 내장 PMR 메모리 리소스(`monotonic_buffer_resource`, `unsynchronized_pool_resource`)를 활용한다.
- 메모리 리소스 체이닝(upstream 패턴)을 이해한다.
- 아레나 기반 메모리 관리로 성능 최적화 원리를 이해한다.

---

## 핵심 개념

### 1. C++ 할당자 Named Requirements

STL 컨테이너는 두 번째 템플릿 매개변수로 할당자를 받는다.
최소 구현 요건:

```cpp
template <typename T>
struct MyAllocator {
    using value_type = T;

    MyAllocator() noexcept = default;

    // rebind 지원: 다른 타입으로 변환 생성
    template <typename U>
    MyAllocator(const MyAllocator<U>&) noexcept {}

    T* allocate(std::size_t n);                      // n 개 객체용 메모리 확보
    void deallocate(T* p, std::size_t n) noexcept;  // 해제
};

template <typename T, typename U>
bool operator==(const MyAllocator<T>&, const MyAllocator<U>&) noexcept;
```

컨테이너 내부에서는 `rebind`를 통해 노드 타입용 할당자로 변환된다.
예: `list<int, MyAllocator<int>>` → 내부적으로 `MyAllocator<list_node<int>>` 사용

### 2. std::pmr — 폴리모픽 메모리 리소스 (C++17)

```cpp
#include <memory_resource>

class memory_resource {
protected:
    virtual void* do_allocate(size_t bytes, size_t align) = 0;
    virtual void  do_deallocate(void* p, size_t bytes, size_t align) = 0;
    virtual bool  do_is_equal(const memory_resource&) const noexcept = 0;
};
```

`std::pmr::polymorphic_allocator<T>` 는 런타임에 `memory_resource*` 를 통해
동적으로 할당 전략을 교체할 수 있는 할당자이다.

### 3. 내장 메모리 리소스 비교

| 리소스 | 할당 속도 | 해제 방식 | 용도 |
|--------|-----------|-----------|------|
| `monotonic_buffer_resource` | O(1) 매우 빠름 | 일괄 해제만 가능 | 임시 아레나, 레벨 단위 메모리 |
| `unsynchronized_pool_resource` | 빠름 | 개별 해제 후 재사용 | 단일 스레드 반복 할당/해제 |
| `synchronized_pool_resource` | 보통 | 개별 해제 후 재사용 | 멀티스레드 풀 |
| `null_memory_resource()` | — | — | 업스트림 차단, 디버깅 |

### 4. PMR 컨테이너

`std::pmr::vector<T>` = `std::vector<T, std::pmr::polymorphic_allocator<T>>`

```cpp
std::array<std::byte, 1024> buf{};
std::pmr::monotonic_buffer_resource arena(buf.data(), buf.size(),
                                          std::pmr::null_memory_resource());
std::pmr::vector<int> v(&arena);  // 힙 할당 없이 스택 버퍼만 사용!
```

### 5. 업스트림 체이닝

메모리 리소스는 용량 초과 시 업스트림 리소스에서 추가 메모리를 요청한다.

```
[컨테이너]
   ↓ allocate()
[LoggingResource]   ← 커스텀 래퍼 (로깅, 통계 등)
   ↓ upstream
[monotonic_buffer_resource]  ← 빠른 아레나
   ↓ upstream (버퍼 초과 시)
[null_memory_resource]  ← 예외 발생 (초과 방지)
```

### 6. 커스텀 memory_resource 구현

```cpp
class MyResource : public std::pmr::memory_resource {
private:
    void* do_allocate(size_t bytes, size_t align) override { ... }
    void  do_deallocate(void* p, size_t bytes, size_t align) override { ... }
    bool  do_is_equal(const memory_resource& o) const noexcept override {
        return this == &o;
    }
};
```

---

## 과제 설명

### homework.cpp — 커스텀 할당자 (TrackingAllocator)
- 할당자 Named Requirements를 만족하는 `TrackingAllocator<T>` 구현
- 할당/해제 횟수와 최대 사용 바이트를 전역 통계 구조체로 추적
- `std::vector<int>` 와 `std::list<int>` 에 적용하여 내부 할당 패턴 관찰

### homework2.cpp — std::pmr 실습
- `monotonic_buffer_resource` 로 힙 없는 스택 아레나 구성
- `LoggingResource` (커스텀 `memory_resource`) 구현 및 통계 출력
- 업스트림 체이닝: `LoggingResource → monotonic_buffer_resource`
- `unsynchronized_pool_resource` 로 풀 재사용 확인

### game1.cpp — 메모리 던전 (아레나 기반 탐험 게임)
- 각 던전 층이 `monotonic_buffer_resource` 아레나를 독립적으로 소유
- PMR 컨테이너(`pmr::vector<Item>`, `pmr::string`)로 아이템·몬스터 관리
- 층 탈출 시 `DungeonFloor` 소멸 → 아레나 전체 해제 (O(1)) 시연
- 플레이어가 아이템을 수거하며 골드를 쌓는 인터랙티브 게임
