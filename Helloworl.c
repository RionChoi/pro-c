#include <stdio.h>

int main(void) {
    // 프로그램 시작
    printf("Hello, World!\n");
    // 정수형 변수 예제
    int age = 12;
    printf("나이는 %d살입니다.\n", age);
    age += 1;
    printf("내년에는 %d살입니다.\n", age);
    /* 프로그램 종료 */
    // 실수형 변수 예제
    double height = 175.5;
    printf("키는 %.1fcm입니다.\n", height);
    // 상수형 변수 예제
    const double PI = 3.14159;
    printf("원주율은 %.5f입니다.\n", PI);

    // 연산 예제
    int a = 10;
    int b = 3;
    printf("덧셈: %d + %d = %d\n", a, b, a + b);
    printf("뺄셈: %d - %d = %d\n", a, b, a - b);
    printf("곱셈: %d * %d = %d\n", a, b, a * b);
    printf("나눗셈: %d / %d = %d\n",    a, b, a / b);
    // scanf 예제
    int num = 0;
    printf("숫자를 입력하세요: ");
    if (scanf("%d", &num) != 1) {
        fprintf(stderr, "입력 오류\n");
        return 1;
    }
    printf("입력한 숫자는 %d입니다.\n", num);
    return 0;
    
}