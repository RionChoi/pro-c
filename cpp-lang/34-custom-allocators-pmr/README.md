# Session 34: 커스텀 할당자 & std::pmr

## 학습 목표
- C++17 std::pmr (Polymorphic Memory Resource) 이해
- 커스텀 메모리 할당자 설계 및 구현
- 메모리 풀과 단선형 할당자 활용
- 성능 최적화를 위한 메모리 관리

## 핵심 개념
1. **std::pmr::memory_resource** — 추상 기반 클래스
2. **std::pmr::monotonic_buffer_resource** — 선형 할당
3. **std::pmr::unsynchronized_pool_resource** — 메모리 풀
4. **std::pmr::synchronized_pool_resource** — 스레드 안전 풀
5. **std::pmr::polymorphic_allocator** — 다형 할당자

## 실습 항목
- homework.cpp: 기본 std::pmr 사용법
- homework2.cpp: 커스텀 memory_resource 구현
- game1.cpp: 메모리 할당자를 사용한 게임 (메모리 제약 환경)

## 컴파일 명령
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp -o hw34
g++ -std=c++17 -Wall -Wextra -Wpedantic homework2.cpp -o hw34_2
g++ -std=c++17 -Wall -Wextra -Wpedantic game1.cpp -o game34
```
