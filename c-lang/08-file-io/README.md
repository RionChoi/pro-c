# 세션 08 — 파일 I/O (File Input/Output)

## 학습 목표
- `fopen`, `fscanf`, `fprintf`, `fgets`, `fclose` 흐름을 익힌다.
- 파일 읽기/쓰기 중 발생하는 오류를 안정적으로 처리한다.
- 파일 데이터를 동적 메모리와 결합해 분석 프로그램을 작성한다.

## 핵심 개념

### 1) 파일 열기/닫기
```c
FILE *fp = fopen("data.txt", "r");
if (fp == NULL) {
    fprintf(stderr, "파일 열기 실패\n");
    return 1;
}
fclose(fp);
```

### 2) 정수 반복 읽기
```c
int value;
while (fscanf(fp, "%d", &value) == 1) {
    sum += value;
}
```

### 3) 문자열 입력 + 파일 기록
- 사용자 입력은 `fgets` 사용
- 필요 시 `strcspn`으로 줄바꿈 제거 후 저장

## 과제
1. **homework.c** — 텍스트 파일 정수 합계 계산기
2. **homework2.c** — 동적 배열 기반 파일 통계 분석기

## 게임
- **game1.c** — 추측 기록/최고 기록 파일 저장 숫자 맞히기

## 참조
- Bro Code C Full Course — File I/O 파트
