#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define MAX_HISTORY 10

typedef double (*Operation)(double, double);

typedef struct {
    char name[32];
    char symbol;
    Operation op;
} OpInfo;

double op_add(double a, double b) { return a + b; }
double op_sub(double a, double b) { return a - b; }
double op_mul(double a, double b) { return a * b; }
double op_div(double a, double b) {
    if (b == 0) {
        fprintf(stderr, "0으로 나눌 수 없습니다.\n");
        return 0.0 / 0.0;
    }
    return a / b;
}
double op_pow(double a, double b) { return pow(a, b); }
double op_sqrt(double a, double b) {
    (void)b;
    if (a < 0) {
        fprintf(stderr, "음수의 제곱근은 실수 범위에 없습니다.\n");
        return 0.0 / 0.0;
    }
    return sqrt(a);
}
double op_log(double a, double b) {
    (void)b;
    if (a <= 0) {
        fprintf(stderr, "로그의 입력은 양수여야 합니다.\n");
        return 0.0 / 0.0;
    }
    return log(a);
}
double op_mod(double a, double b) {
    if (b == 0) {
        fprintf(stderr, "0으로 나머지 연산 불가.\n");
        return 0.0;
    }
    return (double)((int)a % (int)b);
}

static OpInfo operations[] = {
    { "덧셈",     '+', op_add   },
    { "뺄셈",     '-', op_sub   },
    { "곱셈",     '*', op_mul   },
    { "나눗셈",   '/', op_div   },
    { "거듭제곱", 'p', op_pow   },
    { "제곱근",   's', op_sqrt  },
    { "로그",     'l', op_log   },
    { "나머지",   'm', op_mod   },
};
static const size_t OP_COUNT = sizeof(operations) / sizeof(operations[0]);

typedef struct {
    double a;
    double b;
    double result;
    char symbol;
} HistoryEntry;

static HistoryEntry history[MAX_HISTORY];
static size_t history_count = 0;

void print_banner(void) {
    printf("=================================\n");
    printf("  함수 포인터 계산기 (with History)\n");
    printf("=================================\n");
}

void print_operations(void) {
    printf("\n=== 사용 가능한 연산 ===\n");
    for (size_t i = 0; i < OP_COUNT; i++) {
        printf("  %c : %s\n", operations[i].symbol, operations[i].name);
    }
    printf("\n");
}

void print_history(void) {
    printf("\n=== 최근 연산 기록 ===\n");
    if (history_count == 0) {
        printf("  (기록 없음)\n");
        return;
    }
    size_t start = (history_count >= MAX_HISTORY) ? history_count - MAX_HISTORY : 0;
    for (size_t i = start; i < history_count; i++) {
        HistoryEntry *h = &history[i];
        if (h->symbol == 's' || h->symbol == 'l') {
            printf("  [%zu] %.2g %c = %.6g\n", i + 1, h->a, h->symbol, h->result);
        } else {
            printf("  [%zu] %.2g %c %.2g = %.6g\n", i + 1, h->a, h->symbol, h->b, h->result);
        }
    }
}

void add_history(double a, double b, double result, char symbol) {
    history[history_count % MAX_HISTORY] = (HistoryEntry){ a, b, result, symbol };
    history_count++;
}

OpInfo *find_op_by_symbol(char symbol) {
    for (size_t i = 0; i < OP_COUNT; i++) {
        if (operations[i].symbol == symbol) {
            return &operations[i];
        }
    }
    return NULL;
}

double get_number(const char *prompt) {
    char buf[64];
    printf("%s", prompt);
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        fprintf(stderr, "입력 오류\n");
        return 0.0 / 0.0;
    }
    return atof(buf);
}

int main(void) {
    print_banner();
    print_operations();

    while (1) {
        printf("\n명령어: ");
        printf("\n  숫자 2개 + 연산기호 (예: 10 5 +)\n");
        printf("  h:History  p:PrintOps  q:Quit\n");
        printf("> ");

        char line[128];
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        /* 공백 제거 */
        size_t len = strlen(line);
        for (size_t i = 0; i < len; i++) {
            if (isspace((unsigned char)line[i])) {
                memmove(&line[i], &line[i + 1], len - i);
                i--;
                len--;
            }
        }

        if (len == 0) continue;

        if (line[0] == 'q' || line[0] == 'Q') {
            printf("계산기를 종료합니다.\n");
            break;
        }
        if (line[0] == 'h' || line[0] == 'H') {
            print_history();
            continue;
        }
        if (line[0] == 'p' || line[0] == 'P') {
            print_operations();
            continue;
        }

        /* 파싱: a b symbol */
        double a, b;
        char symbol;
        int fields = sscanf(line, "%lf %lf %c", &a, &b, &symbol);
        if (fields < 2) {
            printf("사용법: a b <연산기호>  예: 10 5 +\n");
            continue;
        }
        if (fields == 2) {
            printf("연산 기호를 입력하세요 (예: + - * / p s l m): ");
            if (fgets(line, sizeof(line), stdin) == NULL) break;
            symbol = line[0];
        }

        OpInfo *op = find_op_by_symbol(symbol);
        if (op == NULL) {
            printf("알 수 없는 연산 기호: '%c'\n", symbol);
            print_operations();
            continue;
        }

        double result = op->op(a, b);
        printf("  결과: %.6g\n", result);
        add_history(a, b, result, symbol);
    }

    print_history();
    return 0;
}
