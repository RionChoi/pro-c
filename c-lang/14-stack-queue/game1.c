// game1.c — 괄호 균형 검사기: 스택 활용 게임
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_EXPR 256

typedef struct {
    char data[MAX_EXPR];
    int top;
} Stack;

static void stack_init(Stack *s) {
    s->top = -1;
}

static int stack_is_empty(const Stack *s) {
    return s->top == -1;
}

static int stack_push(Stack *s, char c) {
    if (s->top >= (int)(sizeof(s->data) / sizeof(s->data[0])) - 1) {
        return -1;
    }
    s->data[++s->top] = c;
    return 0;
}

static char stack_pop(Stack *s) {
    if (stack_is_empty(s)) return '\0';
    return s->data[s->top--];
}

static int is_balanced(const char *expr) {
    Stack s;
    stack_init(&s);

    for (int i = 0; expr[i] != '\0'; i++) {
        char c = expr[i];
        if (c == '(' || c == '[' || c == '{') {
            if (stack_push(&s, c) != 0) return -1;
        } else if (c == ')' || c == ']' || c == '}') {
            char top = stack_pop(&s);
            if (top == '\0') {
                fprintf(stderr, "오류: '%c' (위치 %d) — 대응되는 여는 괄호 없음\n", c, i + 1);
                return 0;
            }
            if ((c == ')' && top != '(') ||
                (c == ']' && top != '[') ||
                (c == '}' && top != '{')) {
                fprintf(stderr, "오류: '%c' (위치 %d) — '%c'와 짝이 맞지 않음\n", c, i + 1, top);
                return 0;
            }
        }
    }

    if (!stack_is_empty(&s)) {
        fprintf(stderr, "오류: 여는 괄호가 %d개 남음\n", s.top + 1);
        return 0;
    }

    return 1;
}

static void trim_newline(char *s) {
    s[strcspn(s, "\n")] = '\0';
}

int main(void) {
    char line[MAX_EXPR];

    printf("=================================\n");
    printf("  괄호 균형 검사기 게임\n");
    printf("=================================\n");
    printf("(), [], {} 괄호의 균형을 검사합니다.\n");
    printf("종료: q\n\n");

    while (1) {
        printf("수식 입력> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        trim_newline(line);

        if (strcmp(line, "q") == 0) break;
        if (strlen(line) == 0) continue;

        // 공백 제거 (편의상)
        char cleaned[MAX_EXPR];
        int j = 0;
        for (int i = 0; line[i] != '\0'; i++) {
            if (line[i] == '(' || line[i] == ')' ||
                line[i] == '[' || line[i] == ']' ||
                line[i] == '{' || line[i] == '}') {
                cleaned[j++] = line[i];
            }
        }
        cleaned[j] = '\0';

        if (j == 0) {
            printf("  괄호가 없는 수식입니다.\n\n");
            continue;
        }

        printf("  검사: %s\n", cleaned);
        int result = is_balanced(cleaned);
        if (result == 1) {
            printf("  결과: 균형 O (PASS)\n\n");
        } else {
            printf("  결과: 균형 X (FAIL)\n\n");
        }
    }

    printf("종료합니다.\n");
    return 0;
}
