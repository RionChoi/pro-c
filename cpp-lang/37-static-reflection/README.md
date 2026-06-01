# Session 37: 정적 리플렉션 & 고급 메타프로그래밍 (Static Reflection & Advanced Metaprogramming)

## 학습 목표
1. **정적 리플렉션** — 컴파일 타임에 타입 정보 추출
2. **SFINAE** — Substitution Failure Is Not An Error 기법
3. **Concepts** — C++20 제약 조건 정의
4. **타입 특성(Traits)** — 메타 함수를 통한 타입 분류

## 핵심 개념

### 정적 리플렉션(Static Reflection)이란?
- 컴파일 타임에 타입의 구조 정보를 분석
- 동적 리플렉션과 달리 **0 오버헤드**
- C++17 이후 가능한 기법들

### SFINAE(Substitution Failure Is Not An Error)
- 템플릿 특수화 실패가 컴파일 에러가 아님
- 함수 오버로드 해석을 가능하게 함
- `std::enable_if`로 조건부 컴파일

### Type Traits
- `is_integral<T>::value` — T가 정수형인가?
- `std::remove_reference<T>` — 참조 제거
- `std::is_same<T, U>` — T와 U가 같은가?

### Concepts (C++20)
- 템플릿 파라미터 제약
- 컴파일 에러 메시지 개선
- 가독성 향상

## 학습 구성

| 파일 | 주제 |
|------|------|
| `homework.cpp` | 타입 특성(Traits) 구현 및 활용 |
| `homework2.cpp` | 정적 다형성 (CRTP + Traits) |
| `game1.cpp` | Concepts 기반 알고리즘 - 던전 레이드 게임 |

---

## 구현 가이드

### homework.cpp: 타입 특성 구현
**요구사항**:
- `is_pointer<T>` — T가 포인터인가?
- `is_const<T>` — T가 const인가?
- `is_integral<T>` — T가 정수형인가?
- `remove_const<T>` — const 제거
- `decay<T>` — 배열/함수를 포인터로 변환
- 각 traits를 템플릿 특수화로 구현
- 타입 분류 함수 작성

---

### homework2.cpp: CRTP + Traits 정적 다형성
**요구사항**:
- `Shape` CRTP 베이스 클래스
- `Circle`, `Rectangle` 파생 클래스
- `area()` 메서드 컴파일 타임 계산
- `is_shape<T>` trait으로 타입 검증
- 제약 함수: Shape 타입만 받는 `calculate_total_area()`

---

### game1.cpp: Concepts 기반 게임 알고리즘
**요구사항**:
- `Damageable` Concept — `take_damage()` 필요
- `Attackable` Concept — `attack()` 필요
- 몬스터, 플레이어 클래스
- Concept 기반 함수 템플릿:
  - `battle(Attackable&, Damageable&)`
  - `heal(Damageable&)`
- 던전 레이드 게임:
  1. 캐릭터 생성
  2. 몬스터와 전투
  3. 회복
  4. 보스전
  5. 게임 종료

---

## 학습 포인트
- ✅ SFINAE와 enable_if의 실제 활용
- ✅ 타입 특성으로 메타 함수 구현
- ✅ CRTP를 통한 정적 다형성
- ✅ Concepts 문법 (C++20)
- ✅ 컴파일 타임 다형성의 성능 이점

## 컴파일 명령어

**C++17 버전 (CRTP 활용)**:
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp -o hw1
g++ -std=c++17 -Wall -Wextra -Wpedantic homework2.cpp -o hw2
```

**C++20 버전 (Concepts 활용)**:
```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic game1.cpp -o game1
```

