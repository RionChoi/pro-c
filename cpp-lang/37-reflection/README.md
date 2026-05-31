# Session 37: 정적 리플렉션 & 고급 메타프로그래밍 (Static Reflection & Advanced Metaprogramming)

## 학습 목표
- C++17 기반의 정적 리플렉션 구현 방식 이해
- 타입 정보를 컴파일 타임에 추출하는 메타프로그래밍
- SFINAE와 컨셉(C++20)을 활용한 타입 분류
- 제너릭 직렬화 및 자동 필드 열거 구현

## 핵심 개념

### 1. 정적 리플렉션이란
- 컴파일 타임에 타입의 메타데이터를 조사
- 런타임 오버헤드 없음
- C++17: SFINAE, `std::tuple`, `std::enable_if`
- C++20+: `std::is_specialization_v`, Concepts

### 2. 구조체 필드 열거
- Aggregate initialization 활용
- `std::tuple`로 필드 묶음
- Reflection traits 설계
- 자동 필드 수 계산 (aggregate sizeof trick)

### 3. 메타프로그래밍 기법
- Type traits: `std::enable_if`, `std::is_same`
- SFINAE (Substitution Failure Is Not An Error)
- Variable templates
- Function templates with tag dispatch

### 4. 실용적 응용
- 구조체 → JSON 변환
- 구조체 → 테이블 출력
- 선택적 직렬화 (조건부 필드)

## 세션 파일
- `homework.cpp` — 기본 타입 리플렉션과 필드 열거
- `homework2.cpp` — 제너릭 구조체 프린터 구현
- `game1.cpp` — 정적 리플렉션을 활용한 캐릭터 설정 시스템
