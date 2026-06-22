/*
 * student.h — 학생 관리 헤더 (멀티파일 프로젝트)
 */

#ifndef STUDENT_H
#define STUDENT_H

#include <stdbool.h>

#define MAX_NAME     64
#define MAX_STUDENTS 100

typedef struct {
    int    id;
    char   name[MAX_NAME];
    int    score;
    bool   active;
} Student;

/* ── CRUD 함수 선언 ── */
void   student_init(void);
bool   student_add(const char *name, int score);
bool   student_remove(int id);
void   student_print_all(void);
bool   student_update_score(int id, int score);
int    student_count(void);

#endif /* STUDENT_H */
