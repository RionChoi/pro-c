/*
 * 세션 05 — 과제 1: 학생 성적 구조체 관리
 *
 * 기능:
 *   - 학생 정보(이름, 나이, 점수 3개)를 구조체로 관리
 *   - 최대 5명의 학생 입력
 *   - 학생별 평균 점수, 전체 최고 점수 학생 출력
 */

#include <stdio.h>
#include <string.h>

#define MAX_STUDENTS 5
#define NUM_SCORES   3
#define NAME_SIZE    64

struct Student {
    char name[NAME_SIZE];
    int  age;
    int  scores[NUM_SCORES];
};

/* 학생 한 명의 평균 점수를 계산한다 */
double calc_average(const struct Student *s) {
    int sum = 0;
    for (int i = 0; i < NUM_SCORES; i++) {
        sum += s->scores[i];
    }
    return (double)sum / NUM_SCORES;
}

/* 학생 정보를 출력한다 */
void print_student(const struct Student *s, int index) {
    double avg = calc_average(s);
    printf("\n--- 학생 %d ---\n", index + 1);
    printf("  이름: %s\n", s->name);
    printf("  나이: %d세\n", s->age);
    printf("  점수: ");
    for (int i = 0; i < NUM_SCORES; i++) {
        printf("%d", s->scores[i]);
        if (i < NUM_SCORES - 1) {
            printf(", ");
        }
    }
    printf("\n");
    printf("  평균: %.1f\n", avg);
}

/* 가장 높은 평균 점수를 가진 학생 인덱스를 반환한다 */
int find_top_student(const struct Student students[], int count) {
    int top_index = 0;
    double top_avg = calc_average(&students[0]);

    for (int i = 1; i < count; i++) {
        double avg = calc_average(&students[i]);
        if (avg > top_avg) {
            top_avg = avg;
            top_index = i;
        }
    }
    return top_index;
}

int main(void) {
    struct Student students[MAX_STUDENTS];
    int count = 0;

    printf("=== 학생 성적 관리 시스템 ===\n");
    printf("학생 수를 입력하세요 (1~%d): ", MAX_STUDENTS);
    if (scanf("%d", &count) != 1 || count < 1 || count > MAX_STUDENTS) {
        fprintf(stderr, "입력 오류: 1~%d 범위의 정수를 입력하세요.\n", MAX_STUDENTS);
        return 1;
    }

    /* 입력 버퍼 정리 */
    while (getchar() != '\n');

    for (int i = 0; i < count; i++) {
        printf("\n[학생 %d 입력]\n", i + 1);

        printf("  이름: ");
        if (fgets(students[i].name, NAME_SIZE, stdin) == NULL) {
            fprintf(stderr, "이름 입력 오류\n");
            return 1;
        }
        students[i].name[strcspn(students[i].name, "\n")] = '\0';

        printf("  나이: ");
        if (scanf("%d", &students[i].age) != 1 || students[i].age < 1) {
            fprintf(stderr, "나이 입력 오류\n");
            return 1;
        }

        for (int j = 0; j < NUM_SCORES; j++) {
            printf("  점수 %d (0~100): ", j + 1);
            if (scanf("%d", &students[i].scores[j]) != 1 ||
                students[i].scores[j] < 0 || students[i].scores[j] > 100) {
                fprintf(stderr, "점수 입력 오류: 0~100 범위를 입력하세요.\n");
                return 1;
            }
        }

        /* 다음 fgets를 위한 버퍼 정리 */
        while (getchar() != '\n');
    }

    /* 전체 학생 출력 */
    printf("\n========== 성적표 ==========\n");
    for (int i = 0; i < count; i++) {
        print_student(&students[i], i);
    }

    /* 최고 점수 학생 */
    int top = find_top_student(students, count);
    printf("\n★ 최고 평균 학생: %s (평균 %.1f점)\n",
           students[top].name, calc_average(&students[top]));

    return 0;
}
