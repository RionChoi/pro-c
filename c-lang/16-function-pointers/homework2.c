#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*CompareFunc)(const void *, const void *);

int compare_int(const void *a, const void *b) {
    int ai = *(const int *)a;
    int bi = *(const int *)b;
    if (ai < bi) return -1;
    if (ai > bi) return 1;
    return 0;
}

int compare_int_desc(const void *a, const void *b) {
    return compare_int(b, a);
}

int compare_str(const void *a, const void *b) {
    const char *sa = *(const char **)a;
    const char *sb = *(const char **)b;
    return strcmp(sa, sb);
}

int compare_str_len(const void *a, const void *b) {
    const char *sa = *(const char **)a;
    const char *sb = *(const char **)b;
    size_t la = strlen(sa);
    size_t lb = strlen(sb);
    if (la < lb) return -1;
    if (la > lb) return 1;
    return strcmp(sa, sb);
}

int compare_abs(const void *a, const void *b) {
    int ai = *(const int *)a;
    int bi = *(const int *)b;
    int abs_a = ai < 0 ? -ai : ai;
    int abs_b = bi < 0 ? -bi : bi;
    if (abs_a < abs_b) return -1;
    if (abs_a > abs_b) return 1;
    return 0;
}

void print_int_array(const char *label, const int *arr, size_t n) {
    printf("%s: ", label);
    for (size_t i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("\n");
}

void print_str_array(const char *label, char *arr[], size_t n) {
    printf("%s: ", label);
    for (size_t i = 0; i < n; i++) {
        printf("%s", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("\n");
}

int main(void) {
    /* 정수 배열 오름차순 정렬 */
    int numbers[] = { 42, 17, 99, 8, 23, 56, 1, 78, 34, 91 };
    size_t num_count = sizeof(numbers) / sizeof(numbers[0]);

    printf("=== qsort 콜백: 정수 배열 정렬 ===\n");
    print_int_array("정렬 전", numbers, num_count);
    qsort(numbers, num_count, sizeof(int), compare_int);
    print_int_array("오름차순", numbers, num_count);
    qsort(numbers, num_count, sizeof(int), compare_int_desc);
    print_int_array("내림차순", numbers, num_count);

    /* 문자열 배열: 사전순 */
    char *fruits[] = { "사과", "포도", "딸기", "복숭아", "키위", "바나나" };
    size_t fruit_count = sizeof(fruits) / sizeof(fruits[0]);

    printf("\n=== qsort 콜백: 문자열 배열 정렬 ===\n");
    print_str_array("정렬 전", fruits, fruit_count);
    qsort(fruits, fruit_count, sizeof(char *), compare_str);
    print_str_array("사전순", fruits, fruit_count);
    qsort(fruits, fruit_count, sizeof(char *), compare_str_len);
    print_str_array("길이순", fruits, fruit_count);

    /* 사용자 정의 정렬: 절대값 기준 */
    int abs_numbers[] = { -5, 3, -8, 1, -2, 7, -9, 0 };
    size_t abs_count = sizeof(abs_numbers) / sizeof(abs_numbers[0]);

    printf("\n=== qsort 콜백: 절대값 기준 정렬 ===\n");
    print_int_array("정렬 전", abs_numbers, abs_count);
    qsort(abs_numbers, abs_count, sizeof(int), compare_abs);
    print_int_array("절대값순", abs_numbers, abs_count);

    return 0;
}
