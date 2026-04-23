// game1.c — 숫자 맞추기 게임: 에러 처리 적용
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define MIN 1
#define MAX 100
#define MAX_TRIES 7

static void trim_newline(char *s) {
    s[strcspn(s, "\n")] = '\0';
}

int main(void) {
    srand((unsigned int)time(NULL));

    printf("=================================\n");
    printf("  숫자 맞추기 게임 (1~100)\n");
    printf("=================================\n");
    printf("1부터 100 사이의 숫자를 맞추세요.\n");
    printf("최대 %d번 시도 가능.\n\n", MAX_TRIES);

    int secret = (rand() % (MAX - MIN + 1)) + MIN;
    int attempt = 0;
    int guess;

    while (attempt < MAX_TRIES) {
        printf("시도 %d/%d — 숫자 입력> ", attempt + 1, MAX_TRIES);

        char buf[64];
        if (!fgets(buf, sizeof(buf), stdin)) {
            fprintf(stderr, "입력 오류. 다시 시도하세요.\n");
            continue;
        }
        trim_newline(buf);

        if (strlen(buf) == 0) {
            fprintf(stderr, "입력 오류: 빈 문자열.\n");
            continue;
        }

        char *end;
        errno = 0;
        long val = strtol(buf, &end, 10);
        if (errno == ERANGE) {
            fprintf(stderr, "오류: 너무 큰 숫자입니다.\n");
            continue;
        }
        if (*end != '\0' || *buf == '\0') {
            fprintf(stderr, "오류: 정수를 입력하세요.\n");
            continue;
        }
        if (val < MIN || val > MAX) {
            fprintf(stderr, "오류: %d~%d 사이의 숫자를 입력하세요.\n", MIN, MAX);
            continue;
        }

        guess = (int)val;
        attempt++;

        if (guess == secret) {
            printf("정답! %d번 만에 맞추셨습니다.\n", attempt);
            printf("결과: PASS\n");
            return 0;
        } else if (guess < secret) {
            printf("UP! 더 큰 숫자입니다.\n\n");
        } else {
            printf("DOWN! 더 작은 숫자입니다.\n\n");
        }
    }

    printf("시도 횟수 초과! 정답은 %d였습니다.\n", secret);
    printf("결과: FAIL\n");
    return 0;
}
