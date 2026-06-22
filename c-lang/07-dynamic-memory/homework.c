/*
 * 세션 07 — 과제 1: 동적 배열 점수 분석기
 *
 * 기능:
 *   - 학생 수를 입력받아 점수 배열을 동적 할당
 *   - 점수 입력 후 합계/평균/최대/최소 출력
 *   - 모든 입력/할당 오류 처리
 */

#include <stdio.h>
#include <stdlib.h>

#define MIN_SCORE 0
#define MAX_SCORE 100

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

int main(void) {
    int student_count;

    if (!read_int("학생 수를 입력하세요: ", &student_count)) {
        return 1;
    }

    if (student_count <= 0) {
        fprintf(stderr, "학생 수는 1 이상이어야 합니다.\n");
        return 1;
    }

    int *scores = (int *)malloc((size_t)student_count * sizeof(int));
    if (scores == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        return 1;
    }

    for (int i = 0; i < student_count; i++) {
        int score;
        char prompt[64];
        (void)snprintf(prompt, sizeof(prompt), "%d번 학생 점수(0~100): ", i + 1);

        if (!read_int(prompt, &score)) {
            free(scores);
            return 1;
        }

        if (score < MIN_SCORE || score > MAX_SCORE) {
            fprintf(stderr, "점수 범위 오류: 0~100 사이만 허용됩니다.\n");
            free(scores);
            return 1;
        }

        scores[i] = score;
    }

    int sum = 0;
    int min_score = scores[0];
    int max_score = scores[0];

    for (int i = 0; i < student_count; i++) {
        const int value = scores[i];
        sum += value;

        if (value < min_score) {
            min_score = value;
        }
        if (value > max_score) {
            max_score = value;
        }
    }

    const double average = (double)sum / (double)student_count;

    printf("\n[점수 분석 결과]\n");
    printf("합계: %d\n", sum);
    printf("평균: %.2f\n", average);
    printf("최소: %d\n", min_score);
    printf("최대: %d\n", max_score);

    free(scores);
    return 0;
}
