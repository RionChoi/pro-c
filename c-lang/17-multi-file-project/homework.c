/*
 * homework.c — 계산기 모듈 (멀티파일 프로젝트)
 *
 * 분할 컴파일:
 *   gcc -c calc.c -o calc.o
 *   gcc -c main.c -o main.o   (본 파일을 main.c로 컴파일)
 *   gcc calc.o main.o -o calc
 *
 * 또는 한 줄로:
 *   gcc calc.c homework.c -o calc
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* ── extern 전역 변수 (calc.c에서 정의) ── */
extern unsigned int g_op_count;

/* ── 함수 선언 (calc.h 없이 extern 선언) ── */
extern int  calc_add(int a, int b);
extern int  calc_sub(int a, int b);
extern int  calc_mul(int a, int b);
extern int  calc_div(int a, int b, int *rem);
extern void calc_print_stats(void);

static void print_result(int a, int b, int result, int rem, char op, bool ok)
{
    if (!ok) {
        fprintf(stderr, "연산 오류\n");
        return;
    }
    if (op == '/' && rem != 0)
        printf("%d %c %d = %d (나머지 %d)\n", a, op, b, result, rem);
    else
        printf("%d %c %d = %d\n", a, op, b, result);
}

int main(void)
{
    int a, b, result, rem;
    char op;
    bool running = true;

    printf("=== 멀티파일 계산기 ===\n");
    printf("사용 가능 연산: + - * / (q: 종료)\n");

    while (running) {
        printf("\n수식을 입력하세요 (예: 10 + 3): ");
        if (scanf("%d %c", &a, &op) != 2) {
            fprintf(stderr, "입력 오류: 올바른 형식으로 입력하세요.\n");
            /* 잘못된 입력 버퍼 비우기 */
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }

        if (op == 'q' || op == 'Q') {
            running = false;
            continue;
        }

        if (scanf("%d", &b) != 1) {
            fprintf(stderr, "입력 오류: 두 번째 숫자를 입력하세요.\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            continue;
        }

        result = 0;
        rem    = 0;

        switch (op) {
        case '+': result = calc_add(a, b); print_result(a, b, result, 0, op, true);  break;
        case '-': result = calc_sub(a, b); print_result(a, b, result, 0, op, true);  break;
        case '*': result = calc_mul(a, b); print_result(a, b, result, 0, op, true);  break;
        case '/':
            if (b == 0) {
                fprintf(stderr, "오류: 0으로 나눌 수 없습니다.\n");
            } else {
                result = calc_div(a, b, &rem);
                print_result(a, b, result, rem, op, true);
            }
            break;
        default:
            fprintf(stderr, "지원하지 않는 연산자: %c\n", op);
        }
    }

    calc_print_stats();
    printf("계산기를 종료합니다.\n");
    return 0;
}
