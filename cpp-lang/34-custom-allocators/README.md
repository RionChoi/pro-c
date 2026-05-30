# Session 34: 커스텀 할당자 & std::pmr

## 학습 목표
- C++ 메모리 할당자(allocator) 개념 이해
- std::allocator 인터페이스와 커스텀 할당자 구현
- std::pmr (Polymorphic Memory Resource) 사용법
- 메모리 풀을 활용한 성능 최적화
- 할당자를 템플릿 컨테이너와 함께 사용

## 핵심 개념

### 1. 표준 할당자 (std::allocator)
```cpp
std::vector<int> v;  // std::allocator<int>를 기본값으로 사용
```

### 2. 커스텀 할당자 요구사항
- `allocate()` — 메모리 할당
- `deallocate()` — 메모리 해제
- `construct()` / `destroy()` (C++20 이전)
- `rebind` — 다른 타입의 할당자로 변환

### 3. std::pmr (C++17)
- 런타임 다형성으로 할당자 선택
- `std::pmr::memory_resource` 상속으로 구현
- `std::pmr::vector`, `std::pmr::string` 등 제공

## 과제

### homework.cpp — 커스텀 할당자
- CountingAllocator: 할당/해제 횟수 추적
- 벡터에 커스텀 할당자 적용
- 메모리 사용량 통계 출력

### homework2.cpp — std::pmr 메모리 풀
- std::pmr::monotonic_buffer_resource (증가만 하고 감소 안 함)
- std::pmr::synchronized_pool_resource (스레드 안전)
- std::pmr::vector/std::pmr::string과 함께 사용

### game1.cpp — 동적 메모리 풀 게임
- 제한된 메모리 풀 내에서 객체 생성/삭제
- 할당 실패 시나리오 처리
- 메모리 효율 점수 계산

## 컴파일 및 실행
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/34-custom-allocators/homework.cpp -o hw34
./hw34

g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/34-custom-allocators/homework2.cpp -o hw34_2
./hw34_2

g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/34-custom-allocators/game1.cpp -o game34
./game34
```

## 학습 포인트
- ✓ 할당자가 STL 컨테이너와 어떻게 상호작용하는지
- ✓ std::pmr의 다형적 메모리 관리
- ✓ 메모리 풀로 성능 최적화
- ✓ 할당 실패 처리 전략
