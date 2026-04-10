#include <stdio.h>

void input_scores(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("점수 %d 입력: ", i + 1);
        if (scanf("%d", &arr[i]) != 1) {
            arr[i] = -1;
        }
    }
}

int count_pass(const int arr[], int n) {
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (arr[i] >= 60) count++;
    }
    return count;
}

int main(void) {
    int scores[5];
    input_scores(scores, 5);

    for (int i = 0; i < 5; i++) {
        if (scores[i] < 0 || scores[i] > 100) {
            printf("입력 오류\n");
            return 1;
        }
    }

    printf("합격 인원: %d명\n", count_pass(scores, 5));
    return 0;
}
