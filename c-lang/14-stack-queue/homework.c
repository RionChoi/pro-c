// homework.c — 스택 구현: push/pop/top 연산
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STACK 64

typedef struct {
    int data[MAX_STACK];
    int top;
} Stack;

static void stack_init(Stack *s) {
    s->top = -1;
}

static int stack_is_empty(const Stack *s) {
    return s->top == -1;
}

static int stack_is_full(const Stack *s) {
    return s->top == MAX_STACK - 1;
}

static int stack_push(Stack *s, int value) {
    if (stack_is_full(s)) {
        fprintf(stderr, "스택 오버플로우: push 실패\n");
        return -1;
    }
    s->data[++s->top] = value;
    return 0;
}

static int stack_pop(Stack *s, int *out) {
    if (stack_is_empty(s)) {
        fprintf(stderr, "스택 언더플로우: pop 실패\n");
        return -1;
    }
    *out = s->data[s->top--];
    return 0;
}

static int stack_top(const Stack *s) {
    if (stack_is_empty(s)) {
        fprintf(stderr, "스택이 비어있습니다.\n");
        return -1;
    }
    return s->data[s->top];
}

static void stack_print(const Stack *s) {
    printf("스택 (bottom→top): ");
    if (stack_is_empty(s)) {
        printf("[empty]");
    } else {
        for (int i = 0; i <= s->top; i++) {
            printf("%d", s->data[i]);
            if (i < s->top) printf(", ");
        }
    }
    printf("\n");
}

int main(void) {
    Stack s;
    stack_init(&s);

    printf("=== 스택 연산 테스트 ===\n");
    printf("스택 초기 상태: "); stack_print(&s);

    printf("\n-- Push 10, 20, 30 --\n");
    stack_push(&s, 10);
    stack_push(&s, 20);
    stack_push(&s, 30);
    printf("После push: "); stack_print(&s);
    printf("Top: %d\n", stack_top(&s));

    printf("\n-- Pop --\n");
    int val;
    stack_pop(&s, &val);
    printf("Pop: %d", val);
    printf(" — после pop: "); stack_print(&s);

    printf("\n-- Push 40, 50 --\n");
    stack_push(&s, 40);
    stack_push(&s, 50);
    printf("После push: "); stack_print(&s);
    printf("Top: %d\n", stack_top(&s));

    printf("\n-- 전체 pop --\n");
    while (!stack_is_empty(&s)) {
        stack_pop(&s, &val);
        printf("Pop: %d\n", val);
    }

    printf("\n-- 빈 스택 pop 시도 --\n");
    if (stack_pop(&s, &val) != 0) {
        printf("예상대로 오류 처리됨.\n");
    }

    printf("\n테스트 완료.\n");
    return 0;
}
