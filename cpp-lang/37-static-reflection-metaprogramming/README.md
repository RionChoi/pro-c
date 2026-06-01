# Session 37: 정적 리플렉션 & 고급 메타프로그래밍 (Static Reflection & Advanced Metaprogramming)

## 학습 목표
- 컴파일 타임에 타입 정보 추출 및 조작
- 메타프로그래밍을 이용한 제너릭 코드 자동 생성
- 정적 리플렉션의 설계 패턴 이해
- C++20 리플렉션 제안 (P1061) 시뮬레이션

## 핵심 개념

### 1. 정적 리플렉션(Static Reflection)이란?
컴파일 타임에 타입의 구조(멤버, 함수 등)에 관한 정보를 추출하는 기법

### 2. SFINAE와 템플릿 메타프로그래밍
```cpp
template <typename T>
std::enable_if_t<std::is_integral_v<T>> process(T x) { ... }
```

### 3. 구조체 바인딩과 메타정보
```cpp
struct Point { int x, y; };
auto [x, y] = point;  // 컴파일 타임 멤버 접근
```

### 4. std::tuple을 이용한 동적 필드 처리
```cpp
auto fields = std::make_tuple(obj.field1, obj.field2, ...);
std::apply([](auto... args) { ... }, fields);
```

## 과제 설명

### `homework.cpp`
**목표**: 기본 정적 리플렉션 구현
- 클래스의 멤버 개수 계산 (구조체 바인딩 이용)
- 각 멤버의 타입 이름 출력
- 멤버별 크기(sizeof) 표시

### `homework2.cpp`
**목표**: 고급 메타프로그래밍 - 제너릭 직렬화
- 구조체의 모든 멤버를 자동으로 JSON으로 변환
- std::tuple 포장을 통한 메타루핑
- 임의의 구조체에 적용 가능

### `game1.cpp`
**목표**: 메타프로그래밍을 활용한 객체 검증 게임
- 플레이어 통계를 자동 검증 (범위 체크)
- 직렬화된 데이터 무결성 검증
- 리더보드 자동 생성 및 정렬

## 참고
- C++17 `std::tuple`, `std::apply`
- C++20 `<concepts>` (선택사항)
- 구조화된 바인딩 (Structured Bindings)
