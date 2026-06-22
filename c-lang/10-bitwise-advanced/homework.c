#include <stdio.h>

void print_binary(unsigned int n) {
    for (int i = 7; i >= 0; --i) {
        const unsigned int bit = (n >> (unsigned int)i) & 1U;
        printf("%u", bit);
    }
}

int main(void) {
    unsigned int a = 0U;
    unsigned int b = 0U;

    printf("두 개의 0~255 정수를 입력하세요: ");
    if (scanf("%u %u", &a, &b) != 2) {
        fprintf(stderr, "입력 오류: 두 개의 정수를 입력해야 합니다.\n");
        return 1;
    }

    if (a > 255U || b > 255U) {
        fprintf(stderr, "범위 오류: 0~255 사이만 허용됩니다.\n");
        return 1;
    }

    printf("a      = ");
    print_binary(a);
    printf("\n");

    printf("b      = ");
    print_binary(b);
    printf("\n");

    printf("a & b  = ");
    print_binary(a & b);
    printf("\n");

    printf("a | b  = ");
    print_binary(a | b);
    printf("\n");

    printf("a ^ b  = ");
    print_binary(a ^ b);
    printf("\n");

    return 0;
}
