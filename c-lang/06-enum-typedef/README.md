# 세션 06 — Enum & Typedef

## 학습 목표
- `enum`으로 관련 상수들을 그룹으로 정의하는 방법을 배운다.
- `typedef`로 자료형에 별칭을 붙여 코드 가독성을 높인다.
- `enum`과 `struct`를 결합하여 상태 관리 패턴을 익힌다.

## 핵심 개념

### 1. enum 정의
```c
enum Day { SUN, MON, TUE, WED, THU, FRI, SAT };
enum Color { RED = 1, GREEN = 2, BLUE = 4 };
```

### 2. typedef 사용
```c
typedef unsigned int uint;
typedef struct { char name[64]; int age; } Person;
```

### 3. enum + switch 패턴
```c
enum State { IDLE, RUNNING, PAUSED, STOPPED };

void handle_state(enum State s) {
    switch (s) {
        case IDLE:    printf("대기 중\n"); break;
        case RUNNING: printf("실행 중\n"); break;
        case PAUSED:  printf("일시 정지\n"); break;
        case STOPPED: printf("정지\n"); break;
    }
}
```

## 과제
1. **homework.c** — 요일별 스케줄 관리 프로그램
2. **homework2.c** — 상태 머신 시뮬레이터

## 게임
- **game1.c** — RPG 캐릭터 선택 및 전투 시스템

## 참조
- Bro Code C Full Course — Enum, Typedef 파트
