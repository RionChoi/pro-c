# pro-c

C 언어 학습 프로젝트입니다.

## 오늘 학습 요약

오늘은 기초부터 실습 중심으로 4개 과정을 정리했습니다.

1. 변수와 입출력
- `int`, `double`, `const` 사용
- `printf` 포맷(`%d`, `%.1f`) 이해
- `scanf` 입력과 반환값 검사의 필요성

2. 조건문과 반복문
- `if / else if / else`
- `for`, `while`
- 메뉴 기반 반복 처리 패턴

3. 함수와 배열
- 함수 분리(평균/최대/학점 변환)
- 배열 순회 및 통계 처리
- 재사용 가능한 코드 구조

4. 포인터와 문자열(예정/확장)
- 포인터 기초 개념
- 문자열 입력/처리 기반 확장 과제 준비

## 오늘 개선한 코드 품질 포인트

- `main(void)` 형태 권장
- `scanf` 반환값 검증으로 입력 안정성 강화
- 0으로 나누기 같은 예외 처리 추가
- 함수 분리로 가독성/유지보수성 개선

## 프로젝트 구조

```text
pro-c/
├── README.md
├── Helloworl.c
├── docs/
│   ├── LEARNING_ROADMAP.md
│   └── CODE_REVIEW.md
└── course/
    ├── 01-variables-io/
    │   ├── README.md
    │   └── homework.c
    ├── 02-condition-loop/
    │   ├── README.md
    │   └── homework.c
    ├── 03-function-array/
    │   ├── README.md
    │   └── homework.c
    └── 04-pointers-strings/
        ├── README.md
        └── homework.c
```

## 실행 방법

```bash
cc -Wall -Wextra -Wpedantic Helloworl.c -o hello
./hello
```

과제 실행 예시:

```bash
cc -Wall -Wextra -Wpedantic course/01-variables-io/homework.c -o hw1
./hw1
```

## 다음 학습 권장

- 배열 + 포인터 연계 문제
- 문자열 함수(`strlen`, `strcpy`, `strcmp`) 안전 사용
- 파일 입출력(`fopen`, `fgets`, `fprintf`) 기초
- 미니 CLI 프로젝트(메뉴형)
