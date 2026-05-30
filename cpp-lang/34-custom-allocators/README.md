# Session 34: 커스텀 할당자 & std::pmr

## 학습 목표
- C++의 메모리 할당자(allocator) 개념 이해
- 커스텀 할당자 설계 및 구현
- std::pmr (Polymorphic Memory Resources) 사용
- 메모리 풀과 성능 최적화

## 핵심 개념

### 1. 표준 할당자 (std::allocator)
```cpp
std::vector<int> vec;  // std::allocator<int> 사용
```

### 2. 커스텀 할당자 설계
- `allocate()` 함수 구현
- `deallocate()` 함수 구현
- 템플릿 메서드 패턴

### 3. std::pmr 메모리 리소스
- `std::pmr::memory_resource` 기본 클래스
- `std::pmr::monotonic_buffer_resource` (한 방향 할당)
- `std::pmr::unsynchronized_pool_resource` (풀 기반)
- `std::pmr::vector`, `std::pmr::string` 등 컨테이너

## 할당자 인터페이스
```cpp
template <typename T>
class MyAllocator {
public:
    using value_type = T;
    
    MyAllocator() = default;
    
    T* allocate(size_t n);
    void deallocate(T* p, size_t n);
};
```

## std::pmr 사용 패턴
```cpp
char buffer[1024];
std::pmr::monotonic_buffer_resource res(buffer, sizeof(buffer));
std::pmr::vector<int> vec(&res);
vec.push_back(42);
```

## 파일 구성

| 파일 | 설명 |
|------|------|
| `homework.cpp` | 커스텀 할당자 구현 (counting, tracking) |
| `homework2.cpp` | std::pmr 메모리 풀 사용 |
| `game1.cpp` | 할당자 성능 비교 게임 |

---

## 컴파일 및 실행

```bash
# Session 34 컴파일
g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/34-custom-allocators/homework.cpp -o hw34
./hw34

g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/34-custom-allocators/homework2.cpp -o hw34_2
./hw34_2

g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/34-custom-allocators/game1.cpp -o game34
./game34
```

## 핵심 배울 점
1. 메모리 관리의 세분화 제어
2. 성능 최적화를 위한 할당 전략 선택
3. std::pmr를 통한 유연한 메모리 관리
4. 임베디드 환경에서의 고정 메모리 풀 활용
