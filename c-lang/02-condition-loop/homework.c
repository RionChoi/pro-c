#include <stdio.h>

int is_prime(int n) {
    if (n < 2) return 0;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(void) {
    int menu = -1;

    while (1) {
        printf("\n=== 메뉴 ===\n");
        printf("1) 구구단 출력\n");
        printf("2) 1~N 합\n");
        printf("3) 소수 판별\n");
        printf("0) 종료\n");
        printf("선택: ");

        if (scanf("%d", &menu) != 1) {
            printf("입력 오류\n");
            return 1;
        }

        if (menu == 0) {
            printf("종료합니다.\n");
            break;
        }

        if (menu == 1) {
            int dan;
            printf("단 입력: ");
            if (scanf("%d", &dan) != 1) {
                printf("입력 오류\n");
                return 1;
            }
            for (int i = 1; i <= 9; i++) {
                printf("%d x %d = %d\n", dan, i, dan * i);
            }
        } else if (menu == 2) {
            int n;
            printf("N 입력: ");
            if (scanf("%d", &n) != 1) {
                printf("입력 오류\n");
                return 1;
            }
            long long sum = 0;
            for (int i = 1; i <= n; i++) sum += i;
            printf("1~%d 합 = %lld\n", n, sum);
        } else if (menu == 3) {
            int x;
            printf("정수 입력: ");
            if (scanf("%d", &x) != 1) {
                printf("입력 오류\n");
                return 1;
            }
            printf("%d는 %s입니다.\n", x, is_prime(x) ? "소수" : "소수 아님");
        } else {
            printf("잘못된 메뉴 입력입니다.\n");
        }
    }

    return 0;
}
