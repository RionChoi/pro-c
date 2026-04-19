#include <stdio.h>
#include <string.h>

int main(void) {
    char answer[32] = "cprogramming";
    char input[32];
    int chances = 3;

    printf("[문자열 맞추기 게임] 기회 3번\n");

    while (chances > 0) {
        printf("단어 입력: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("입력 오류\n");
            return 1;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, answer) == 0) {
            printf("정답!\n");
            return 0;
        }

        chances--;
        printf("틀렸습니다. 남은 기회: %d\n", chances);
    }

    printf("실패! 정답은 %s\n", answer);
    return 0;
}
