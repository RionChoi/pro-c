# 세션 16: 함수 포인터 & 콜백 (Function Pointers & Callbacks)

## 학습 목표
- 함수 포인터의 개념과 용법을 이해한다.
- 콜백 함수의 패턴을 구현한다.
- 함수를 인자로 전달하고 동적으로 호출하는 코드를 작성한다.
- 함수 포인터 배열, 구조체 내 함수 포인터 등을 활용한다.

## 핵심 개념

### 함수 포인터란?
함수도 메모리에 로드되는 코드이며, 그 시작 주소값을 포인터로保存할 수 있다.

### 함수 포인터 선언
```c
/* int를 인자로 받고 int를 반환하는 함수 포인터 */
int (*func_ptr)(int);

/* 더 읽기 쉽게 typedef 사용 */
typedef int (*CompareFunc)(int, int);
```

### 함수 포인터 사용 예시
```c
int add(int a, int b) { return a + b; }
int mul(int a, int b) { return a * b; }

int (*operation)(int, int) = add;
printf("%d\n", operation(3, 4)); /* 7 */
operation = mul;
printf("%d\n", operation(3, 4)); /* 12 */
```

### 콜백 함수 패턴
다른 함수에 함수 포인터를 전달し、必要に応じて呼び出す。
```c
void process(int (*callback)(int)) {
    printf("결과: %d\n", callback(10));
}
```

## 세션 과제

### 과제 1: 기본 함수 포인터 (homework.c)
- 두 수와 연산 함수를 함수 포인터로 연결
- `add`, `sub`, `mul`, `div` 함수를 정의하고 선택적으로 호출
- 메뉴 driven으로 사용자가 연산 선택

### 과제 2: qsort와 콜백 (homework2.c)
- `qsort`를 사용하여 정수 배열, 문자열 배열 정렬
- 사용자 정의 비교 함수를 작성し、関数ポインタで渡す
- 정렬 전/후 상태 출력

### 게임 1: 계산기 with 함수 포인터 (game1.c)
- 사칙연산 + α (제곱, 제곱근, 로그)
- 각 연산이 함수 포인터로 관리
- 히스토리 기능 (최근 10개 연산 기록)
- 사용자가 직접 연산 추가 가능 (함수 포인터 배열)

## 연습 문제
1. 함수 포인터를 구조체 멤버로 사용하기
2. 함수 포인터를 반환하는 함수 만들기 (함수 factory)
3. 함수 포인터 배열로 상태 머신(state machine) 구현
4. 라이브러리頼 콜백 패턴 이해하기 (sort, atexit 등)
