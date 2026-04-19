#include <stdio.h>

double average_score(const int scores[], int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) sum += scores[i];
    return (double)sum / n;
}

int max_score(const int scores[], int n) {
    int max = scores[0];
    for (int i = 1; i < n; i++) {
        if (scores[i] > max) max = scores[i];
    }
    return max;
}

char to_grade(int score) {
    if (score >= 90) return 'A';
    if (score >= 80) return 'B';
    if (score >= 70) return 'C';
    if (score >= 60) return 'D';
    return 'F';
}

#define STUDENT_COUNT 5

int main(void) {
    int scores[STUDENT_COUNT];

    printf("학생 5명의 점수를 입력하세요.\n");
    for (int i = 0; i < STUDENT_COUNT; i++) {
        printf("학생 %d 점수: ", i + 1);
        if (scanf("%d", &scores[i]) != 1) {
            printf("입력 오류\n");
            return 1;
        }
    }

    double avg = average_score(scores, STUDENT_COUNT);
    int max = max_score(scores, STUDENT_COUNT);

    printf("\n번호\t점수\t학점\n");
    printf("---------------------\n");
    for (int i = 0; i < STUDENT_COUNT; i++) {
        printf("%d\t%d\t%c\n", i + 1, scores[i], to_grade(scores[i]));
    }
    printf("---------------------\n");
    printf("평균: %.2f\n", avg);
    printf("최고점: %d\n", max);

    return 0;
}
