# Session 39: C++ DSL 설계 (Fluent Interface & Builder)

## 학습 목표
- **DSL** (Domain-Specific Language) 개념
- **Fluent Interface** 패턴
- **Builder 패턴** 활용
- 메서드 체이닝을 통한 직관적 API 설계

## 핵심 개념

### DSL이란?
특정 문제 영역을 해결하기 위한 언어/API.  
C++에서는 메서드 체이닝, 연산자 오버로딩, 템플릿 등으로 구현.

### Fluent Interface
메서드가 자기 자신(`*this`)을 반환하여 연쇄 호출 가능:
```cpp
query.select("name").where("age > 18").orderBy("name");
```

### Builder 패턴
복잡한 객체 생성을 단계적으로 수행:
```cpp
Person person = PersonBuilder()
    .withName("John")
    .withAge(30)
    .withCity("Seoul")
    .build();
```

## 과제 설명

1. **homework.cpp** — SQL 쿼리 빌더 (Fluent Interface)
2. **homework2.cpp** — HTTP 요청 빌더 (Builder Pattern)
3. **game1.cpp** — 게임 캐릭터 생성 DSL

---
