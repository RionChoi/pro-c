/*
 * 세션 12 - 과제 2: 이진 탐색 트리 (Binary Search Tree)
 * 삽입, 탐색, 중위 순회, 최대/최소 값 찾기
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TreeNode {
    int key;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

TreeNode *create_node(int key) {
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    if (node == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        return NULL;
    }
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    return node;
}

TreeNode *insert(TreeNode *root, int key) {
    if (root == NULL) {
        return create_node(key);
    }
    if (key < root->key) {
        root->left = insert(root->left, key);
    } else if (key > root->key) {
        root->right = insert(root->right, key);
    }
    return root;
}

TreeNode *search(TreeNode *root, int key) {
    if (root == NULL || root->key == key) {
        return root;
    }
    if (key < root->key) {
        return search(root->left, key);
    }
    return search(root->right, key);
}

void inorder(TreeNode *root) {
    if (root == NULL) return;
    inorder(root->left);
    printf("%d ", root->key);
    inorder(root->right);
}

TreeNode *find_min(TreeNode *node) {
    while (node->left != NULL) node = node->left;
    return node;
}

TreeNode *find_max(TreeNode *node) {
    while (node->right != NULL) node = node->right;
    return node;
}

void free_tree(TreeNode *root) {
    if (root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

int main(void) {
    TreeNode *root = NULL;
    int values[] = {50, 30, 70, 20, 40, 60, 80};
    int n = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < n; i++) {
        root = insert(root, values[i]);
    }

    printf("중위 순회 (오름차순): ");
    inorder(root);
    printf("\n");

    printf("최소값: %d\n", find_min(root)->key);
    printf("최대값: %d\n", find_max(root)->key);

    int key;
    char input[32];
    printf("\n탐색할 값 입력: ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        fprintf(stderr, "입력 오류\n");
        free_tree(root);
        return 1;
    }
    input[strcspn(input, "\n")] = '\0';

    if (sscanf(input, "%d", &key) != 1) {
        fprintf(stderr, "정수를 입력하세요.\n");
        free_tree(root);
        return 1;
    }

    TreeNode *found = search(root, key);
    if (found != NULL) {
        printf("값 %d를 찾았습니다.\n", key);
    } else {
        printf("값 %d는 트리에 없습니다.\n", key);
    }

    free_tree(root);
    return 0;
}
