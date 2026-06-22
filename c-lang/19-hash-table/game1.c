/*
 * game1.c — 해시 테이블 기반 Inventory 시스템
 *
 * 텍스트 기반 RPG 아이템 관리 미니 게임
 * 해시 테이블을 이용해 플레이어의 인벤토리(아이템 추가/제거/조회/목록)를 관리한다.
 *
 * 사용법: inventory
 *  add <아이템> <개수>   — 아이템 추가 (개수만큼 합산)
 *  remove <아이템>       — 아이템 제거
 *  show <아이템>         — 특정 아이템 조회
 *  list                  — 전체 인벤토리 출력
 *  clear                 — 인벤토리 비우기
 *  help                  — 도움말
 *  quit                  — 종료
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define TABLE_SIZE  31
#define NAME_SIZE   64
#define CMD_SIZE   256

typedef struct Node {
    char           name[NAME_SIZE];
    int            count;
    struct Node   *next;
} Node;

typedef struct {
    Node **buckets;
    int    size;
} HashTable;

typedef struct {
    char name[CMD_SIZE];
    char arg1[CMD_SIZE];
    char arg2[CMD_SIZE];
} Command;

/* ── Hash table implementation ── */

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
    ht->size = size;
    ht->buckets = calloc((size_t)size, sizeof(Node *));
    if (ht->buckets == NULL) { free(ht); return NULL; }
    return ht;
}

static void ht_destroy(HashTable *ht)
{
    if (ht == NULL) return;
    for (int i = 0; i < ht->size; i++) {
        Node *c = ht->buckets[i];
        while (c != NULL) { Node *n = c->next; free(c); c = n; }
    }
    free(ht->buckets);
    free(ht);
}

static Node **ht_lookup_node(HashTable *ht, const char *key, bool create)
{
    unsigned int idx = hash_djb2(key, ht->size);
    Node **pp = &ht->buckets[idx];
    Node *c = *pp;
    while (c != NULL) {
        if (strcmp(c->name, key) == 0) return pp;
        pp = &c->next; c = c->next;
    }
    if (!create) return NULL;
    Node *node = malloc(sizeof(Node));
    if (node == NULL) return NULL;
    strncpy(node->name, key, NAME_SIZE - 1);
    node->name[NAME_SIZE - 1] = '\0';
    node->count = 0;
    node->next = NULL;
    *pp = node;
    return pp;
}

static bool ht_has(HashTable *ht, const char *key)
{
    return ht_lookup_node(ht, key, false) != NULL;
}

static bool ht_set(HashTable *ht, const char *key, int count)
{
    Node **pp = ht_lookup_node(ht, key, true);
    if (pp == NULL) return false;
    (*pp)->count = count;
    return true;
}

static int ht_get(HashTable *ht, const char *key, int def_val)
{
    Node **pp = ht_lookup_node(ht, key, false);
    return pp ? (*pp)->count : def_val;
}

static bool ht_remove(HashTable *ht, const char *key)
{
    Node **pp = ht_lookup_node(ht, key, false);
    if (pp == NULL) return false;
    Node *c = *pp;
    *pp = c->next;
    free(c);
    return true;
}

static void ht_list_all(HashTable *ht)
{
    printf("\n═══════ 인벤토리 ═══════\n");
    int used = 0;
    for (int i = 0; i < ht->size; i++) {
        Node *c = ht->buckets[i];
        while (c != NULL) {
            used++;
            printf("  [%2d] %-20s x%d\n", used, c->name, c->count);
            c = c->next;
        }
    }
    if (used == 0) printf("  (비어 있음)\n");
    printf("════════════════════════\n");
}

/* ── Command parsing ── */

static void trim(char *s)
{
    while (isspace((unsigned char)*s)) s++;
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) *end-- = '\0';
}

static void parse_command(const char *line, Command *cmd)
{
    memset(cmd, 0, sizeof(*cmd));
    char buf[CMD_SIZE];
    strncpy(buf, line, CMD_SIZE - 1);
    buf[CMD_SIZE - 1] = '\0';
    trim(buf);

    char *tok[3] = { NULL, NULL, NULL };
    int n = 0;
    char *p = buf;
    while (n < 3 && *p) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;
        tok[n++] = p;
        while (*p && !isspace((unsigned char)*p)) p++;
        if (*p) *p++ = '\0';
    }

    if (tok[0]) strncpy(cmd->name, tok[0], CMD_SIZE - 1);
    if (tok[1]) strncpy(cmd->arg1, tok[1], NAME_SIZE - 1);
    if (tok[2]) strncpy(cmd->arg2, tok[2], NAME_SIZE - 1);
}

static void print_help(void)
{
    printf("\n────── 명령어 ──────\n");
    printf("  add <아이템> <개수>  — 아이템 추가\n");
    printf("  remove <아이템>      — 아이템 제거\n");
    printf("  show <아이템>        — 특정 아이템 조회\n");
    printf("  list                 — 전체 인벤토리\n");
    printf("  clear                — 인벤토리 비우기\n");
    printf("  help                 — 도움말\n");
    printf("  quit                 — 종료\n");
    printf("──────────────────────\n");
}

int main(void)
{
    HashTable *inv = ht_create(TABLE_SIZE);
    if (inv == NULL) {
        fprintf(stderr, "인벤토리 초기화 실패\n");
        return 1;
    }

    /* 초기 아이템 */
    ht_set(inv, "소드",      1);
    ht_set(inv, "포션",      5);
    ht_set(inv, "방패",      1);
    ht_set(inv, "금화",    100);

    printf("═══════════════════════════════\n");
    printf("    🎒  RPG 인벤토리 시스템  🎒\n");
    printf("═══════════════════════════════\n");
    print_help();

    char line[CMD_SIZE];
    while (true) {
        printf("\n> ");
        if (fgets(line, sizeof(line), stdin) == NULL) break;

        Command cmd;
        parse_command(line, &cmd);
        if (cmd.name[0] == '\0') continue;

        if (strcmp(cmd.name, "quit") == 0 || strcmp(cmd.name, "exit") == 0) {
            printf("인벤토리를 종료합니다.\n");
            break;

        } else if (strcmp(cmd.name, "help") == 0) {
            print_help();

        } else if (strcmp(cmd.name, "list") == 0) {
            ht_list_all(inv);

        } else if (strcmp(cmd.name, "clear") == 0) {
            ht_destroy(inv);
            inv = ht_create(TABLE_SIZE);
            printf("인벤토리를 비웠습니다.\n");

        } else if (strcmp(cmd.name, "add") == 0) {
            if (cmd.arg1[0] == '\0' || cmd.arg2[0] == '\0') {
                fprintf(stderr, "사용법: add <아이템> <개수>\n");
            } else {
                int cur = ht_get(inv, cmd.arg1, 0);
                int add = atoi(cmd.arg2);
                if (add <= 0) {
                    fprintf(stderr, "개수는 양수여야 합니다.\n");
                } else {
                    ht_set(inv, cmd.arg1, cur + add);
                    printf("'%s'을(를) %d개 추가했습니다. (보유: %d)\n",
                           cmd.arg1, add, cur + add);
                }
            }

        } else if (strcmp(cmd.name, "remove") == 0) {
            if (cmd.arg1[0] == '\0') {
                fprintf(stderr, "사용법: remove <아이템>\n");
            } else {
                if (ht_has(inv, cmd.arg1)) {
                    ht_remove(inv, cmd.arg1);
                    printf("'%s'을(를) 인벤토리에서 제거했습니다.\n", cmd.arg1);
                } else {
                    fprintf(stderr, "'%s'은(는) 인벤토리에 없습니다.\n", cmd.arg1);
                }
            }

        } else if (strcmp(cmd.name, "show") == 0) {
            if (cmd.arg1[0] == '\0') {
                fprintf(stderr, "사용법: show <아이템>\n");
            } else {
                int cnt = ht_get(inv, cmd.arg1, -1);
                if (cnt >= 0)
                    printf("'%s': x%d\n", cmd.arg1, cnt);
                else
                    printf("'%s'은(는) 인벤토리에 없습니다.\n", cmd.arg1);
            }

        } else {
            fprintf(stderr, "알 수 없는 명령어: '%s' (help 입력)\n", cmd.name);
        }
    }

    ht_destroy(inv);
    return 0;
}
