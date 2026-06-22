#include <stdio.h>

#define SQUARE(x) ((x) * (x))
#define CUBE(x)   ((x) * (x) * (x))

int main(void) {
    int value = 0;

    printf("정수를 입력하세요: ");
    if (scanf("%d", &value) != 1) {
        fprintf(stderr, "입력 오류: 정수를 입력해야 합니다.\n");
        return 1;
    }

    printf("SQUARE(%d) = %d\n", value, SQUARE(value));
    printf("CUBE(%d) = %d\n", value, CUBE(value));

    return 0;
}
