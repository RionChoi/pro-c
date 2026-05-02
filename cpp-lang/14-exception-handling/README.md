# 세션 14: 예외 처리 (Exception Handling)

## 학습 목표
- C++의 예외 처리 메커니즘을 이해한다.
- try-catch-throw 문법을 학습한다.
- 표준 예외 클래스와 커스텀 예외를 작성한다.
- RAII 패턴과 예외 안전성을 학습한다.

## 핵심 개념
1. **기본 예외 처리**
   - `throw` — 예외 발생
   - `try` — 예외 발생 가능 블록
   - `catch` — 예외 처리 블록

2. **표준 예외 클래스**
   - `std::exception` — 모든 표준 예외의 기본 클래스
   - `std::runtime_error` — 런타임 오류
   - `std::invalid_argument` — 잘못된 인자
   - `std::out_of_range` — 범위 초과
   - `std::logic_error` — 논리 오류

3. **예외 안전성**
   - 강한 보장 (strong guarantee)
   - 기본 보장 (basic guarantee)
   - 예외 불가능 보장 (no-throw guarantee)

4. **RAII (Resource Acquisition Is Initialization)**
   - 생성자에서 리소스 획득
   - 소멸자에서 리소스 해제
   - 예외 발생 시에도 리소스 안전하게 해제

5. **커스텀 예외**
   - std::exception 상속
   - what() 메서드 구현
   - 특화된 예외 타입 정의

## 파일 설명
- **homework.cpp**: try-catch-throw 기초, 표준 예외 처리
- **homework2.cpp**: 커스텀 예외 클래스, 중첩된 try-catch
- **game1.cpp**: 예외 처리를 활용한 ATM 시스템

## 참조 자료
- Bro Code C++ 강좌: 예외 처리 섹션
- cppreference.com: `<stdexcept>` 헤더 문서
