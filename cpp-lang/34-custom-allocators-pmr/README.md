# Session 34: 커스텀 할당자 & std::pmr

## 학습 목표
- 메모리 할당자(allocator) 인터페이스 이해
- std::allocator 커스터마이징
- std::pmr(Polymorphic Memory Resources) 활용
- 메모리 풀(memory pool) 구현
- allocator traits와 REBIND 패턴

## 핵심 개념

### 1. Allocator 인터페이스
```cpp
template <typename T>
class MyAllocator {
public:
    using value_type = T;
    
    T* allocate(std::size_t n);
    void deallocate(T* p, std::size_t n);
    
    template <typename U>
    struct rebind {
        using other = MyAllocator<U>;
    };
};
```

### 2. std::pmr::memory_resource
- 가상 함수로 동적 메모리 관리 정책 변경
- `allocate()`, `deallocate()`, `is_equal()` 구현
- 런타임 다형성으로 할당 전략 교체

### 3. std::pmr::monotonic_buffer_resource
- 할당만 하고 개별 해제 없음
- 모든 메모리를 한 번에 해제
- 성능 최적화에 유용

### 4. std::pmr::unsynchronized_pool_resource
- 고정 크기 메모리 풀
- 동기화 없음 (단일 스레드 최적화)

## 참고 자료
- cppreference: Custom allocators
- C++20: std::pmr namespace
