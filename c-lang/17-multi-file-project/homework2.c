/*
 * homework2.c — 학생 관리 프로그램 (멀티파일 프로젝트)
 *
 * 파일 구성:
 *   student.h  — 구조체 선언 및 함수 프로토타입
 *   student.c  — CRUD 함수 구현
 *   main.c     — 메인 로직 (본 파일)
 *
 * 분할 컴파일:
 *   gcc -c student.c -o student.o
 *   gcc -c homework2.c -o main.o   (본 파일을 main.o로)
 *   gcc student.o main.o -o student_mgr
 *
 * 또는 한 줄로:
 *   gcc student.c homework2.c -o student_mgr
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "student.h"

#define MAX_NAME  64
#define MAX_STUDENTS 100

/* ── 学生 배열과 CRUD 함수 (student.c에서 정의) ── */
extern void   student_init(void);
extern bool   student_add(const char *name, int score);
extern bool   student_remove(int id);
extern void   student_print_all(void);
extern bool   student_update_score(int id, int score);
extern int    student_count(void);

/* ── 输入 유틸 ── */
static int  read_int(const char *prompt);
static void read_str(const char *prompt, char *buf, size_t len);

int main(void)
{
    student_init();

    printf("=== 학생 관리 프로그램 (멀티파일) ===\n");
    printf("1. 학생 추가  2. 학생 삭제  3. 성적 수정\n");
    printf("4. 전체 목록  5. 학생 수  0. 종료\n");

    int choice;
    bool running = true;

    while (running) {
        choice = read_int("선택: ");
        switch (choice) {
        case 1: {
            char name[MAX_NAME];
            int score = read_int("이름: ");
            read_str("이름: ", name, MAX_NAME);
            score = read_int("성적: ");
            if (student_add(name, score))
                printf("추가 완료 (ID: %d)\n", student_count());
            else
                fprintf(stderr, "오류: 최대 학생 수 초과\n");
            break;
        }
        case 2: {
            int id = read_int("삭제할 학생 ID: ");
            if (student_remove(id))
                printf("삭제 완료\n");
            else
                fprintf(stderr, "오류: 존재하지 않는 ID\n");
            break;
        }
        case 3: {
            int id    = read_int("수정할 학생 ID: ");
            int score = read_int("새 성적: ");
            if (student_update_score(id, score))
                printf("수정 완료\n");
            else
                fprintf(stderr, "오류: 존재하지 않는 ID\n");
            break;
        }
        case 4:
            student_print_all();
            break;
        case 5:
            printf("총 학생 수: %d\n", student_count());
            break;
        case 0:
            running = false;
            break;
        default:
            fprintf(stderr, "올바른 메뉴를 선택하세요.\n");
        }
    }

    printf("프로그램을 종료합니다.\n");
    return 0;
}

/* ── 유틸 구현 ── */
static int read_int(const char *prompt)
{
    int value;
    printf("%s", prompt);
    if (scanf("%d", &value) != 1) {
        fprintf(stderr, "입력 오류\n");
        value = -1;
    }
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {} /* 버퍼 비우기 */
    return value;
}

static void read_str(const char *prompt, char *buf, size_t len)
{
    printf("%s", prompt);
    if (fgets(buf, (int)len, stdin) != NULL) {
        buf[strcspn(buf, "\n")] = '\0';
    }
}
