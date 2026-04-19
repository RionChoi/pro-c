#include <stdio.h>
#include <string.h>

#define N 5
#define NAME_SIZE 64

int read_scores(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        printf("점수 %d 입력: ", i + 1);
        if (scanf("%d", &arr[i]) != 1) {
            fprintf(stderr, "입력 오류: 정수를 입력하세요.\n");
            return 0;
        }
        if (arr[i] < 0 || arr[i] > 100) {
            fprintf(stderr, "입력 오류: 점수는 0~100 범위여야 합니다.\n");
            return 0;
        }
    }
    return 1;
}

int sum_scores(const int *arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

int max_score(const int *arr, int n) {
    int maxv = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > maxv) {
            maxv = arr[i];
        }
    }
    return maxv;
}

int main(void) {
    int scores[N] = {0};
    char name[NAME_SIZE];

    printf("이름을 입력하세요: ");
    if (fgets(name, sizeof(name), stdin) == NULL) {
        fprintf(stderr, "입력 오류: 이름을 읽지 못했습니다.\n");
        return 1;
    }

    name[strcspn(name, "\n")] = '\0';

    if (!read_scores(scores, N)) {
        return 1;
    }

    int sum = sum_scores(scores, N);
    int maxv = max_score(scores, N);
    double avg = (double)sum / N;

    printf("\n[결과]\n");
    printf("이름: %s\n", name);
    printf("총합: %d\n", sum);
    printf("평균: %.2f\n", avg);
    printf("최고점: %d\n", maxv);

    return 0;
}
