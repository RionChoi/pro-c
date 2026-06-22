#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int roll_die(void) {
    return (rand() % 6) + 1;
}

int main(void) {
    srand((unsigned int)time(NULL));

    int player = roll_die();
    int computer = roll_die();

    printf("[주사위 게임]\n");
    printf("나: %d, 컴퓨터: %d\n", player, computer);

    if (player > computer) {
        printf("승리!\n");
    } else if (player < computer) {
        printf("패배!\n");
    } else {
        printf("무승부\n");
    }

    return 0;
}
