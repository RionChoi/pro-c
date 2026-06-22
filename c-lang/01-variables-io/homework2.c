#include <stdio.h>

int main(void) {
    double celsius;

    printf("섭씨 온도를 입력하세요: ");
    if (scanf("%lf", &celsius) != 1) {
        printf("입력 오류\n");
        return 1;
    }

    double fahrenheit = (celsius * 9.0 / 5.0) + 32.0;
    double kelvin = celsius + 273.15;

    printf("섭씨: %.2f°C\n", celsius);
    printf("화씨: %.2f°F\n", fahrenheit);
    printf("켈빈: %.2fK\n", kelvin);

    return 0;
}
