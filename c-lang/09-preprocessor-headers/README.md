# C 세션 09: 전처리기 & 헤더

## 학습 목표
- `#define` 매크로와 상수 정의를 이해한다.
- 헤더 파일 선언/구현 분리 개념을 익힌다.
- include guard의 필요성을 이해한다.

## 파일 구성
- `homework.c`: 매크로를 활용한 기본 계산기
- `homework2.c`: 조건부 컴파일로 디버그 출력 제어
- `game1.c`: 매크로 기반 업다운 게임

## 실행 예시
```bash
clang -std=c11 -Wall -Wextra -Wpedantic homework.c -o homework
./homework
```
