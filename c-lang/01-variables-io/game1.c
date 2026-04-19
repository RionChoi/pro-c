#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    srand((unsigned int)time(NULL));
    int answer = (rand() % 50) + 1;
    int guess, tries = 0;

    printf("[숫자 맞추기 게임] 1~50 사이 숫자를 맞혀보세요.\n");

    while (1) {
        printf("입력: ");
        if (scanf("%d", &guess) != 1) {
            printf("입력 오류\n");
            return 1;
        }

        tries++;

        if (guess < answer) {
            printf("UP\n");
        } else if (guess > answer) {
            printf("DOWN\n");
        } else {
            printf("정답! 시도 횟수: %d\n", tries);
            break;
        }
    }

    return 0;
}
