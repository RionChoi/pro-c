# 세션 04: 함수 & 오버로딩 (Functions & Overloading)

## 학습 내용
- 함수 선언과 정의
- 매개변수 전달 방식 (값 전달, 참조 전달, const 참조)
- 함수 오버로딩 (Function Overloading)
- 기본 매개변수 (Default Arguments)
- 인라인 함수 (inline)
- 스택 프레임과 호출 규약

## 핵심 개념
C++에서는 같은 이름의 함수가 매개변수 타입이나 개수에 따라 다른 기능을 수행할 수 있다. 이것을 "함수 오버로딩"이라고 한다.

## 과제 1 (homework.cpp)
다음 기능을 수행하는 함수를 작성하세요.
- 두 정수의 합, 차, 곱을 계산하는 함수들
- 같은 이름 `calculate`로 오버로딩하여 구현

## 과제 2 (homework2.cpp)
삼각형, 원, 사각형의 면적을 계산하는 오버로딩된 함수들을 작성하세요.
- `double area(double base, double height)` — 삼각형
- `double area(double radius)` — 원
- `double area(double width, double height)` — 사각형

## 미니 게임 (game1.cpp)
간단한 계산기 게임을 구현하세요.
- 무작위로 덧셈, 뺄셈, 곱셈 문제 생성
- 사용자가 정답을 입력
- 5문제 출제 후 점수 표시
