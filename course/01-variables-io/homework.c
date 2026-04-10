#include <stdio.h>

int main(void) {
    int a, b;

    printf("정수 2개를 입력하세요: ");
    if (scanf("%d %d", &a, &b) != 2) {
        printf("입력 오류\n");
        return 1;
    }

    printf("덧셈: %d + %d = %d\n", a, b, a + b);
    printf("뺄셈: %d - %d = %d\n", a, b, a - b);
    printf("곱셈: %d * %d = %d\n", a, b, a * b);

    if (b == 0) {
        printf("나눗셈: 0으로 나눌 수 없습니다.\n");
    } else {
        printf("나눗셈(몫): %d / %d = %d\n", a, b, a / b);
        printf("나눗셈(나머지): %d %% %d = %d\n", a, b, a % b);
    }

    return 0;
}
