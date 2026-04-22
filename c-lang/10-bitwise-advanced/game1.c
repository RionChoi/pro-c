#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TURNS 6

int main(void) {
    int hint = 0;
    int guess = 0;

    srand((unsigned int)time(NULL));
    const int secret = (rand() % 32) + 1; /* 1~32 */

    printf("=== XOR 힌트 숫자 게임 ===\n");
    printf("1~32 사이 정수를 맞혀보세요. 총 %d번 기회가 있습니다.\n", MAX_TURNS);

    for (int turn = 1; turn <= MAX_TURNS; ++turn) {
        hint = secret ^ turn;
        printf("[턴 %d] 힌트(secret XOR %d) = %d\n", turn, turn, hint);
        printf("추측 입력: ");

        if (scanf("%d", &guess) != 1) {
            fprintf(stderr, "입력 오류: 정수를 입력하세요.\n");
            return 1;
        }

        if (guess == secret) {
            printf("정답! 비밀 숫자는 %d였습니다.\n", secret);
            return 0;
        }

        if (guess < secret) {
            printf("더 큰 수입니다.\n");
        } else {
            printf("더 작은 수입니다.\n");
        }
    }

    printf("실패! 정답은 %d였습니다.\n", secret);
    return 0;
}
