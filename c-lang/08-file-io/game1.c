/*
 * 세션 08 — 게임: 기록 저장 숫자 맞히기
 *
 * 규칙:
 *   - 1~100 난수를 맞힐 때까지 계속 시도
 *   - 각 입력 숫자를 guess_history.txt에 저장
 *   - 시도 횟수를 best_score.txt에 최고 기록으로 저장
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INPUT_SIZE 64
#define MIN_NUMBER 1
#define MAX_NUMBER 100

static int load_best_score(const char *path, int *best_score) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        return 0;
    }

    if (fscanf(fp, "%d", best_score) != 1) {
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1;
}

static int save_best_score(const char *path, int score) {
    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        return 0;
    }

    (void)fprintf(fp, "%d\n", score);
    fclose(fp);
    return 1;
}

int main(void) {
    const char *history_file = "guess_history.txt";
    const char *best_file = "best_score.txt";
    FILE *history = fopen(history_file, "w");

    if (history == NULL) {
        fprintf(stderr, "기록 파일을 열 수 없습니다.\n");
        return 1;
    }

    srand((unsigned int)time(NULL));
    const int target = (rand() % MAX_NUMBER) + MIN_NUMBER;

    int attempts = 0;
    char input[INPUT_SIZE];

    printf("=== 숫자 맞히기 게임 ===\n");
    printf("%d~%d 사이 숫자를 맞혀보세요.\n", MIN_NUMBER, MAX_NUMBER);

    while (1) {
        long guess_long;
        char *end_ptr;

        printf("입력: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            fclose(history);
            fprintf(stderr, "입력 오류\n");
            return 1;
        }

        guess_long = strtol(input, &end_ptr, 10);
        if (end_ptr == input) {
            printf("정수를 입력하세요.\n");
            continue;
        }

        if (guess_long < MIN_NUMBER || guess_long > MAX_NUMBER) {
            printf("범위를 벗어났습니다. (%d~%d)\n", MIN_NUMBER, MAX_NUMBER);
            continue;
        }

        const int guess = (int)guess_long;
        attempts++;
        (void)fprintf(history, "%d\n", guess);

        if (guess < target) {
            printf("더 큰 수입니다.\n");
        } else if (guess > target) {
            printf("더 작은 수입니다.\n");
        } else {
            printf("정답! 시도 횟수: %d\n", attempts);
            break;
        }
    }

    fclose(history);

    int best_score = 0;
    if (load_best_score(best_file, &best_score)) {
        if (attempts < best_score) {
            if (!save_best_score(best_file, attempts)) {
                fprintf(stderr, "최고 기록 저장 실패\n");
                return 1;
            }
            printf("새 최고 기록입니다! (%d회)\n", attempts);
        } else {
            printf("현재 최고 기록: %d회\n", best_score);
        }
    } else {
        if (!save_best_score(best_file, attempts)) {
            fprintf(stderr, "최고 기록 저장 실패\n");
            return 1;
        }
        printf("첫 최고 기록 저장! (%d회)\n", attempts);
    }

    printf("기록 파일: %s\n", history_file);
    return 0;
}
