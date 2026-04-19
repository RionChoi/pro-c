/*
 * 세션 06 — 과제 1: 요일별 스케줄 관리 프로그램
 *
 * 기능:
 *   - enum으로 요일(SUN~SAT)을 정의
 *   - typedef로 구조체 별칭 사용
 *   - 요일별 일정을 등록/조회
 *   - 메뉴 기반 인터페이스
 */

#include <stdio.h>
#include <string.h>

#define TASK_SIZE     128
#define MAX_TASKS     5

typedef enum {
    SUN = 0, MON, TUE, WED, THU, FRI, SAT,
    DAY_COUNT
} Day;

typedef struct {
    char tasks[MAX_TASKS][TASK_SIZE];
    int  task_count;
} Schedule;

static Schedule weekly[DAY_COUNT];

/* 요일 이름을 반환한다 */
const char *day_name(Day d) {
    static const char *names[] = {
        "일요일", "월요일", "화요일", "수요일",
        "목요일", "금요일", "토요일"
    };
    if (d >= SUN && d < DAY_COUNT) {
        return names[d];
    }
    return "알 수 없음";
}

/* 요일 번호를 입력받는다 */
Day input_day(void) {
    int d;
    printf("  요일 선택 (0=일 1=월 2=화 3=수 4=목 5=금 6=토): ");
    if (scanf("%d", &d) != 1 || d < 0 || d >= DAY_COUNT) {
        fprintf(stderr, "  유효하지 않은 요일입니다.\n");
        while (getchar() != '\n');
        return DAY_COUNT; /* 에러 표시 */
    }
    while (getchar() != '\n');
    return (Day)d;
}

/* 일정을 추가한다 */
void add_task(void) {
    Day d = input_day();
    if (d == DAY_COUNT) return;

    Schedule *s = &weekly[d];
    if (s->task_count >= MAX_TASKS) {
        fprintf(stderr, "  %s 일정이 가득 찼습니다 (최대 %d개).\n",
                day_name(d), MAX_TASKS);
        return;
    }

    printf("  일정 내용: ");
    if (fgets(s->tasks[s->task_count], TASK_SIZE, stdin) == NULL) {
        fprintf(stderr, "  입력 오류\n");
        return;
    }
    s->tasks[s->task_count][strcspn(s->tasks[s->task_count], "\n")] = '\0';
    s->task_count++;
    printf("  → %s에 일정 등록 완료!\n", day_name(d));
}

/* 특정 요일의 일정을 조회한다 */
void view_day(void) {
    Day d = input_day();
    if (d == DAY_COUNT) return;

    const Schedule *s = &weekly[d];
    printf("\n  === %s 일정 ===\n", day_name(d));
    if (s->task_count == 0) {
        printf("  (일정 없음)\n");
        return;
    }
    for (int i = 0; i < s->task_count; i++) {
        printf("  %d. %s\n", i + 1, s->tasks[i]);
    }
}

/* 전체 주간 일정을 출력한다 */
void view_week(void) {
    printf("\n  ===== 주간 스케줄 =====\n");
    for (Day d = SUN; d < DAY_COUNT; d++) {
        const Schedule *s = &weekly[d];
        printf("\n  [%s]", day_name(d));
        if (s->task_count == 0) {
            printf(" — 일정 없음");
        } else {
            printf("\n");
            for (int i = 0; i < s->task_count; i++) {
                printf("    %d. %s\n", i + 1, s->tasks[i]);
            }
        }
    }
    printf("\n");
}

void print_menu(void) {
    printf("\n--- 스케줄 관리 ---\n");
    printf("  1. 일정 추가\n");
    printf("  2. 요일별 조회\n");
    printf("  3. 주간 전체 보기\n");
    printf("  4. 종료\n");
    printf("선택: ");
}

int main(void) {
    int choice;

    /* 초기화 */
    for (int i = 0; i < DAY_COUNT; i++) {
        weekly[i].task_count = 0;
    }

    printf("=== 요일별 스케줄 관리 ===\n");

    while (1) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "숫자를 입력하세요.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1: add_task();  break;
            case 2: view_day();  break;
            case 3: view_week(); break;
            case 4:
                printf("\n프로그램을 종료합니다.\n");
                return 0;
            default:
                fprintf(stderr, "1~4 사이의 번호를 선택하세요.\n");
                break;
        }
    }

    return 0;
}
