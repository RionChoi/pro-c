# Session 40: 최종 마스터 프로젝트 (Master Final Project)

## 학습 목표
- Phase 4 (Sessions 31-40)의 모든 고급 C++ 개념 통합 적용
- 실제 프로덕션 수준의 애플리케이션 설계 및 구현
- Type safety, performance, 메모리 안전성 동시 달성
- 복잡한 C++ 아키텍처의 마스터리 입증

## 핵심 개념 통합

### 1. Type Erasure & Variant (Session 31)
- `std::variant`를 활용한 타입 안전한 다형성
- 런타임 타입 검사와 비용 없음

### 2. C++20 모듈 (Session 32)
- 모듈화된 코드 구조
- 캡슐화와 재사용성

### 3. constexpr 메타프로그래밍 (Session 33)
- 컴파일 타임 설정 및 최적화
- Zero-overhead abstraction

### 4. 커스텀 할당자 & std::pmr (Session 34)
- 효율적인 메모리 관리
- 메모리 풀 기반 할당

### 5. 표현식 템플릿 (Session 35)
- 지연 계산(lazy evaluation)
- 성능 최적화

### 6. 직렬화 & 이진 프로토콜 (Session 36)
- 이진 포맷으로 데이터 저장/복원
- 플랫폼 독립적 인코딩

### 7. 정적 리플렉션 & 메타프로그래밍 (Session 37)
- 런타임 타입 정보 활용
- 동적 디스패치 구현

### 8. SIMD & 컴파일러 인트린직 (Session 38)
- 벡터화된 계산
- 성능 크리티컬 영역 최적화

### 9. C++ DSL 설계 (Session 39)
- Fluent Interface 패턴
- 선언적 API 설계

## 과제

### homework.cpp
**고급 데이터 구조 - 타입 안전 컨테이너 라이브러리**

복합 타입을 안전하게 저장하는 컨테이너 시스템:
- `std::variant` 기반 다형 컨테이너
- 타입별 통계 수집 (constexpr 기반 설정)
- 커스텀 할당자를 활용한 메모리 관리
- 이진 직렬화/역직렬화

예:
```cpp
TypeSafeStorage store;
store.insert(42);
store.insert("hello");
store.insert(3.14);
store.print();  // 타입별 통계 출력
```

### homework2.cpp
**성능 최적화 - 수치 계산 엔진**

SIMD 활용 및 표현식 템플릿 기반 벡터 연산:
- 벡터 덧셈/곱셈 (SIMD 최적화)
- 행렬 연산
- constexpr 기반 차원 검증
- 지연 계산으로 불필요한 임시 객체 제거

예:
```cpp
Vector<4> v1{1, 2, 3, 4};
Vector<4> v2{5, 6, 7, 8};
auto result = v1 + v2;  // 지연 계산
```

### game1.cpp
**통합 프로젝트 - 게임 엔진 프로토타입**

RPG 게임 엔진의 최소 구현:
- ECS 아키텍처 (Entity-Component-System)
- 타입 안전한 컴포넌트 시스템
- 직렬화 가능한 게임 상태
- 성능 최적화 (SIMD 기반 월드 업데이트)
- DSL 스타일 엔티티 생성

특징:
- 100개의 게임 엔티티 시뮬레이션
- 컴포넌트: Position, Velocity, Health, Inventory
- 게임 상태 저장/복원
- 프레임별 업데이트 (벡터화된 물리 계산)

예:
```cpp
GameEngine engine;
auto player = engine.createEntity()
    .withPosition(0, 0)
    .withHealth(100)
    .withInventory(5)
    .build();
engine.update(16);  // 16ms tick
engine.save("game.bin");
```

## 컴파일

```bash
# homework.cpp - 타입 안전 컨테이너
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
    -Wnull-dereference -Wdouble-promotion homework.cpp -o hw40

# homework2.cpp - 성능 엔진
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
    -Wnull-dereference -Wdouble-promotion homework2.cpp -o hw40_2

# game1.cpp - 게임 엔진
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
    -Wnull-dereference -Wdouble-promotion game1.cpp -o game40
```

## 실행

```bash
./hw40
./hw40_2
./game40
```

## 평가 기준

✅ Type safety 달성  
✅ 메모리 안전성 (smart pointers, RAII)  
✅ Zero-overhead abstraction  
✅ 코드 재사용성과 모듈화  
✅ 성능 최적화 (SIMD, constexpr)  
✅ 에러 처리 및 견고성  

## 학습 완료

이 프로젝트는 40세션 C/C++ 학습의 최종 마스터 과정으로:
- 기초부터 고급까지의 모든 개념 통합
- 실전 애플리케이션 수준의 코드 품질
- 성능과 안전성의 균형
- 전문가 수준의 C++ 아키텍처 설계 능력 입증
