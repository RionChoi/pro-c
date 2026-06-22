# Session 35: 표현식 템플릿 (Expression Templates)

## 학습 목표
- 표현식 템플릿(Expression Templates) 패턴 이해
- Lazy evaluation으로 성능 최적화
- 임시 객체 생성 최소화
- DSL(Domain-Specific Language) 구현

## 핵심 개념

### 1. 표현식 트리
- 표현식을 트리 구조로 표현
- 평가를 지연시켜 최적화 수행

### 2. Lazy Evaluation
- 표현식을 즉시 계산하지 않음
- 필요할 때만 계산 수행
- 임시 객체 생성 제거

### 3. 메타프로그래밍
- CRTP(Curiously Recurring Template Pattern)
- 컴파일 타임 최적화

## 실습 내용
- 벡터 표현식 템플릿
- 행렬 연산 최적화
- 수학 표현식 DSL 구현
- 성능 비교 및 분석
