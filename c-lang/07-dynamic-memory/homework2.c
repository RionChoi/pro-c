#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n;
    printf("Enter number of elements: ");
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "Input error\n");
        return 1;
    }
    if (n <= 0) {
        fprintf(stderr, "Number must be positive\n");
        return 1;
    }
    int *arr = malloc(n * sizeof(int));
    if (!arr) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    // Initialize array
    for (int i = 0; i < n; ++i) arr[i] = i + 1;
    // Increase size by 5 using realloc
    int extra = 5;
    int *new_arr = realloc(arr, (n + extra) * sizeof(int));
    if (!new_arr) {
        fprintf(stderr, "Reallocation failed\n");
        free(arr);
        return 1;
    }
    arr = new_arr;
    for (int i = n; i < n + extra; ++i) arr[i] = (i + 1) * 2;
    // Compute sum
    long sum = 0;
    for (int i = 0; i < n + extra; ++i) sum += arr[i];
    printf("Sum = %ld\n", sum);
    free(arr);
    return 0;
}
