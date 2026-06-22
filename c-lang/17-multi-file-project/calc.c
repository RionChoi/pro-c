/*
 * calc.c — 계산기 함수 정의 (멀티파일 프로젝트)
 *
 * 이 파일은 homework.c(main)와 함께 분할 컴파일된다.
 */

#include <stdio.h>

/* ── 모듈 전역 변수 (외부 참조 불가) ── */
static unsigned int g_op_count = 0;

/* ── extern: 다른 파일에서 정의된 전역 변수 ── */
extern unsigned int g_op_count;   /* homework.c의 전역 변수를 참조 */

int calc_add(int a, int b)
{
    g_op_count++;
    return a + b;
}

int calc_sub(int a, int b)
{
    g_op_count++;
    return a - b;
}

int calc_mul(int a, int b)
{
    g_op_count++;
    return a * b;
}

int calc_div(int a, int b, int *rem)
{
    g_op_count++;
    if (rem != NULL)
        *rem = a % b;
    return a / b;
}

void calc_print_stats(void)
{
    printf("\n[통계] 총 연산 횟수: %u\n", g_op_count);
}
