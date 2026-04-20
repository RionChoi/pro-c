#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n;
    printf("Enter number of elements: ");
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "입력 오류\n");
        return 1;
    }
    if (n <= 0) {
        fprintf(stderr, "양수 값을 입력하세요\n");
        return 1;
    }
    int *arr = malloc(n * sizeof(int));
    if (!arr) {
        fprintf(stderr, "메모리 할당 실패\n");
        return 1;
    }
    for (int i = 0; i < n; ++i) {
        arr[i] = i + 1;
    }
    long sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += arr[i];
    }
    printf("Sum = %ld\n", sum);
    free(arr);
    return 0;
}
