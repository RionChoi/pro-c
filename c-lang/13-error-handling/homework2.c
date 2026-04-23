// homework.c — 파일 계산기: 다양한 에러 상황을 처리
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define FILENAME "calc_data.txt"

static void trim_newline(char *s) {
    s[strcspn(s, "\n")] = '\0';
}

static int parse_double(const char *token, double *out) {
    char *end;
    errno = 0;
    *out = strtod(token, &end);
    if (errno == ERANGE) {
        fprintf(stderr, "오버플로우 발생: %s\n", token);
        return -1;
    }
    if (*end != '\0') {
        fprintf(stderr, "숫자 아님: %s\n", token);
        return -1;
    }
    return 0;
}

static double calculate(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/':
            if (b == 0.0) {
                errno = EDOM;
                fprintf(stderr, "나눗셈 오류: 0으로 나눌 수 없습니다.\n");
                return 0.0;
            }
            return a / b;
        default:
            errno = EINVAL;
            fprintf(stderr, "알 수 없는 연산자: %c\n", op);
            return 0.0;
    }
}

int main(void) {
    FILE *fp = fopen(FILENAME, "r");
    if (!fp) {
        fprintf(stderr, "파일 열기 오류 (%s): %s\n", FILENAME, strerror(errno));
        fprintf(stderr, "파일이 없습니다. 기본 예제를 실행합니다.\n");
        // 직접 입력 모드로 전환
        char line[256];
        printf("형식: 숫자 연산자 숫자 (예: 10 + 5)\n");
        printf("종료: q\n");
        while (1) {
            printf("> ");
            if (!fgets(line, sizeof(line), stdin)) break;
            trim_newline(line);
            if (strcmp(line, "q") == 0) break;
            if (strlen(line) == 0) continue;

            double a, b; char op;
            int cnt = sscanf(line, "%lf %c %lf", &a, &op, &b);
            if (cnt != 3) {
                fprintf(stderr, "입력 오류: 형식을 확인하세요.\n");
                continue;
            }

            errno = 0;
            double result = calculate(a, b, op);
            if (errno == 0) {
                printf("결과: %.6g\n", result);
            }
        }
        return 0;
    }

    char line[256];
    int line_no = 0;
    while (fgets(line, sizeof(line), fp)) {
        line_no++;
        trim_newline(line);
        if (strlen(line) == 0) continue;

        double a, b; char op;
        int cnt = sscanf(line, "%lf %c %lf", &a, &op, &b);
        if (cnt != 3) {
            fprintf(stderr, "줄 %d: 잘못된 형식 — '%s'\n", line_no, line);
            continue;
        }

        errno = 0;
        double result = calculate(a, b, op);
        if (errno == 0) {
            printf("줄 %d: %.6g %c %.6g = %.6g\n", line_no, a, op, b, result);
        }
    }

    if (ferror(fp)) {
        fprintf(stderr, "파일 읽기 오류: %s\n", strerror(errno));
        fclose(fp);
        return 1;
    }

    fclose(fp);
    printf("계산 완료.\n");
    return 0;
}
