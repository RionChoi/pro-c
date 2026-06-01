# Session 37: 정적 리플렉션 & 고급 메타프로그래밍 (Static Reflection & Advanced Metaprogramming)

## 학습 목표
- 컴파일 타임에 타입 정보 접근 및 조작
- SFINAE를 활용한 함수 오버로드
- constexpr를 이용한 정적 리플렉션
- 템플릿 특수화를 통한 조건부 컴파일
- 타입 특성(type traits) 구현 및 활용

## 핵심 개념

### 1. SFINAE (Substitution Failure Is Not An Error)
- 템플릿 특수화 실패가 오류가 아님
- 함수 오버로드 해결에 활용

### 2. 정적 리플렉션
- 컴파일 타임에 타입 정보 수집
- constexpr 함수로 구현

### 3. Type Traits
- 타입의 성질을 컴파일 타임에 판단
- is_integral, is_pointer 등

### 4. concepts (C++20)
- 템플릿 매개변수 제약
- 더 명확한 오류 메시지

### 5. 메타함수
- 컴파일 타임 계산
- 재귀적 템플릿 특수화

## 파일 구성

| 파일 | 설명 |
|------|------|
| `homework.cpp` | SFINAE와 type traits 기초 |
| `homework2.cpp` | constexpr 리플렉션과 컴파일 타임 프로그래밍 |
| `game1.cpp` | 메타프로그래밍을 활용한 RPG 시스템 |

## 컴파일 및 실행

```bash
# homework.cpp
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp -o hw37
./hw37

# homework2.cpp
g++ -std=c++17 -Wall -Wextra -Wpedantic homework2.cpp -o hw37_2
./hw37_2

# game1.cpp (C++20 features used)
g++ -std=c++20 -Wall -Wextra -Wpedantic game1.cpp -o game37
./game37
```

## 학습 포인트
- enable_if를 이용한 조건부 함수 정의
- is_same, is_integral 등 타입 검사
- constexpr 리플렉션의 활용
- 컴파일 타임과 런타임의 경계
- 메타프로그래밍의 실용적 응용
