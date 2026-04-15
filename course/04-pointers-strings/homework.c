#include <stdio.h>
#include <string.h>

#define N 5
#define NAME_SIZE 64
#define MAX_RETRY 3

int read_scores(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        int ok = 0;
        for (int attempt = 0; attempt < MAX_RETRY; attempt++) {
            printf("점수 %d 입력 (0~100): ", i + 1);
            if (scanf("%d", &arr[i]) != 1) {
                fprintf(stderr, "입력 오류: 정수를 입력하세요. (%d/%d)\n",
                        attempt + 1, MAX_RETRY);
                /* 버퍼 정리 */
                int c;
                while ((c = getchar()) != '\n' && c != EOF)
                    ;
                continue;
            }
            if (arr[i] < 0 || arr[i] > 100) {
                fprintf(stderr, "범위 오류: 0~100 사이 값을 입력하세요. (%d/%d)\n",
                        attempt + 1, MAX_RETRY);
                continue;
            }
            ok = 1;
            break;
        }
        if (!ok) {
            fprintf(stderr, "입력 실패: 점수 %d를 %d회 시도 후 중단합니다.\n",
                    i + 1, MAX_RETRY);
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

int min_score(const int *arr, int n) {
    int minv = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] < minv) {
            minv = arr[i];
        }
    }
    return minv;
}

void sort_asc(int *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (arr[i] > arr[j]) {
                int tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
            }
        }
    }
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

    int sum  = sum_scores(scores, N);
    int maxv = max_score(scores, N);
    int minv = min_score(scores, N);
    double avg = (double)sum / N;

    sort_asc(scores, N);

    printf("\n[결과]\n");
    printf("이름: %s\n", name);
    printf("총합: %d\n", sum);
    printf("평균: %.2f\n", avg);
    printf("최고점: %d\n", maxv);
    printf("최저점: %d\n", minv);
    printf("오름차순 정렬: ");
    for (int i = 0; i < N; i++) {
        printf("%d", scores[i]);
        if (i < N - 1) printf(" ");
    }
    printf("\n");

    return 0;
}
