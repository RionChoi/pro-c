/*
 * 세션 07 — 과제 2: realloc 기반 확장 가계부
 *
 * 기능:
 *   - 초기 항목 수만큼 지출 금액을 동적 할당해 입력
 *   - 추가 항목 수를 받아 realloc으로 배열 확장
 *   - 총지출/평균/최대 지출 출력
 */

#include <stdio.h>
#include <stdlib.h>

static int read_int(const char *prompt, int *out_value) {
    if (prompt != NULL) {
        printf("%s", prompt);
    }

    if (scanf("%d", out_value) != 1) {
        fprintf(stderr, "입력 오류: 정수를 입력하세요.\n");
        while (getchar() != '\n') {
            /* 입력 버퍼 비우기 */
        }
        return 0;
    }

    return 1;
}

static int read_amounts(int *arr, int start, int end) {
    for (int i = start; i < end; i++) {
        int amount;
        char prompt[64];
        (void)snprintf(prompt, sizeof(prompt), "%d번 지출 금액(원): ", i + 1);

        if (!read_int(prompt, &amount)) {
            return 0;
        }

        if (amount < 0) {
            fprintf(stderr, "금액은 0 이상이어야 합니다.\n");
            return 0;
        }

        arr[i] = amount;
    }

    return 1;
}

int main(void) {
    int count;

    if (!read_int("초기 지출 항목 수: ", &count)) {
        return 1;
    }

    if (count <= 0) {
        fprintf(stderr, "항목 수는 1 이상이어야 합니다.\n");
        return 1;
    }

    int *expenses = (int *)malloc((size_t)count * sizeof(int));
    if (expenses == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        return 1;
    }

    if (!read_amounts(expenses, 0, count)) {
        free(expenses);
        return 1;
    }

    int extra;
    if (!read_int("추가 지출 항목 수: ", &extra)) {
        free(expenses);
        return 1;
    }

    if (extra < 0) {
        fprintf(stderr, "추가 항목 수는 0 이상이어야 합니다.\n");
        free(expenses);
        return 1;
    }

    const int new_count = count + extra;
    if (extra > 0) {
        int *tmp = (int *)realloc(expenses, (size_t)new_count * sizeof(int));
        if (tmp == NULL) {
            fprintf(stderr, "메모리 재할당 실패\n");
            free(expenses);
            return 1;
        }

        expenses = tmp;

        if (!read_amounts(expenses, count, new_count)) {
            free(expenses);
            return 1;
        }
    }

    long long total = 0;
    int max_expense = expenses[0];

    for (int i = 0; i < new_count; i++) {
        total += (long long)expenses[i];
        if (expenses[i] > max_expense) {
            max_expense = expenses[i];
        }
    }

    const double average = (double)total / (double)new_count;

    printf("\n[가계부 요약]\n");
    printf("총 항목 수: %d\n", new_count);
    printf("총지출: %lld원\n", total);
    printf("평균 지출: %.2f원\n", average);
    printf("최대 지출: %d원\n", max_expense);

    free(expenses);
    return 0;
}
