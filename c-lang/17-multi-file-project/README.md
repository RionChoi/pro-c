# 세션 17: 멀티파일 프로젝트 (Multi-file Project)

## 학습 목표
- 헤더 파일(.h)과 소스 파일(.c)을 분리하여 모듈화된 코드를 작성한다.
- `extern` 키워드로 다른 파일의 전역 변수와 함수를 참조한다.
- 분할 컴파일(declared once, defined once, used many)를 이해한다.
- 커맨드라인에서 여러 소스 파일을 함께 컴파일한다.

## 핵심 개념

### 파일 구조
```
project/
├── calc.h        // 헤더: 선언만 (중복 정의 방지)
├── calc.c        // 소스: 실제 구현
├── main.c        // 소스: main() — 헤더 참조
```

### 헤더 가드 (Header Guard)
```c
#ifndef CALC_H
#define CALC_H
// ... declarations ...
#endif
```

### extern 선언
```c
// file1.c
int global_count = 0;

// file2.c
extern int global_count;  // file1.c의 전역 변수 참조
```

### 분할 컴파일
```bash
gcc -c calc.c    # calc.o 생성
gcc -c main.c    # main.o 생성
gcc calc.o main.o -o program
```

## 과제 구성
| 파일 | 내용 |
|------|------|
| `homework.c` | 계산기: add/sub/mul/div 함수 분리 + 모듈 전역 연산 횟수 카운터 |
| `homework2.c` | 학생 관리: 구조체 배열을 별도 모듈로 분리 (student.h / student.c / main.c) |
| `game1.c` | 텍스트 어드벤처: 각 방/아이템/이벤트를 독립 소스 파일로 분리 |

## 연습 문제
1.calculator 모듈을 만들어 四則연산 함수를 별도 파일로 분리한다.
2.학생 정보(학번, 이름, 성적)를 구조체로 정의하고 CRUD 함수를 별도 파일에 구현한다.
3.텍스트 어드벤처 게임을 방(rooms.c), 아이템(items.c), 메인(main.c)으로 분리한다.
