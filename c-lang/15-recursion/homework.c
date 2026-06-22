#include <stdio.h>
#include <stdlib.h>

/* 재귀 팩토리얼 */
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

/* 재귀 피보나치 */
int fibonacci(int n) {
    if (n == 0) {
        return 0;
    }
    if (n == 1) {
        return 1;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

/* 반복 피보나치 (비교용) */
int fibonacci_iter(int n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    int a = 0, b = 1, c = 0;
    for (int i = 2; i <= n; i++) {
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}

int main(void) {
    int n;

    printf("양의 정수를 입력하세요 (0 <= n <= 20): ");
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "입력 오류\n");
        return 1;
    }

    if (n < 0 || n > 20) {
        fprintf(stderr, "0부터 20 사이의 값을 입력하세요.\n");
        return 1;
    }

    printf("\n=== 팩토리얼 ===\n");
    printf("factorial(%d) = %d\n", n, factorial(n));

    printf("\n=== 피보나치 ===\n");
    printf("fibonacci(%d) [재귀] = %d\n", n, fibonacci(n));
    printf("fibonacci(%d) [반복] = %d\n", n, fibonacci_iter(n));

    printf("\n=== 피보나치 수열 (0 ~ %d) ===\n", n < 15 ? n : 15);
    int limit = n < 15 ? n : 15;
    for (int i = 0; i <= limit; i++) {
        printf("%d ", fibonacci(i));
    }
    printf("\n");

    return 0;
}
