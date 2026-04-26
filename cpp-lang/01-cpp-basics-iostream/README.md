# Session 01: C++ 기초 & iostream

## 학습 목표
- C++ 프로그램 기본 구조(`int main()`)를 이해한다.
- `std::cout`, `std::cin`, `std::getline`으로 입출력을 다룬다.
- 기본 자료형(`int`, `double`, `char`, `std::string`)을 사용한다.

## 파일 설명
- `homework.cpp`: 이름/나이/키를 입력받아 자기소개를 출력하는 기본 입출력 연습
- `homework2.cpp`: 과목 점수 3개를 입력받아 총점/평균/학점을 계산
- `game1.cpp`: 업다운 숫자 맞추기 게임 (난수 + 반복문)

## 실행 예시
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic homework.cpp -o homework
./homework
```
