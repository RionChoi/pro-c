# 세션 07 — 동적 메모리 (Dynamic Memory)

## 학습 목표
- `malloc`, `calloc`, `realloc`, `free`의 사용 시점을 익힌다.
- 사용자 입력 크기에 맞춰 메모리를 동적으로 관리한다.
- 할당 실패, 입력 오류, 범위 오류를 안전하게 처리한다.

## 핵심 개념

### 1) `malloc` / `free`
```c
int *arr = (int *)malloc(sizeof(int) * n);
if (arr == NULL) {
    fprintf(stderr, "메모리 할당 실패\n");
    return 1;
}
free(arr);
```

### 2) `realloc`
```c
int *tmp = (int *)realloc(arr, sizeof(int) * new_size);
if (tmp == NULL) {
    free(arr);
    return 1;
}
arr = tmp;
```

### 3) 입력 검증
- 모든 `scanf`의 반환값 검사
- 문자열 입력은 `fgets` 사용
- 오류 메시지는 `stderr`로 출력

## 과제
1. **homework.c** — 동적 배열 점수 분석기
2. **homework2.c** — `realloc` 기반 확장 가계부

## 게임
- **game1.c** — 동적 맵 보물 탐험 게임

## 참조
- Bro Code C Full Course — Dynamic Memory 파트
