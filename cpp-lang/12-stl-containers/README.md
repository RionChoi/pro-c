# Session 12: STL 컨테이너 (STL Containers)

## 학습 목표
- `std::vector`, `std::deque`, `std::map`의 기본 사용법을 익힌다.
- 컨테이너의 삽입/삭제/탐색 패턴을 학습한다.
- 실제 문제를 컨테이너로 모델링하는 연습을 한다.

## 파일 구성
- `homework.cpp`: vector와 deque를 활용한 점수 관리
- `homework2.cpp`: map을 활용한 단어 빈도 분석
- `game1.cpp`: vector 기반 숫자 빙고 게임

## 실행 예시
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wnull-dereference -Wdouble-promotion homework.cpp -o /tmp/session12_hw
./tmp/session12_hw
```
