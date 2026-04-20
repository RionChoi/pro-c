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
        fprintf(stderr, "양수만 허용\n");
        return 1;
    }
    // Open file for writing
    FILE *fp = fopen("data.txt", "w");
    if (!fp) {
        fprintf(stderr, "파일 열기 실패\n");
        return 1;
    }
    // Write numbers 1..n to file
    for (int i = 1; i <= n; ++i) {
        fprintf(fp, "%d\n", i);
    }
    fclose(fp);
    // Open file for reading
    fp = fopen("data.txt", "r");
    if (!fp) {
        fprintf(stderr, "파일 읽기 실패\n");
        return 1;
    }
    long sum = 0;
    int value;
    while (fscanf(fp, "%d", &value) == 1) {
        sum += value;
    }
    fclose(fp);
    printf("Sum = %ld\n", sum);
    return 0;
}
