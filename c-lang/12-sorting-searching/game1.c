/*
 * 세션 12 - 미니 게임: 숫자 야구 게임 (Number Baseball)
 * 컴퓨터가 생성한 3자리 숫자를 맞히는 게임
 * 정답의 자리와 숫자가 모두 맞으면 스트라이크,
 * 숫자만 맞으면 볼, 아무것도 아니면 아웃
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void generate_secret(int *digits) {
    int nums[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (int i = 9; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = nums[i];
        nums[i] = nums[j];
        nums[j] = tmp;
    }
    digits[0] = nums[0];
    digits[1] = nums[1];
    digits[2] = nums[2];
}

int main(void) {
    srand((unsigned int)time(NULL));
    int secret[3];
    generate_secret(secret);

    printf("=== 숫자 야구 게임 ===\n");
    printf("세 자리 숫자를 맞춰보세요!\n");
    printf("각 자리의 숫자와 위치가 맞으면 스트라이크,\n");
    printf("숫자만 맞으면 볼, 아무것도 아니면 아웃입니다.\n\n");

    int attempt = 0;
    while (1) {
        attempt++;
        printf("시도 %d - 세 자리 숫자 입력: ", attempt);
        char input[16];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            fprintf(stderr, "입력 오류\n");
            continue;
        }
        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) != 3) {
            printf("세 자리를 입력하세요.\n\n");
            continue;
        }

        int guess[3];
        int valid = 1;
        for (int i = 0; i < 3; i++) {
            if (input[i] < '0' || input[i] > '9') {
                valid = 0;
                break;
            }
            guess[i] = input[i] - '0';
        }
        if (!valid) {
            printf("숫자 세 자리를 입력하세요.\n\n");
            continue;
        }

        int strike = 0, ball = 0;
        for (int i = 0; i < 3; i++) {
            if (guess[i] == secret[i]) {
                strike++;
            } else {
                for (int j = 0; j < 3; j++) {
                    if (guess[i] == secret[j]) {
                        ball++;
                        break;
                    }
                }
            }
        }

        if (strike == 3) {
            printf("\n정답입니다! %d번 만에 맞혔습니다!\n", attempt);
            printf("정답: %d%d%d\n", secret[0], secret[1], secret[2]);
            break;
        } else if (strike == 0 && ball == 0) {
            printf("아웃!\n\n");
        } else {
            printf("%d 스트라이크 %d 볼\n\n", strike, ball);
        }

        if (attempt >= 9) {
            printf("9번 기회 모두 사용. 정답: %d%d%d\n",
                   secret[0], secret[1], secret[2]);
            break;
        }
    }

    return 0;
}
