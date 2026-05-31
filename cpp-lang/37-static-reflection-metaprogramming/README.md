# Session 37: 정적 리플렉션 & 고급 메타프로그래밍 (Static Reflection & Advanced Metaprogramming)

## 학습 목표
- 정적 리플렉션(Static Reflection) 기초
- std::tuple과 인덱스 수열 활용
- CRTP (Curiously Recurring Template Pattern)
- 컴파일 타임 구조체 내성(Introspection)
- 메타프로그래밍 패턴 심화
- 타입 특성 추출 및 활용

## 핵심 개념

### 1. 정적 리플렉션의 필요성
- 컴파일 타임에 타입 정보 분석
- 제네릭 코드의 타입별 동작 커스터마이징
- 직렬화/역직렬화 자동화
- 타입 안전성 보장

### 2. std::tuple과 인덱스 수열
- 다양한 타입을 하나의 컨테이너에 저장
- 인덱스 수열로 튜플 요소 순회
- 타입 인덱싱 및 접근
- 가변 길이 템플릿 활용

### 3. CRTP 패턴
- 정적 다형성(static polymorphism)
- 가상 함수 오버헤드 제거
- 컴파일 타임 디스패치
- Mixins 구현

### 4. 컴파일 타임 내성
- 타입의 멤버 수 파악
- 각 멤버의 타입 추출
- 멤버 레이아웃 분석
- 제네릭 접근자(accessor) 생성

## 실습 내용
- 튜플 기반 다형성 프로그래밍
- CRTP를 활용한 정적 다형성
- 타입 특성 추출 및 활용
- 구조체 멤버 순회 및 조작
- 컴파일 타임 알고리즘

