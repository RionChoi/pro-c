# Session 37: 정적 리플렉션 & 고급 메타프로그래밍 (Static Reflection & Advanced Metaprogramming)

## 목표
- C++의 타입 특성(Type Traits)과 SFINAE 활용
- 컴파일 타임에 타입 정보 검사 및 활용
- 제네릭 직렬화를 위한 리플렉션 기법
- 멤버 감지(Member Detection) 패턴
- 타입 안전한 유틸리티 작성

## 학습 내용

### 1. Type Traits (타입 특성)
- `std::is_integral`, `std::is_floating_point` 등의 기본 특성
- 커스텀 타입 특성 만들기
- SFINAE를 이용한 조건부 컴파일

### 2. 멤버 감지 패턴 (Member Detection)
- decltype과 sizeof를 이용한 멤버 존재 여부 확인
- 다양한 멤버 함수/변수 검사
- enable_if와 함께 사용

### 3. 제네릭 직렬화
- 리플렉션 기반 자동 직렬화
- 타입별 처리 분기
- 컨테이너 감지 및 처리

### 4. 함수 오버로드 해석
- decltype으로 함수 시그니처 분석
- 반환 타입 추론
- 함수 특성 검사

## 과제

### homework.cpp
다양한 타입 특성 검사 및 SFINAE를 사용한 조건부 함수 구현:
- 정수/부동소수 타입 판별
- 포인터/참조 타입 판별
- 커스텀 특성 작성
- 멤버 변수 감지

### homework2.cpp
리플렉션 기반 제네릭 직렬화:
- 구조체 멤버의 자동 감지
- 타입별로 적절한 직렬화 전략
- 복합 타입(struct inside struct) 지원
- 컨테이너 자동 처리

### game1.cpp
**타입 안전 게임 엔진**:
제네릭 컴포넌트 시스템에서 리플렉션을 활용해:
- 엔티티의 컴포넌트 자동 등록
- 타입 안전한 컴포넌트 쿼리
- 직렬화 가능한 게임 상태 저장
