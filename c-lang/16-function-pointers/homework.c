#include <stdio.h>
#include <stdlib.h>

typedef int (*BinaryOp)(int, int);

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }

double div_op(int a, int b) {
    if (b == 0) {
        fprintf(stderr, "0으로 나눌 수 없습니다.\n");
        return 0.0;
    }
    return (double)a / b;
}

int mod_op(int a, int b) {
    if (b == 0) {
        fprintf(stderr, "0으로 나머지 연산 불가.\n");
        return 0;
    }
    return a % b;
}

void print_menu(void) {
    printf("\n=== 연산 선택 ===\n");
    printf("1: 더하기 (+)\n");
    printf("2: 빼기 (-)\n");
    printf("3: 곱하기 (*)\n");
    printf("4: 나누기 (/)\n");
    printf("5: 나머지 (%%)\n");
    printf("0: 종료\n");
    printf("선택: ");
}

int main(void) {
    BinaryOp ops[5] = { add, sub, mul, NULL, NULL };
    int choice;
    int a, b;

    printf("=================================\n");
    printf("     함수 포인터 계산기\n");
    printf("=================================\n");

    while (1) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "입력 오류\n");
            return 1;
        }

        if (choice == 0) {
            printf("계산기를 종료합니다.\n");
            break;
        }

        if (choice < 1 || choice > 5) {
            printf("1~5 사이의 값을 입력하세요.\n");
            continue;
        }

        printf("두 정수를 입력하세요: ");
        if (scanf("%d %d", &a, &b) != 2) {
            fprintf(stderr, "입력 오류\n");
            return 1;
        }

        if (choice == 4) {
            double result = div_op(a, b);
            printf("%d / %d = %.6f\n", a, b, result);
        } else if (choice == 5) {
            int result = mod_op(a, b);
            printf("%d %% %d = %d\n", a, b, result);
        } else {
            int result = ops[choice - 1](a, b);
            printf("%d %c %d = %d\n", a, "+-*"[choice - 1], b, result);
        }
    }

    return 0;
}
