/*
 * homework.c — 해시 테이블 구현 (분리 연결법)
 *
 * 기능: 키-값 쌍의 삽입, 검색, 삭제, 전체 출력, 메모리 해제
 * 해시 함수: djb2 (Dan Bernstein)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TABLE_SIZE 31
#define KEY_SIZE   64

typedef struct Node {
    char           key[KEY_SIZE];
    int            value;
    struct Node    *next;
} Node;

typedef struct {
    Node **buckets;
    int    size;
} HashTable;

static unsigned int hash_djb2(const char *key, int table_size)
{
    unsigned int h = 5381;
    int c;
    while ((c = (unsigned char)(*key++)) != '\0')
        h = ((h << 5) + h) + c;  /* h * 33 + c */
    return h % (unsigned int)table_size;
}

HashTable *ht_create(int size)
{
    HashTable *ht = malloc(sizeof(HashTable));
    if (ht == NULL) {
        fprintf(stderr, "해시 테이블 생성 실패\n");
        return NULL;
    }
    ht->size   = size;
    ht->buckets = calloc((size_t)size, sizeof(Node *));
    if (ht->buckets == NULL) {
        fprintf(stderr, "버킷 배열 생성 실패\n");
        free(ht);
        return NULL;
    }
    return ht;
}

void ht_destroy(HashTable *ht)
{
    if (ht == NULL) return;
    for (int i = 0; i < ht->size; i++) {
        Node *curr = ht->buckets[i];
        while (curr != NULL) {
            Node *next = curr->next;
            free(curr);
            curr = next;
        }
    }
    free(ht->buckets);
    free(ht);
}

bool ht_insert(HashTable *ht, const char *key, int value)
{
    if (ht == NULL || key == NULL) return false;

    unsigned int idx = hash_djb2(key, ht->size);

    /* 이미 존재하는 키なら更新 */
    Node *curr = ht->buckets[idx];
    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            curr->value = value;
            return true;
        }
        curr = curr->next;
    }

    /* 새 노드 생성 —链表头部插入 */
    Node *node = malloc(sizeof(Node));
    if (node == NULL) {
        fprintf(stderr, "노드 생성 실패\n");
        return false;
    }
    strncpy(node->key, key, KEY_SIZE - 1);
    node->key[KEY_SIZE - 1] = '\0';
    node->value = value;
    node->next  = ht->buckets[idx];
    ht->buckets[idx] = node;
    return true;
}

bool ht_search(const HashTable *ht, const char *key, int *out_value)
{
    if (ht == NULL || key == NULL) return false;

    unsigned int idx = hash_djb2(key, ht->size);
    Node *curr = ht->buckets[idx];
    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            if (out_value != NULL) *out_value = curr->value;
            return true;
        }
        curr = curr->next;
    }
    return false;
}

bool ht_delete(HashTable *ht, const char *key)
{
    if (ht == NULL || key == NULL) return false;

    unsigned int idx = hash_djb2(key, ht->size);
    Node **pp = &ht->buckets[idx];
    Node  *curr = *pp;

    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            *pp = curr->next;
            free(curr);
            return true;
        }
        pp = &curr->next;
        curr = curr->next;
    }
    return false;
}

void ht_print(const HashTable *ht)
{
    if (ht == NULL) return;
    printf("\n===== 해시 테이블 (size=%d) =====\n", ht->size);
    int used = 0;
    for (int i = 0; i < ht->size; i++) {
        Node *curr = ht->buckets[i];
        if (curr == NULL) continue;
        used++;
        printf("[%3d]: ", i);
        while (curr != NULL) {
            printf("(%s → %d)", curr->key, curr->value);
            if (curr->next != NULL) printf(" → ");
            curr = curr->next;
        }
        printf("\n");
    }
    printf("사용 중인 버킷: %d / %d\n", used, ht->size);
    printf("================================\n");
}

int main(void)
{
    HashTable *ht = ht_create(TABLE_SIZE);
    if (ht == NULL) return 1;

    /* 테스트 데이터 삽입 */
    ht_insert(ht, "apple",    3);
    ht_insert(ht, "banana",   7);
    ht_insert(ht, "cherry",  12);
    ht_insert(ht, "date",     5);
    ht_insert(ht, "elder",    9);
    ht_insert(ht, "fig",      1);
    ht_insert(ht, "grape",   15);
    ht_insert(ht, "apple",    6);  /* 갱신 테스트 */

    ht_print(ht);

    /* 검색 테스트 */
    const char *keys[] = { "banana", "cherry", "mango" };
    for (int i = 0; i < 3; i++) {
        int val;
        if (ht_search(ht, keys[i], &val))
            printf("'%s' 발견: %d\n", keys[i], val);
        else
            printf("'%s' 찾을 수 없음\n", keys[i]);
    }

    /* 삭제 테스트 */
    printf("\n--- 'banana' 삭제 ---\n");
    if (ht_delete(ht, "banana"))
        printf("삭제 완료\n");
    else
        printf("삭제 실패: 키가 존재하지 않음\n");

    ht_print(ht);

    ht_destroy(ht);
    return 0;
}
