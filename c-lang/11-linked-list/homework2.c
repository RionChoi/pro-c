/*
 * 세션 11 - 과제 2: 연결 리스트 노드 삭제
 * 값으로 노드를 찾아 삭제하는 기능을 추가한 연결 리스트
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

int delete_node(Node **head, int value) {
    if (*head == NULL) return 0;

    if ((*head)->data == value) {
        Node *temp = *head;
        *head = (*head)->next;
        free(temp);
        return 1;
    }

    Node *curr = *head;
    while (curr->next != NULL) {
        if (curr->next->data == value) {
            Node *temp = curr->next;
            curr->next = temp->next;
            free(temp);
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

void print_list(Node *head) {
    Node *curr = head;
    while (curr != NULL) {
        printf("%d", curr->data);
        if (curr->next != NULL) printf(" -> ");
        curr = curr->next;
    }
    printf(" -> NULL\n");
}

void free_list(Node *head) {
    Node *curr = head;
    while (curr != NULL) {
        Node *next = curr->next;
        free(curr);
        curr = next;
    }
}

int main(void) {
    Node *head = NULL;
    char input[32];

    printf("=== 연결 리스트에 정수 추가 ===\n");
    while (1) {
        printf("정수 입력 (0: 종료): ");
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
        if (value == 0) break;
        append_node(&head, value);
    }

    printf("\n현재 리스트: ");
    print_list(head);

    printf("\n=== 노드 삭제 ===\n");
    printf("삭제할 값 입력: ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        fprintf(stderr, "입력 오류\n");
        free_list(head);
        return 1;
    }
    input[strcspn(input, "\n")] = '\0';

    int del_val;
    if (sscanf(input, "%d", &del_val) != 1) {
        fprintf(stderr, "정수를 입력하세요.\n");
        free_list(head);
        return 1;
    }

    if (delete_node(&head, del_val)) {
        printf("값 %d 삭제 완료.\n", del_val);
    } else {
        printf("값 %d를 찾을 수 없습니다.\n", del_val);
    }

    printf("최종 리스트: ");
    print_list(head);

    free_list(head);
    return 0;
}
