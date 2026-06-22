# Session 40: 최종 마스터 프로젝트 (Master Final Project)

## 학습 목표
- Phase 4 (Sessions 31-40)의 모든 고급 C++ 개념 통합 적용
- 실제 프로덕션 수준의 애플리케이션 설계 및 구현
- Type safety, performance, 메모리 안전성 동시 달성
- 복잡한 C++ 아키텍처의 마스터리 입증

## 핵심 개념 통합

### 1. Type Erasure & Variant (Session 31)
- `std::variant`를 활용한 타입 안전한 다형성

### 2. constexpr 메타프로그래밍 (Session 33)
- 컴파일 타임 설정 및 최적화

### 3. 커스텀 할당자 & std::pmr (Session 34)
- 효율적인 메모리 관리
- 메모리 풀 기반 할당

### 4. 표현식 템플릿 (Session 35)
- 지연 계산(lazy evaluation)
- 성능 최적화

### 5. 직렬화 & 이진 프로토콜 (Session 36)
- 이진 포맷으로 데이터 저장/복원

### 6. ECS 아키텍처 (Session 24)
- Entity-Component-System 게임 엔진 패턴

### 7. C++ DSL 설계 (Session 39)
- Fluent Interface 패턴
- 선언적 API 설계

## 과제

### homework.cpp
**고급 데이터 구조 - 타입 안전 컨테이너 라이브러리**
- `std::variant` 기반 다형 컨테이너 (int, double, string)
- 타입별 통계 수집 (count, average, min, max)
- 커스텀 PoolAllocator 메모리 관리
- 이진 직렬화/역직렬화

### homework2.cpp
**성능 최적화 - 수치 계산 엔진**
- N차원 Vector 템플릿 클래스 (add, multiply, dot, magnitude, normalize)
- M×N Matrix 템플릿 클래스 (multiply, transpose)
- Expression Templates: VectorExpr, AddExpr, ScaleExpr
- constexpr 기반 차원 검증

### game1.cpp
**통합 프로젝트 - 게임 엔진 프로토타입 (ECS)**
- 컴포넌트: Position, Velocity, Health, Inventory (std::variant)
- Entity/GameWorld 시스템
- EntityBuilder DSL 패턴
- 100개 엔티티 물리 시뮬레이션
- 게임 상태 이진 직렬화/역직렬화

## 컴파일

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
    -Wnull-dereference -Wdouble-promotion homework.cpp -o hw40

g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
    -Wnull-dereference -Wdouble-promotion homework2.cpp -o hw40_2

g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion \
    -Wnull-dereference -Wdouble-promotion game1.cpp -o game40
```

## 학습 완료

이 프로젝트는 40세션 C/C++ 학습의 최종 마스터 과정으로:
- 기초부터 고급까지의 모든 개념 통합
- 실전 애플리케이션 수준의 코드 품질
- 성능과 안전성의 균형
- 전문가 수준의 C++ 아키텍처 설계 능력 입증
