/*
 * 세션 12 - 과제 1: 정렬 알고리즘 비교
 * 버블 정렬, 선택 정렬, 삽입 정렬 구현 및 비교
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ARR_SIZE 10

void print_array(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void bubble_sort(int *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (arr[j] > arr[j + 1]) {
                int tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
}

void selection_sort(int *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        int tmp = arr[i];
        arr[i] = arr[min_idx];
        arr[min_idx] = tmp;
    }
}

void insertion_sort(int *arr, int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

double measure_sort(void (*sort_fn)(int *, int), int *arr, int n) {
    int *copy = (int *)malloc(sizeof(int) * n);
    if (copy == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        return -1.0;
    }
    memcpy(copy, arr, sizeof(int) * n);

    clock_t start = clock();
    sort_fn(copy, n);
    clock_t end = clock();

    double elapsed = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;
    free(copy);
    return elapsed;
}

int main(void) {
    int original[ARR_SIZE];
    srand((unsigned int)time(NULL));

    printf("=== 정렬 알고리즘 비교 ===\n");
    printf("원본 배열: ");
    for (int i = 0; i < ARR_SIZE; i++) {
        original[i] = rand() % 100;
        printf("%d ", original[i]);
    }
    printf("\n\n");

    double b_time = measure_sort(bubble_sort, original, ARR_SIZE);
    printf("버블 정렬 결과: ");
    print_array(original, ARR_SIZE);
    printf("소요 시간: %.3f ms\n\n", b_time);

    double s_time = measure_sort(selection_sort, original, ARR_SIZE);
    printf("선택 정렬 결과: ");
    print_array(original, ARR_SIZE);
    printf("소요 시간: %.3f ms\n\n", s_time);

    double i_time = measure_sort(insertion_sort, original, ARR_SIZE);
    printf("삽입 정렬 결과: ");
    print_array(original, ARR_SIZE);
    printf("소요 시간: %.3f ms\n\n", i_time);

    printf("=== 비교 요약 ===\n");
    printf("버블:   %.3f ms\n", b_time);
    printf("선택:   %.3f ms\n", s_time);
    printf("삽입:   %.3f ms\n", i_time);

    return 0;
}
