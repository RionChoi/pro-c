# C++ Session 34: 커스텀 할당자 & std::pmr

## 학습 목표
- C++ 표준 라이브러리 할당자 인터페이스 이해
- 커스텀 할당자 작성 및 구현
- std::pmr (Polymorphic Memory Resource) 개념 학습
- std::pmr::monotonic_buffer_resource 활용
- std::pmr::unsynchronized_pool_resource 활용
- 성능 최적화를 위한 메모리 관리

## 핵심 개념

### 1. 표준 할당자 인터페이스
- Allocator concept: `allocate()`, `deallocate()`, `allocate_at_least()` 등
- 상태 없는 vs 상태 있는 할당자
- 할당자의 타입 특성 (type traits)

### 2. std::pmr (C++17 이상)
- **polymorphic_allocator**: 런타임 타입의 메모리 리소스 사용
- **memory_resource**: 할당/해제 가상 함수 정의
- 내장 메모리 리소스:
  - `new_delete_resource`: 일반적인 new/delete 사용
  - `monotonic_buffer_resource`: 해제 없는 할당 (성능 극대화)
  - `unsynchronized_pool_resource`: 다양한 크기 풀 관리
  - `synchronized_pool_resource`: 스레드 안전 풀

### 3. 사용 사례
- 메모리 할당 성능 최적화
- 게임 엔진의 프레임별 메모리 풀
- 실시간 시스템에서의 예측 가능한 성능
- 다양한 메모리 전략 동적 선택

## 과제 설명

### homework.cpp
커스텀 할당자 작성 및 std::vector와 함께 사용:
- IntAllocator: int 전용 할당자
- std::vector<int, IntAllocator> 사용
- 할당/해제 횟수 추적

### homework2.cpp
std::pmr을 사용한 메모리 리소스 관리:
- monotonic_buffer_resource 사용
- 여러 std::vector를 같은 버퍼에서 할당
- 메모리 할당 횟수 비교

### game1.cpp
메모리 최적화 게임 — 동적 텍스처 로딩:
- 텍스처 메모리 풀 관리
- 레벨별 메모리 할당 전략
- 메모리 사용량 모니터링 게임

## 컴파일 방법

```bash
# C++17 이상 필요
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp -o hw34a
./hw34a

g++ -std=c++17 -Wall -Wextra -Wpedantic homework2.cpp -o hw34b
./hw34b

g++ -std=c++17 -Wall -Wextra -Wpedantic game1.cpp -o hw34c
./hw34c
```

## 참조
- C++ std::pmr documentation
- "Allocators Demystified" (ACCU conference talk)
- CppReference: std::pmr::polymorphic_allocator
