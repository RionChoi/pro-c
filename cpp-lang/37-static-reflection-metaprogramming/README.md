# Session 37: 정적 리플렉션 & 고급 메타프로그래밍 (Static Reflection & Advanced Metaprogramming)

## 학습 목표
- 컴파일 타임 리플렉션(Reflection) 구현
- 타입 정보를 런타임에 활용하기
- 메타함수(Meta-function) 설계 패턴
- 고급 SFINAE와 개념(Concepts)

## 핵심 개념

### 정적 리플렉션 (Static Reflection)
- 컴파일 타임에 타입 정보 수집
- `constexpr` 함수와 `std::tuple` 활용
- 구조체 멤버 자동 탐색
- 직렬화/역직렬화 자동화

### 메타프로그래밍 (Metaprogramming)
- 템플릿을 이용한 타입 기반 프로그래밍
- 타입 트레잇(Type Traits) 설계
- 컴파일 타임 계산
- 타입 변환 및 래핑

### SFINAE (Substitution Failure Is Not An Error)
- 템플릿 인스턴스화 실패 처리
- 오버로드 해결(Overload Resolution)
- `std::enable_if` vs `std::void_t`
- 개념으로 대체 (C++20)

### 리플렉션 라이브러리
- 멤버 정보 저장소
- 자동 직렬화/역직렬화
- 동적 생성 및 복사
- 타입 안전성 보장

## 과제 설명

### homework.cpp
기본 메타프로그래밍 기법:
- `TypeTraits` 클래스: 타입 특성 쿼리
- `IsPointer`, `IsArray`, `IsFunction` 특성
- `RemovePointer`, `AddPointer` 타입 변환
- `std::integral_constant` 활용

### homework2.cpp
구조체 멤버 리플렉션:
- `Member` 구조체: 멤버 이름과 포인터 저장
- `ClassInfo` 클래스: 구조체 메타정보 정의
- 컴파일 타임 멤버 등록
- `for_each` 함수로 모든 멤버 순회
- 패턴 매칭 예제

### game1.cpp
**동적 객체 관리 게임**:
- 플레이어와 몬스터 리플렉션 정의
- 게임 엔티티 자동 복제 및 비교
- 객체 풀(Object Pool) 리플렉션 기반 구현
- 상태 저장 및 복원
- 경전투 시뮬레이션

## 컴파일 및 실행

```bash
# 컴파일
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp -o hw37_traits
g++ -std=c++17 -Wall -Wextra -Wpedantic homework2.cpp -o hw37_reflection
g++ -std=c++17 -Wall -Wextra -Wpedantic game1.cpp -o hw37_game

# 실행
./hw37_traits
./hw37_reflection
./hw37_game
```

## 핵심 포인트
- 메타프로그래밍은 제네릭 코드의 기초
- 리플렉션으로 보일러플레이트 코드 제거
- C++20 Concepts가 SFINAE를 단순화
- 타입 안전성과 유연성 동시 달성
