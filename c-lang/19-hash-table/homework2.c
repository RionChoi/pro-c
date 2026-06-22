/*
 * homework2.c — 단어 빈도수 세기 (해시 테이블 기반)
 *
 * 사용법: wordcount [파일명]
 *   파일省略: stdin에서 읽음
 *
 * 해시 테이블을 이용해 텍스트 파일의 단어 빈도수를 효율적으로 카운트한다.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define INIT_SIZE    31
#define KEY_SIZE     256
#define BUF_SIZE     4096

typedef struct Node {
    char           key[KEY_SIZE];
    int            count;
    struct Node   *next;
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
        h = ((h << 5) + h) + c;
    return h % (unsigned int)table_size;
}

static HashTable *ht_create(int size)
{
    HashTable *ht = malloc(sizeof(HashTable));
    if (ht == NULL) return NULL;
    ht->size   = size;
    ht->buckets = calloc((size_t)size, sizeof(Node *));
    if (ht->buckets == NULL) {
        free(ht);
        return NULL;
    }
    return ht;
}

static void ht_destroy(HashTable *ht)
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

static Node **ht_lookup_node(HashTable *ht, const char *key, bool create)
{
    unsigned int idx = hash_djb2(key, ht->size);
    Node **pp = &ht->buckets[idx];
    Node  *curr = *pp;

    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0)
            return pp;
        pp = &curr->next;
        curr = curr->next;
    }

    if (!create) return NULL;

    Node *node = malloc(sizeof(Node));
    if (node == NULL) return NULL;
    strncpy(node->key, key, KEY_SIZE - 1);
    node->key[KEY_SIZE - 1] = '\0';
    node->count = 0;
    node->next  = NULL;
    *pp = node;
    return pp;
}

static void htIncrement(HashTable *ht, const char *key)
{
    Node **pp = ht_lookup_node(ht, key, true);
    if (pp != NULL) (*pp)->count++;
}

static int count_nodes(HashTable *ht)
{
    int n = 0;
    for (int i = 0; i < ht->size; i++) {
        Node *c = ht->buckets[i];
        while (c != NULL) { n++; c = c->next; }
    }
    return n;
}

static int cmp_count_desc(const void *a, const void *b)
{
    /* 포인터의 포인터를 역참조 */
    const Node *na = *(const Node *const *)a;
    const Node *nb = *(const Node *const *)b;
    return nb->count - na->count;
}

static void process_stream(FILE *fp, HashTable *ht)
{
    char word[KEY_SIZE];
    int pos = 0;
    int c;

    while ((c = fgetc(fp)) != EOF) {
        if (isalnum((unsigned char)c) || c == '_') {
            if (pos < KEY_SIZE - 1)
                word[pos++] = (char)tolower((unsigned char)c);
        } else {
            if (pos > 0) {
                word[pos] = '\0';
                htIncrement(ht, word);
                pos = 0;
            }
        }
    }
    if (pos > 0) {
        word[pos] = '\0';
        htIncrement(ht, word);
    }
}

int main(int argc, char *argv[])
{
    FILE *fp = stdin;
    bool file_given = false;

    if (argc > 2) {
        fprintf(stderr, "사용법: %s [파일]\n", argv[0]);
        return 1;
    }

    if (argc == 2) {
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            fprintf(stderr, "오류: '%s'을(를) 열 수 없습니다.\n", argv[1]);
            return 1;
        }
        file_given = true;
    }

    HashTable *ht = ht_create(INIT_SIZE);
    if (ht == NULL) {
        fprintf(stderr, "해시 테이블 생성 실패\n");
        if (file_given) fclose(fp);
        return 1;
    }

    process_stream(fp, ht);
    if (file_given) fclose(fp);

    /* 모든 노드를 배열에 수집 */
    int n = count_nodes(ht);
    Node **arr = malloc((size_t)n * sizeof(Node *));
    if (arr == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        ht_destroy(ht);
        return 1;
    }

    int idx = 0;
    for (int i = 0; i < ht->size; i++) {
        Node *c = ht->buckets[i];
        while (c != NULL) { arr[idx++] = c; c = c->next; }
    }

    qsort(arr, (size_t)n, sizeof(Node *), cmp_count_desc);

    printf("\n%-40s %s\n", "단어", "빈도");
    printf("----------------------------------------\n");
    int top = n < 30 ? n : 30;
    for (int i = 0; i < top; i++)
        printf("%-40s %5d\n", arr[i]->key, arr[i]->count);
    printf("----------------------------------------\n");
    printf("총 고유 단어 수: %d\n", n);

    free(arr);
    ht_destroy(ht);
    return 0;
}
