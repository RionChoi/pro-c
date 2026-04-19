#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    int user, ai;
    srand((unsigned int)time(NULL));

    printf("[가위바위보] 1:가위 2:바위 3:보\n");
    printf("입력: ");

    if (scanf("%d", &user) != 1 || user < 1 || user > 3) {
        printf("입력 오류\n");
        return 1;
    }

    ai = (rand() % 3) + 1;
    printf("컴퓨터: %d\n", ai);

    if (user == ai) {
        printf("무승부\n");
    } else if ((user == 1 && ai == 3) || (user == 2 && ai == 1) || (user == 3 && ai == 2)) {
        printf("승리!\n");
    } else {
        printf("패배!\n");
    }

    return 0;
}
