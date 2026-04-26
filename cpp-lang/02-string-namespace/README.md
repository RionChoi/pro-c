# Session 02: 문자열 & 네임스페이스

## 학습 목표
- `std::string`의 기본 연산(길이, 연결, 부분 문자열)을 학습한다.
- `std::getline`으로 문장 입력을 안정적으로 처리한다.
- 네임스페이스(`namespace`)를 정의하고 `::` 범위 지정 연산자를 사용한다.

## 파일 설명
- `homework.cpp`: 문자열 분석기 (문자 수, 공백 수, 첫/마지막 문자)
- `homework2.cpp`: 문자열 유틸리티 네임스페이스 구현 (대문자화, 단어 수 세기)
- `game1.cpp`: 단어 뒤집기 퀴즈 게임

## 실행 예시
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic homework2.cpp -o homework2
./homework2
```
