# Session 37: 정적 리플렉션 & 고급 메타프로그래밍 (Static Reflection & Advanced Metaprogramming)

## 학습 목표
- 컴파일 타임 리플렉션(reflection) 구현
- `std::tuple`과 `std::index_sequence` 활용
- 타입 특성(type traits)과 SFINAE 심화
- 구조체의 멤버 정보 동적 추출 (C++20 방식)
- 범용 직렬화/역직렬화 구현

## 핵심 개념

### 1. 정적 리플렉션
컴파일 타임에 타입의 메타데이터 수집.
- 멤버 이름, 타입, 개수 등을 구조체로부터 추출

### 2. Tuple 기반 구조체 분석
`std::tuple_element`, `std::tuple_size` 활용으로 멤버 조회.

### 3. 매개변수 팩 (Parameter Pack) 압축풀기
`std::index_sequence`로 각 멤버에 접근.

### 4. SFINAE와 Concepts
함수 오버로드 및 템플릿 특수화 조건.

### 5. 포장/언팩 (Packing/Unpacking)
구조체 ↔ 튜플 변환을 통한 유연한 데이터 처리.

## 파일 구성
- `homework.cpp` — Tuple 기반 구조체 분석
- `homework2.cpp` — 범용 직렬화 (다양한 타입 처리)
- `game1.cpp` — 리플렉션을 활용한 고급 게임 엔진

## 사전 개념
- `std::tuple` 및 유틸리티
- `std::type_traits` (is_same, is_integral 등)
- `std::index_sequence` (C++14)
- 가변인자 템플릿 (Variadic Templates)
