# Session 37: 정적 리플렉션 & 고급 메타프로그래밍 (Static Reflection & Advanced Metaprogramming)

## 학습 목표
- 컴파일 타임 타입 정보 추출 및 활용
- 정적 리플렉션 패턴 구현
- 고급 템플릿 메타프로그래밍 기법
- 타입 특성(type traits)을 이용한 조건부 컴파일
- 구조체 멤버 자동 순회 (Struct Binding)

## 핵심 개념

### 1. Type Traits의 개요
- std::enable_if를 이용한 조건부 템플릿 특수화
- std::is_same, std::is_integral 등으로 타입 검사
- SFINAE (Substitution Failure Is Not An Error)

### 2. 정적 리플렉션 패턴
- 매크로 기반 리플렉션: 메타데이터 등록
- 템플릿 기반 타입 정보 보관
- 멤버 메타데이터 추출 및 순회

### 3. 구조체 멤버 자동 직렬화
```cpp
// 리플렉션을 이용해 구조체 멤버를 자동으로 JSON/이진 형식으로 직렬화
REFLECT(Student, name, age, score);
```

### 4. 고급 메타프로그래밍
- Variadic templates와 parameter packing
- std::tuple과 std::apply
- constexpr 함수로 컴파일 타임 계산
- 타입 리스트 조작

## 과제 설명

### homework.cpp: 기본 Type Traits와 정적 선택
- enable_if를 이용한 정수/부동소수점 타입 구분
- 서로 다른 타입에 대한 조건부 함수 오버로드
- std::is_same, std::is_integral 활용

### homework2.cpp: 구조체 멤버 메타데이터 추출
- 구조체의 멤버를 자동으로 추출하는 리플렉션 시스템 구현
- 멤버 이름과 타입 정보를 매크로로 등록
- 등록된 멤버를 순회하여 정보 출력

### game1.cpp: 동적 JSON 직렬화 게임
- 리플렉션을 이용해 게임 객체를 JSON으로 자동 직렬화
- 플레이어 정보, 인벤토리를 자동으로 JSON 변환
- JSON 역직렬화로 게임 상태 복원

## 컴파일 및 실행

```bash
# homework 컴파일
g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/37-static-reflection-metaprogramming/homework.cpp -o hw37
./hw37

# homework2 컴파일
g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/37-static-reflection-metaprogramming/homework2.cpp -o hw37_2
./hw37_2

# game1 컴파일
g++ -std=c++17 -Wall -Wextra -Wpedantic cpp-lang/37-static-reflection-metaprogramming/game1.cpp -o game37
./game37
```

## 참고 자료
- std::enable_if, std::is_same, std::is_integral
- std::tuple, std::apply
- constexpr와 컴파일 타임 연산
- 매크로를 이용한 메타데이터 등록
- 바리아딕 템플릿 (Variadic Templates)
