# Session 13: 에러 처리 (Error Handling)

## 학습 목표
C 언어에서의 다양한 에러 처리 기법을 이해하고 적용한다.

## 핵심 개념

### 1. errno와 에러 코드
- `<errno.h>`의 `errno` 전역 변수
- 주요 에러 코드: `EDOM`, `ERANGE`, `ENOENT`, `EINVAL`
- `perror()`: 시스템 에러 메시지 출력
- `strerror()`: 에러 코드를 문자열로 변환

### 2. assert (단정)
- `<assert.h>`의 `assert()` 매크로
- 디버깅 시 조건 검증
- `NDEBUG` 매크로 정의 시 비활성화

### 3. setjmp / longjmp (비-local 점프)
- `setjmp()`: 현재 실행 위치 저장 (처음 호출 시 0 반환)
- `longjmp()`: 저장된 위치로 복귀 (두 번째 호출 시 0 아닌 값 반환)
- 복잡한 에러 처리 시나리오에 사용

### 4. 종료 함수 (atexit)
- `atexit()`: 프로그램 종료 시 호출될 함수 등록
- 정리 작업에 활용

## 과제 구성
| 파일 | 내용 |
|------|------|
| `homework.c` | 파일 계산기 — 다양한 에러 상황을 처리 |
| `homework2.c` | 문자열 파싱 — 입력 검증과 오류 처리 |
| `game1.c` | 숫자 맞추기 게임 — 에러 처리 적용 |
