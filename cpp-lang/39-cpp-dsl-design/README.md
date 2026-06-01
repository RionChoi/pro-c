# Session 39: C++ DSL 설계 (C++ DSL Design: Fluent Interface & Builder)

## 학습 목표
1. **DSL (Domain-Specific Language)** 개념 이해
2. **Fluent Interface** 패턴으로 자연스러운 API 설계
3. **Builder Pattern** 구현 및 활용
4. **Method Chaining**을 통한 읽기 좋은 코드 작성
5. 실제 DSL 설계 및 구현 경험

## 핵심 개념

### DSL이란?
- 특정 도메인(영역) 문제 해결을 위한 언어
- 일반 목적 언어(General Purpose Language)와 달리 제한된 범위
- SQL, CSS, GraphQL 등이 대표적인 DSL의 예

### Fluent Interface (유창한 인터페이스)
- 메서드 체이닝을 통해 자연스러운 문장 같은 코드 작성
- `return *this;` 패턴으로 메서드 연쇄
- 가독성과 표현력 향상

### Builder Pattern
- 복잡한 객체를 단계별로 구성
- 선택적 매개변수를 우아하게 처리
- 객체 생성 로직을 분리하여 유연성 증대

## 파일 설명
- **homework.cpp**: Fluent Interface 기초 - 쿼리 빌더(Query Builder)
- **homework2.cpp**: Builder 패턴 심화 - HTTP 요청 빌더
- **game1.cpp**: DSL 게임 - SQL 쿼리 빌더 게임 (쿼리 실행 및 검증)

## 컴파일
```bash
# 기본 컴파일
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp -o hw39
g++ -std=c++17 -Wall -Wextra -Wpedantic homework2.cpp -o hw39-2
g++ -std=c++17 -Wall -Wextra -Wpedantic game1.cpp -o game39

# 실행
./hw39
./hw39-2
./game39
```

## 학습 포인트
- **Fluent Interface**는 코드 가독성을 극적으로 향상시킬 수 있음
- **Builder Pattern**은 객체 생성이 복잡할 때 매우 유용
- DSL 설계는 적절한 패턴 조합으로 시작
- 메서드 연쇄와 조건부 실행을 통합하는 방법
- C++ 템플릿과 함께 사용하면 더욱 강력한 DSL 구성 가능
