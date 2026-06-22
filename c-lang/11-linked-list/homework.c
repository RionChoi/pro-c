/*
 * 세션 11 - 과제 1: 연결 리스트 기본
 * 정수를 입력받아 연결 리스트에 추가, 0 입력 시 전체 출력 후 종료
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node *create_node(int value) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        return NULL;
    }
    new_node->data = value;
    new_node->next = NULL;
    return new_node;
}

void append_node(Node **head, int value) {
    Node *new_node = create_node(value);
    if (new_node == NULL) return;

    if (*head == NULL) {
        *head = new_node;
        return;
    }

    Node *curr = *head;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = new_node;
}

void free_list(Node *head) {
    Node *curr = head;
    while (curr != NULL) {
        Node *next = curr->next;
        free(curr);
        curr = next;
    }
}

void print_list(Node *head) {
    Node *curr = head;
    while (curr != NULL) {
        printf("%d", curr->data);
        if (curr->next != NULL) {
            printf(" -> ");
        }
        curr = curr->next;
    }
    printf(" -> NULL\n");
}

int main(void) {
    Node *head = NULL;
    char input[32];

    printf("=== 연결 리스트에 정수 추가 ===\n");
    printf("정수를 입력하세요 (0 입력 시 종료):\n");

    while (1) {
        printf(">> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            fprintf(stderr, "입력 오류\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0';

        int value;
        if (sscanf(input, "%d", &value) != 1) {
            fprintf(stderr, "정수를 입력하세요.\n");
            continue;
        }

        if (value == 0) {
            break;
        }

        append_node(&head, value);
    }

    printf("\n리스트 내용: ");
    print_list(head);

    free_list(head);
    return 0;
}
