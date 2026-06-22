#include <stdio.h>

#define MAX_SCORE 100

#ifdef DEBUG
#define DBG_PRINT(fmt, ...) printf("[DEBUG] " fmt "\n", __VA_ARGS__)
#else
#define DBG_PRINT(fmt, ...)
#endif

int clamp_score(int score) {
    if (score < 0) {
        return 0;
    }
    if (score > MAX_SCORE) {
        return MAX_SCORE;
    }
    return score;
}

int main(void) {
    int raw_score = 0;

    printf("점수를 입력하세요 (정수): ");
    if (scanf("%d", &raw_score) != 1) {
        fprintf(stderr, "입력 오류: 정수를 입력해야 합니다.\n");
        return 1;
    }

    DBG_PRINT("raw_score=%d", raw_score);

    const int final_score = clamp_score(raw_score);
    printf("보정 점수: %d\n", final_score);

    return 0;
}
