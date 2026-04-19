# 세션 05 — 구조체 (Structs)

## 학습 목표
- `struct`의 개념과 정의 방법을 이해한다.
- 구조체 멤버에 접근하고 값을 대입하는 방법을 익힌다.
- 구조체 배열을 활용하여 여러 데이터를 관리한다.
- 함수에 구조체를 전달하는 방법을 학습한다.

## 핵심 개념

### 1. 구조체 정의
```c
struct Student {
    char name[64];
    int age;
    double gpa;
};
```

### 2. 구조체 변수 선언과 초기화
```c
struct Student s1 = {"홍길동", 20, 3.85};
```

### 3. 멤버 접근 (`.` 연산자)
```c
printf("이름: %s\n", s1.name);
printf("평점: %.2f\n", s1.gpa);
```

### 4. 구조체 배열
```c
struct Student class[30];
for (int i = 0; i < 30; i++) {
    printf("%s: %.2f\n", class[i].name, class[i].gpa);
}
```

### 5. 함수에 구조체 전달
```c
void print_student(const struct Student *s) {
    printf("%s (%d세) - GPA: %.2f\n", s->name, s->age, s->gpa);
}
```

## 과제
1. **homework.c** — 학생 성적 구조체 관리 (입력/출력/평균)
2. **homework2.c** — 주소록 관리 시스템 (추가/검색/삭제)

## 게임
- **game1.c** — 카드 게임 (구조체 기반 카드 덱)

## 참조
- Bro Code C Full Course — Structs 파트
