#include <stdio.h>
#include <string.h>

/* 재귀적 문자열 뒤집기 */
void reverse_string(char *s, int left, int right) {
    if (left >= right) {
        return;
    }
    char tmp = s[left];
    s[left] = s[right];
    s[right] = tmp;
    reverse_string(s, left + 1, right - 1);
}

/* 재귀적 배열 합산 */
int sum_array(const int *arr, int n) {
    if (n <= 0) {
        return 0;
    }
    return arr[0] + sum_array(arr + 1, n - 1);
}

/* 유클리드 호제법 (재귀 버전) - 최대공약수 */
int gcd(int a, int b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

/* 재귀적 펠린드롬 판별 */
int is_palindrome(const char *s, int left, int right) {
    if (left >= right) {
        return 1;
    }
    if (s[left] != s[right]) {
        return 0;
    }
    return is_palindrome(s, left + 1, right - 1);
}

int main(void) {
    char input[256];

    /* 과제 1: 문자열 뒤집기 */
    printf("=== 문자열 뒤집기 ===\n");
    printf("문자열을 입력하세요: ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        fprintf(stderr, "입력 오류\n");
        return 1;
    }
    input[strcspn(input, "\n")] = '\0';

    int len = (int)strlen(input);
    reverse_string(input, 0, len - 1);
    printf("뒤집은 문자열: %s\n", input);

    /* 과제 2: 배열 합산 */
    printf("\n=== 배열 합산 (재귀) ===\n");
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int arr_size = (int)(sizeof(arr) / sizeof(arr[0]));
    printf("배열: ");
    for (int i = 0; i < arr_size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    printf("합계: %d\n", sum_array(arr, arr_size));

    /* 과제 3: 최대공약수 */
    printf("\n=== 최대공약수 (유클리드 호제법) ===\n");
    int a = 48, b = 18;
    printf("gcd(%d, %d) = %d\n", a, b, gcd(a, b));
    a = 100; b = 35;
    printf("gcd(%d, %d) = %d\n", a, b, gcd(a, b));
    a = 17; b = 13;
    printf("gcd(%d, %d) = %d\n", a, b, gcd(a, b));

    /* 과제 4: 펠린드롬 판별 */
    printf("\n=== 펠린드롬 판별 ===\n");
    const char *words[] = {"level", "radar", "hello", "rotor", "civic"};
    for (size_t i = 0; i < sizeof(words) / sizeof(words[0]); i++) {
        int result = is_palindrome(words[i], 0, (int)strlen(words[i]) - 1);
        printf("%s: %s\n", words[i], result ? "펠린드롬 O" : "펠린드롬 X");
    }

    return 0;
}
