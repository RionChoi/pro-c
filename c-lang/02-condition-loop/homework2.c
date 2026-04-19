#include <stdio.h>

int main(void) {
    int n;
    printf("별 피라미드 높이 입력(1~20): ");

    if (scanf("%d", &n) != 1 || n < 1 || n > 20) {
        printf("입력 오류\n");
        return 1;
    }

    for (int i = 1; i <= n; i++) {
        for (int s = 0; s < n - i; s++) printf(" ");
        for (int k = 0; k < (2 * i - 1); k++) printf("*");
        printf("\n");
    }

    return 0;
}
