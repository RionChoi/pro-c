# 코드 리뷰 리포트: pro-c

- 리뷰 시각: 2026-04-10 14:52:00 KST
- 리뷰 대상 C 파일:
  - `Helloworl.c`
- 컴파일 점검 명령:
  - `clang -std=c11 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wformat=2 -Wnull-dereference -Wdouble-promotion -Wstrict-prototypes Helloworl.c`

---

## 1) 컴파일 경고(Warning) 기준

### 확인된 경고
1. **`main` 프로토타입 경고**
   - 경고: `function declaration without a prototype is deprecated` (`-Wstrict-prototypes`)
   - 현재 코드: `int main()`
   - 권장 코드: `int main(void)`

### 권장 수정
```c
int main(void) {
    ...
}
```

---

## 2) 안전성(Safety) 기준

### 이슈 A: 입력 검증 누락
- 현재 코드:
```c
scanf("%d", &num);
```
- 문제점:
  - 숫자가 아닌 입력이 들어오면 `scanf`가 실패할 수 있고, 이후 `num` 사용이 안전하지 않음
- 권장 수정:
```c
if (scanf("%d", &num) != 1) {
    fprintf(stderr, "입력 오류\n");
    return 1;
}
```

### 이슈 B: 입력 버퍼/흐름 고려 부족(학습 단계 권장)
- 향후 `fgets` + `strtol` 패턴으로 확장하면 잘못된 입력 처리와 에러 원인 구분이 쉬워짐

---

## 3) 가독성(Readability) 기준

### 좋은 점
- 주석을 활용해 학습 포인트를 구분함
- 변수/상수/연산/입력 흐름을 한 파일에서 실습해보기 좋음

### 개선점
1. **파일명 오타/의미성 개선**
   - 현재: `Helloworl.c`
   - 권장: `hello_world.c` 또는 `HelloWorld.c`

2. **`main` 함수 과밀화**
   - 출력, 계산, 입력 실습 코드가 한 함수에 모두 존재
   - 학습 진도상 다음 단계에서 기능별 함수 분리 권장

3. **오탈자 정리**
   - 주석 `연산 에제` → `연산 예제`

---

## 4) 우선순위 액션 아이템

- [ ] `main` 시그니처를 `int main(void)`로 변경
- [ ] `scanf` 반환값 검사 추가
- [ ] 파일명 정리(`Helloworl.c` → `hello_world.c`)
- [ ] 입출력/연산 예제를 함수로 1차 분리

---

## 5) 학습자용 개선 예시(최소 안전 버전)

```c
#include <stdio.h>

int main(void) {
    int num = 0;

    printf("숫자를 입력하세요: ");
    if (scanf("%d", &num) != 1) {
        fprintf(stderr, "입력 오류\n");
        return 1;
    }

    printf("입력한 숫자는 %d입니다.\n", num);
    return 0;
}
```

---

## 총평
기초 문법 학습용 코드로는 매우 적절합니다. 다음 단계는 **입력 안전성 강화**와 **함수 분리 습관**입니다. 이 두 가지를 잡으면 이후 포인터/문자열/파일 I/O로 넘어갈 때 코드 품질이 크게 좋아집니다.
