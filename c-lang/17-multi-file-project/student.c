/*
 * student.c — 학생 CRUD 구현 (멀티파일 프로젝트)
 */

#include <stdio.h>
#include <string.h>
#include "student.h"

static Student students[MAX_STUDENTS];
static int next_id = 1;

void student_init(void)
{
    for (int i = 0; i < MAX_STUDENTS; i++)
        students[i].active = false;
    next_id = 1;
}

bool student_add(const char *name, int score)
{
    for (int i = 0; i < MAX_STUDENTS; i++) {
        if (!students[i].active) {
            students[i].id     = next_id++;
            strncpy(students[i].name, name, MAX_NAME - 1);
            students[i].name[MAX_NAME - 1] = '\0';
            students[i].score  = score;
            students[i].active = true;
            return true;
        }
    }
    return false;
}

bool student_remove(int id)
{
    for (int i = 0; i < MAX_STUDENTS; i++) {
        if (students[i].active && students[i].id == id) {
            students[i].active = false;
            return true;
        }
    }
    return false;
}

bool student_update_score(int id, int score)
{
    for (int i = 0; i < MAX_STUDENTS; i++) {
        if (students[i].active && students[i].id == id) {
            students[i].score = score;
            return true;
        }
    }
    return false;
}

void student_print_all(void)
{
    printf("\n%-5s %-20s %s\n", "ID", "이름", "성적");
    printf("-----------------------------\n");
    int count = 0;
    for (int i = 0; i < MAX_STUDENTS; i++) {
        if (students[i].active) {
            printf("%-5d %-20s %d\n",
                   students[i].id, students[i].name, students[i].score);
            count++;
        }
    }
    if (count == 0)
        printf("(등록된 학생이 없습니다)\n");
}

int student_count(void)
{
    int count = 0;
    for (int i = 0; i < MAX_STUDENTS; i++)
        if (students[i].active) count++;
    return count;
}
