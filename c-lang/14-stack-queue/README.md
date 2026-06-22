# Session 14: 스택 & 큐 (Stack & Queue)

## 학습 목표
스택과 큐의 자료구조 개념을 이해하고 C로 구현한다.

## 핵심 개념

### 스택 (Stack) — LIFO (Last In, First Out)
- `push`: 최상단 삽입
- `pop`: 최상단 제거
- `top`: 최상단 조회
- `isEmpty` / `isFull`: 상태 확인

### 큐 (Queue) — FIFO (First In, First Out)
- `enqueue`: 뒤쪽 삽입
- `dequeue`: 앞쪽 제거
- `front`: 앞쪽 조회
- `isEmpty` / `isFull`: 상태 확인

### 구현 방식
- **배열 기반** 구현 (고정 크기)
- 스택: top指针로 관리
- 큐: front/ rear指针로 환형 버퍼(circular buffer) 구현

## 과제 구성
| 파일 | 내용 |
|------|------|
| `homework.c` | 스택 구현 — push/pop/top 연산 |
| `homework2.c` | 큐 구현 — enqueue/dequeue/front 연산 |
| `game1.c` | 괄호 균형 검사기 — 스택 활용 게임 |
