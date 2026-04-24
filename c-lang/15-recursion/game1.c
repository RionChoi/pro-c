#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DISKS 8

typedef struct {
    int disks[MAX_DISKS];
    int top;
} Rod;

static Rod rods[3];
static int move_count = 0;

void init_rod(Rod *r, int capacity) {
    r->top = 0;
    for (int i = 0; i < capacity; i++) {
        r->disks[i] = 0;
    }
}

int is_empty(const Rod *r) {
    return r->top == 0;
}

int is_full(const Rod *r) {
    return r->top >= MAX_DISKS;
}

int pop(Rod *r) {
    if (is_empty(r)) {
        return -1;
    }
    return r->disks[--r->top];
}

void push(Rod *r, int disk) {
    if (is_full(r)) {
        return;
    }
    r->disks[r->top++] = disk;
}

void init_game(int num_disks) {
    move_count = 0;
    for (int i = 0; i < 3; i++) {
        init_rod(&rods[i], MAX_DISKS);
    }
    for (int i = num_disks; i >= 1; i--) {
        push(&rods[0], i);
    }
}

void print_rod(int idx) {
    const Rod *r = &rods[idx];
    printf("  %c: ", 'A' + idx);
    if (is_empty(r)) {
        printf("|");
    } else {
        for (int i = 0; i < r->top; i++) {
            printf("%d", r->disks[i]);
            if (i < r->top - 1) printf("-");
        }
    }
    printf("\n");
}

void print_state(int num_disks) {
    printf("\n=== 현재 상태 (이동 횟수: %d) ===\n", move_count);
    for (int d = num_disks; d >= 0; d--) {
        for (int c = 0; c < 3; c++) {
            const Rod *r = &rods[c];
            printf("  %c: ", 'A' + c);
            if (d < r->top) {
                for (int disk = r->disks[d]; disk > 0; disk--) {
                    printf("#");
                }
            } else if (d == 0 && is_empty(r)) {
                printf("|");
            } else {
                printf(" ");
            }
            printf("\n");
        }
        if (d > 0) printf("\n");
    }
    print_rod(0);
    print_rod(1);
    print_rod(2);
    printf("\n");
}

void move_disk(int from, int to) {
    int disk = pop(&rods[from]);
    if (disk == -1) {
        return;
    }
    push(&rods[to], disk);
    move_count++;
    printf("이동 %d: %c --> %c (원반 %d)\n", move_count, 'A' + from, 'A' + to, disk);
}

void hanoi(int n, int from, int to, int aux) {
    if (n == 0) {
        return;
    }
    hanoi(n - 1, from, aux, to);
    move_disk(from, to);
    hanoi(n - 1, aux, to, from);
}

int main(void) {
    int num_disks;

    printf("=================================\n");
    printf("       하노이의 탑 (Tower of Hanoi)\n");
    printf("=================================\n");
    printf("원반 수를 선택하세요 (1~8): ");

    if (scanf("%d", &num_disks) != 1) {
        fprintf(stderr, "입력 오류\n");
        return 1;
    }

    if (num_disks < 1 || num_disks > 8) {
        fprintf(stderr, "1부터 8 사이의 값을 입력하세요.\n");
        return 1;
    }

    printf("\n규칙: 모든 원반을 A에서 C로 옮기세요.\n");
    printf("      한 번에 하나의 원반만 움직일 수 있습니다.\n");
    printf("      큰 원반은 작은 원반 위로만 이동할 수 있습니다.\n");

    init_game(num_disks);

    printf("\n--- 초기 상태 ---\n");
    for (int i = num_disks; i >= 1; i--) {
        printf("  A: ");
        for (int d = 0; d < i; d++) printf("#");
        printf("\n");
    }
    print_rod(0);
    print_rod(1);
    print_rod(2);
    printf("\n총 이동 횟수: %d\n", move_count);

    printf("\n=== 풀이 시작 ===\n");
    hanoi(num_disks, 0, 2, 1);

    printf("\n=== 최종 상태 ===\n");
    print_rod(0);
    print_rod(1);
    print_rod(2);

    int expected = (1 << num_disks) - 1;
    printf("\n총 이동 횟수: %d (최소 이동 횟수: %d)\n", move_count, expected);
    printf("게임 완료!\n");

    return 0;
}
