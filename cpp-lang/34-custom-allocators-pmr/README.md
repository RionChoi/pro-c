# Session 34: 커스텀 할당자 & std::pmr (Custom Allocators & PMR)

## 학습 목표
- C++ 메모리 할당자(allocator)의 개념과 인터페이스 이해
- 커스텀 할당자 구현 및 STL 컨테이너에 적용
- std::pmr (polymorphic memory resource) 활용
- 메모리 풀(memory pool) 할당자 구현
- 성능 최적화 기법

## 핵심 개념

### 할당자(Allocator)
- STL 컨테이너의 메모리 관리 전략을 추상화
- `allocate()`, `deallocate()` 메서드 필수
- C++17부터 단순화된 요구사항 (rebind 제거)

### std::pmr (Polymorphic Memory Resource)
- 런타임 다형성을 통한 메모리 할당 전략 변경
- `std::pmr::memory_resource` 기본 클래스
- `std::pmr::monotonic_buffer_resource`, `std::pmr::unsynchronized_pool_resource` 등 제공
- std::pmr::vector, std::pmr::string 등 편의 타입 제공

### 메모리 풀 할당자
- 고정 크기 블록 재사용으로 성능 개선
- 외부 단편화 감소
- 캐시 지역성 향상

## 과제 사양

### homework.cpp
고정 크기 메모리 풀 할당자(FixedPoolAllocator) 구현
- 사전할당된 메모리 풀에서 할당
- 재사용 가능한 빈 블록 추적
- std::vector, std::list와 호환

### homework2.cpp
std::pmr을 활용한 메모리 할당 전략
- std::pmr::monotonic_buffer_resource 사용
- std::pmr::vector, std::pmr::string 활용
- 메모리 사용량 측정 및 비교

### game1.cpp
객체 풀 게임 — 동적 파티클 시뮬레이션
- 할당자를 사용한 파티클 시스템
- 메모리 재사용을 통한 효율적 관리
- 성능 메트릭 출력 (할당 횟수, 메모리 사용)

## 참고
- C++ Allocators specification
- std::pmr documentation
- Memory pool allocation patterns
