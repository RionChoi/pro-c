// homework2.c — 큐 구현: enqueue/dequeue/front 연산
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUEUE 64

typedef struct {
    int data[MAX_QUEUE];
    int front;
    int rear;
    int count;
} Queue;

static void queue_init(Queue *q) {
    q->front = 0;
    q->rear = 0;
    q->count = 0;
}

static int queue_is_empty(const Queue *q) {
    return q->count == 0;
}

static int queue_is_full(const Queue *q) {
    return q->count == MAX_QUEUE;
}

static int queue_enqueue(Queue *q, int value) {
    if (queue_is_full(q)) {
        fprintf(stderr, "큐 포화: enqueue 실패\n");
        return -1;
    }
    q->data[q->rear] = value;
    q->rear = (q->rear + 1) % MAX_QUEUE;
    q->count++;
    return 0;
}

static int queue_dequeue(Queue *q, int *out) {
    if (queue_is_empty(q)) {
        fprintf(stderr, "큐 공백: dequeue 실패\n");
        return -1;
    }
    *out = q->data[q->front];
    q->front = (q->front + 1) % MAX_QUEUE;
    q->count--;
    return 0;
}

static int queue_front(const Queue *q) {
    if (queue_is_empty(q)) {
        fprintf(stderr, "큐가 비어있습니다.\n");
        return -1;
    }
    return q->data[q->front];
}

static void queue_print(const Queue *q) {
    printf("큐 (front→rear): ");
    if (queue_is_empty(q)) {
        printf("[empty]");
    } else {
        int idx = q->front;
        for (int i = 0; i < q->count; i++) {
            printf("%d", q->data[idx]);
            if (i < q->count - 1) printf(", ");
            idx = (idx + 1) % MAX_QUEUE;
        }
    }
    printf("\n");
}

int main(void) {
    Queue q;
    queue_init(&q);

    printf("=== 큐 연산 테스트 ===\n");
    printf("초기 상태: "); queue_print(&q);

    printf("\n-- Enqueue 10, 20, 30 --\n");
    queue_enqueue(&q, 10);
    queue_enqueue(&q, 20);
    queue_enqueue(&q, 30);
    printf("После enqueue: "); queue_print(&q);
    printf("Front: %d\n", queue_front(&q));

    printf("\n-- Dequeue --\n");
    int val;
    queue_dequeue(&q, &val);
    printf("Dequeue: %d", val);
    printf(" — después de dequeue: "); queue_print(&q);

    printf("\n-- Enqueue 40, 50, 60 --\n");
    queue_enqueue(&q, 40);
    queue_enqueue(&q, 50);
    queue_enqueue(&q, 60);
    printf("Después de enqueue: "); queue_print(&q);

    printf("\n-- 전체 dequeue --\n");
    while (!queue_is_empty(&q)) {
        queue_dequeue(&q, &val);
        printf("Dequeue: %d\n", val);
    }

    printf("\n-- 빈 큐 dequeue 시도 --\n");
    if (queue_dequeue(&q, &val) != 0) {
        printf("예상대로 오류 처리됨.\n");
    }

    printf("\n테스트 완료.\n");
    return 0;
}
