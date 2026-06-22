# Session 11: 템플릿 (Templates)

## 학습 목표
- 함수 템플릿으로 타입 독립적인 코드를 작성한다.
- 클래스 템플릿의 기본 구조를 이해한다.
- 템플릿을 사용해 중복 코드를 줄이는 방법을 익힌다.

## 파일 구성
- `homework.cpp`: 함수 템플릿(최대값 찾기, 배열 출력)
- `homework2.cpp`: 클래스 템플릿(Box)과 평균 계산 템플릿
- `game1.cpp`: 템플릿 기반 숫자 비교 미니 게임

## 실행 예시
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wnull-dereference -Wdouble-promotion homework.cpp -o /tmp/session11_hw
./tmp/session11_hw
```
