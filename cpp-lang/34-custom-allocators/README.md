# Session 34: 커스텀 할당자 & std::pmr (Custom Allocators & std::pmr)

## 학습 목표
- STL allocator 인터페이스 이해
- 커스텀 할당자 설계 및 구현
- std::pmr (Polymorphic Memory Resource) 활용
- 메모리 풀 기반 할당 최적화
- 성능 프로파일링 및 메모리 관리

## 핵심 개념

### 1. STL Allocator 인터페이스
- `value_type`, `pointer`, `reference` 등 필요 타입 정의
- `allocate()` / `deallocate()` 구현
- 할당자 상태 관리 (stateless vs stateful)

### 2. Polymorphic Memory Resource (C++17)
- `std::pmr::memory_resource` 기반 클래스
- `std::pmr::polymorphic_allocator` 래퍼
- 런타임 다형성을 통한 유연한 메모리 관리

### 3. 메모리 풀 기반 할당
- 고정 크기 블록 풀
- 가변 크기 풀 (monotonic, unsynchronized)
- 단계별 메모리 할당 최적화

### 4. 성능 최적화 사례
- 객체 풀 패턴
- 메모리 재사용 및 단편화 감소
- 할당 전략의 벤치마크

## 과제 설명

### homework.cpp
- 기본 커스텀 할당자 구현
- 할당/해제 횟수 추적
- `std::vector`, `std::map` 등과 함께 사용

### homework2.cpp
- std::pmr 기반 다양한 할당 전략
- 메모리 풀 구현 및 활용
- 성능 비교 분석

### game1.cpp
- 동적 객체 생성 게임 (메모리 풀 활용)
- 사용자 입력 기반 인벤토리 관리
- 메모리 효율성 테스트

## 참조
- "C++ Standard Library" (Nicolai Josuttis) - Chapter on Allocators
- cppreference.com - std::allocator, std::pmr
