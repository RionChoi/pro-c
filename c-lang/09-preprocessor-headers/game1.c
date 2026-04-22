#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MIN_NUM 1
#define MAX_NUM 20
#define MAX_TRIES 5
#define BUF_SIZE 64

int main(void) {
    char line[BUF_SIZE];
    int guess = 0;
    int try_count = 0;

    srand((unsigned int)time(NULL));
    const int answer = (rand() % (MAX_NUM - MIN_NUM + 1)) + MIN_NUM;

    printf("=== 업다운 게임 ===\n");
    printf("%d부터 %d 사이 숫자를 맞혀보세요. 기회는 %d번입니다.\n", MIN_NUM, MAX_NUM, MAX_TRIES);

    while (try_count < MAX_TRIES) {
        printf("[%d/%d] 추측 입력: ", try_count + 1, MAX_TRIES);
        if (fgets(line, sizeof(line), stdin) == NULL) {
            fprintf(stderr, "입력 오류가 발생했습니다.\n");
            return 1;
        }

        if (sscanf(line, "%d", &guess) != 1) {
            fprintf(stderr, "숫자를 입력해야 합니다.\n");
            continue;
        }

        try_count++;

        if (guess == answer) {
            printf("정답입니다! %d번 만에 맞혔습니다.\n", try_count);
            return 0;
        }

        if (guess < answer) {
            printf("UP!\n");
        } else {
            printf("DOWN!\n");
        }
    }

    printf("실패! 정답은 %d였습니다.\n", answer);
    return 0;
}
