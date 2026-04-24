# 세션 18: 커맨드라인 인자 (Command-line Arguments)

## 학습 목표
- `int main(int argc, char *argv[])` 시그니처를 이해한다.
- `argc`(인자 개수)와 `argv`(인자 목록)를，用来处理外部输入。
- 표준 유틸리티风格的命令行 인터페이스(flags, options, positional args)를 구현한다.
- 환경 변수(`getenv`, `setenv`)를 읽는 방법을 이해한다.

## 핵심 개념

### argc / argv 구조
```c
int main(int argc, char *argv[])
{
    // argv[0] = 프로그램 이름
    // argv[1] = 첫 번째 인자
    // argv[argc] = NULL (널 종단)
}
```

### 옵션 파싱 예시
```c
while ((c = getopt(argc, argv, "hv:n:")) != -1) {
    switch (c) {
    case 'h': print_help(); break;
    case 'v': verbose = atoi(optarg); break;
    case 'n': count = atoi(optarg); break;
    }
}
```

### 환경 변수
```c
#include <stdlib.h>
char *path = getenv("PATH");
setenv("MY_VAR", "value", 1);
```

## 과제 구성
| 파일 | 내용 |
|------|------|
| `homework.c` | 파일 복사 유틸: `cp src dst`, 플래그 `-v`(상세 출력), `-i`(확인) |
| `homework2.c` | 단어 빈도 카운터: 파일 또는 stdin에서 단어 수를 세어 정렬 출력 |
| `game1.c` | 간단한 터미널 RPG: 커맨드라인에서 캐릭터 스탯과 아이템을 인자로 전달받아 전투 |

## 연습 문제
1. `wc` 유틸리티风格的 단어/줄/문자 수 카운터를 만든다.
2. `-n`, `-m` 플래그로 출력 형식을 지정한다.
3. 환경 변수에서 `HOME`을 읽어 기본 디렉터리로 사용한다.
