# Session 34: 커스텀 할당자 & std::pmr

## 학습 목표
- C++17 Polymorphic Memory Resource (PMR) 이해
- 커스텀 할당자 구현 방법 학습
- 메모리 풀과 리소스 관리 최적화
- 할당자 특성(allocator traits) 활용

## 핵심 개념

### 1. std::pmr::memory_resource
- 메모리 할당/해제를 다형적으로 관리
- 상속을 통해 커스텀 할당 전략 구현

### 2. std::pmr::polymorphic_allocator
- 런타임에 메모리 리소스를 결정
- std::vector, std::string 등의 컨테이너에 사용

### 3. 미리 정의된 메모리 리소스
- `std::pmr::new_delete_resource()` — new/delete 사용
- `std::pmr::monotonic_buffer_resource` — 선형 할당
- `std::pmr::unsynchronized_pool_resource` — 메모리 풀

## 실습 내용
- 커스텀 메모리 리소스 구현
- 메모리 풀을 통한 성능 최적화
- 컨테이너에서 PMR 활용
