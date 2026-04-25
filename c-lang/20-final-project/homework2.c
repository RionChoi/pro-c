/*
 * homework2.c — 주소록 확장 기능
 *
 * homework.c 에서 다음을 확장:
 * - 해시 테이블 기반 전화번호 중복 체크 (삽입 시 O(1) 조회)
 * - CSV 파일 저장 (contacts.csv)
 * - CSV 파일 불러오기
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NAME_SIZE    64
#define PHONE_SIZE   32
#define EMAIL_SIZE   64
#define TABLE_SIZE   31
#define LINE_SIZE   512
#define DEFAULT_FN  "contacts.csv"

/* ── Contact ── */
typedef struct Contact {
    char            name[NAME_SIZE];
    char            phone[PHONE_SIZE];
    char            email[EMAIL_SIZE];
    struct Contact *next;
} Contact;

typedef struct {
    Contact *head;
    int      count;
} ContactList;

/* ── Hash Table for phone lookup ── */
typedef struct HNode {
    char           phone[PHONE_SIZE];
    Contact       *contact;
    struct HNode  *next;
} HNode;

typedef struct {
    HNode **buckets;
    int     size;
} PhoneIndex;

/* ── PhoneIndex ── */
static unsigned int hash_phone(const char *phone, int table_size)
{
    unsigned int h = 5381;
    int c;
    while ((c = (unsigned char)(*phone++)) != '\0')
        h = ((h << 5) + h) + c;
    return h % (unsigned int)table_size;
}

static PhoneIndex *pi_create(int size)
{
    PhoneIndex *pi = malloc(sizeof(PhoneIndex));
    if (pi == NULL) return NULL;
    pi->size = size;
    pi->buckets = calloc((size_t)size, sizeof(HNode *));
    if (pi->buckets == NULL) { free(pi); return NULL; }
    return pi;
}

static void pi_destroy(PhoneIndex *pi)
{
    if (pi == NULL) return;
    for (int i = 0; i < pi->size; i++) {
        HNode *c = pi->buckets[i];
        while (c != NULL) { HNode *n = c->next; free(c); c = n; }
    }
    free(pi->buckets);
    free(pi);
}

static bool pi_has(const PhoneIndex *pi, const char *phone)
{
    unsigned int idx = hash_phone(phone, pi->size);
    for (HNode *c = pi->buckets[idx]; c != NULL; c = c->next)
        if (strcmp(c->phone, phone) == 0) return true;
    return false;
}

static void pi_insert(PhoneIndex *pi, Contact *c)
{
    unsigned int idx = hash_phone(c->phone, pi->size);
    HNode *node = malloc(sizeof(HNode));
    if (node == NULL) return;
    strncpy(node->phone, c->phone, PHONE_SIZE - 1);
    node->phone[PHONE_SIZE - 1] = '\0';
    node->contact = c;
    node->next = pi->buckets[idx];
    pi->buckets[idx] = node;
}

static void pi_remove(PhoneIndex *pi, const char *phone)
{
    unsigned int idx = hash_phone(phone, pi->size);
    HNode **pp = &pi->buckets[idx];
    while (*pp != NULL) {
        if (strcmp((*pp)->phone, phone) == 0) {
            HNode *to_free = *pp;
            *pp = to_free->next;
            free(to_free);
            return;
        }
        pp = &(*pp)->next;
    }
}

/* ── ContactList ── */
static ContactList *cl_create(void)
{
    ContactList *cl = malloc(sizeof(ContactList));
    if (cl == NULL) return NULL;
    cl->head  = NULL;
    cl->count = 0;
    return cl;
}

static void cl_destroy(ContactList *cl)
{
    if (cl == NULL) return;
    Contact *c = cl->head;
    while (c != NULL) { Contact *n = c->next; free(c); c = n; }
    free(cl);
}

static Contact *cl_find_by_name(const ContactList *cl, const char *name)
{
    for (Contact *c = cl->head; c != NULL; c = c->next)
        if (strcmp(c->name, name) == 0) return c;
    return NULL;
}

static bool cl_add(ContactList *cl, PhoneIndex *pi,
                   const char *name, const char *phone, const char *email)
{
    if (cl == NULL || name == NULL || phone == NULL) return false;

    /* 해시 테이블로 O(1) 중복 체크 */
    if (pi_has(pi, phone)) {
        fprintf(stderr, "오류: 전화번호 '%s'는 이미 등록되어 있습니다.\n", phone);
        return false;
    }

    Contact *node = malloc(sizeof(Contact));
    if (node == NULL) { fprintf(stderr, "메모리 할당 실패\n"); return false; }

    strncpy(node->name, name, NAME_SIZE - 1);
    node->name[NAME_SIZE - 1] = '\0';
    strncpy(node->phone, phone, PHONE_SIZE - 1);
    node->phone[PHONE_SIZE - 1] = '\0';
    strncpy(node->email, email, EMAIL_SIZE - 1);
    node->email[EMAIL_SIZE - 1] = '\0';
    node->next = cl->head;
    cl->head = node;
    cl->count++;

    pi_insert(pi, node);  /* 해시 테이블에 등록 */
    return true;
}

static bool cl_remove_by_name(ContactList *cl, PhoneIndex *pi, const char *name)
{
    if (cl == NULL) return false;
    Contact **pp = &cl->head;
    while (*pp != NULL) {
        if (strcmp((*pp)->name, name) == 0) {
            pi_remove(pi, (*pp)->phone);
            Contact *to_free = *pp;
            *pp = to_free->next;
            free(to_free);
            cl->count--;
            return true;
        }
        pp = &(*pp)->next;
    }
    return false;
}

static void cl_print_all(const ContactList *cl)
{
    if (cl == NULL || cl->head == NULL) {
        printf("주소록이 비어 있습니다.\n");
        return;
    }
    printf("\n═══════ 주소록 (총 %d명) ═══════\n", cl->count);
    printf("%-20s %-15s %-25s\n", "이름", "전화번호", "이메일");
    printf("------------------------------------------------\n");
    int idx = 1;
    for (Contact *c = cl->head; c != NULL; c = c->next, idx++)
        printf("%2d. %-18s %-15s %-25s\n", idx, c->name, c->phone, c->email);
    printf("================================================\n");
}

/* ── CSV File I/O ── */
static void strip_newline(char *s) { s[strcspn(s, "\n")] = '\0'; }

static bool save_to_csv(const ContactList *cl, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "파일 열기 실패: %s\n", filename);
        return false;
    }
    fprintf(fp, "이름,전화번호,이메일\n");
    for (Contact *c = cl->head; c != NULL; c = c->next)
        fprintf(fp, "%s,%s,%s\n", c->name, c->phone, c->email);
    fclose(fp);
    return true;
}

static bool load_from_csv(ContactList *cl, PhoneIndex *pi, const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "파일이 없습니다: %s (새로 생성됩니다)\n", filename);
        return false;
    }

    char line[LINE_SIZE];
    if (fgets(line, sizeof(line), fp) == NULL) {  /* skip header */
        fclose(fp);
        return true;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        strip_newline(line);
        if (line[0] == '\0') continue;

        char name[NAME_SIZE] = {0};
        char phone[PHONE_SIZE] = {0};
        char email[EMAIL_SIZE] = {0};

        /* Simple CSV parse — find commas */
        int f1 = -1, f2 = -1;
        for (int i = 0; line[i]; i++)
            if (line[i] == ',') { f1 = i; break; }

        if (f1 < 0) continue;
        for (int i = f1 + 1; line[i]; i++)
            if (line[i] == ',') { f2 = i; break; }

        if (f2 < 0) continue;

        strncpy(name, line, (size_t)f1 < NAME_SIZE - 1 ? (size_t)f1 : NAME_SIZE - 1);
        name[NAME_SIZE - 1] = '\0';
        strncpy(phone, line + f1 + 1, (size_t)(f2 - f1 - 1) < PHONE_SIZE - 1 ? (size_t)(f2 - f1 - 1) : PHONE_SIZE - 1);
        phone[PHONE_SIZE - 1] = '\0';
        strncpy(email, line + f2 + 1, EMAIL_SIZE - 1);
        email[EMAIL_SIZE - 1] = '\0';

        /* Avoid duplicates on load */
        if (!pi_has(pi, phone))
            cl_add(cl, pi, name, phone, email);
    }

    fclose(fp);
    return true;
}

/* ── Interactive helpers ── */
static void read_line(const char *prompt, char *buf, int size)
{
    printf("%s", prompt);
    if (fgets(buf, size, stdin) != NULL)
        buf[strcspn(buf, "\n")] = '\0';
}

static void menu(void)
{
    printf("\n═══════ 주소록 메뉴 ═══════\n");
    printf("  1. 추가\n");
    printf("  2. 검색 (이름)\n");
    printf("  3. 삭제\n");
    printf("  4. 전체 목록\n");
    printf("  5. CSV 저장\n");
    printf("  6. CSV 불러오기\n");
    printf("  7. 종료\n");
    printf("선택: ");
}

int main(void)
{
    ContactList *cl = cl_create();
    PhoneIndex *pi = pi_create(TABLE_SIZE);
    if (cl == NULL || pi == NULL) return 1;

    char buf[256];
    char csv_file[NAME_SIZE];
    strncpy(csv_file, DEFAULT_FN, NAME_SIZE - 1);
    csv_file[NAME_SIZE - 1] = '\0';

    printf("=== 주소록 관리 시스템 (해시 테이블 + CSV) ===\n");

    while (true) {
        menu();
        if (fgets(buf, sizeof(buf), stdin) == NULL) break;
        int choice = atoi(buf);

        if (choice == 7) {
            printf("종료합니다.\n");
            break;
        }

        switch (choice) {
            case 1: {
                char name[NAME_SIZE], phone[PHONE_SIZE], email[EMAIL_SIZE];
                read_line("이름: ", name, sizeof(name));
                read_line("전화번호: ", phone, sizeof(phone));
                read_line("이메일: ", email, sizeof(email));
                if (name[0] && phone[0]) {
                    if (cl_add(cl, pi, name, phone, email))
                        printf("'%s' 추가 완료! (총 %d명)\n", name, cl->count);
                }
                break;
            }
            case 2: {
                char search[NAME_SIZE];
                read_line("검색할 이름: ", search, sizeof(search));
                Contact *found = cl_find_by_name(cl, search);
                if (found) {
                    printf("  이름:   %s\n", found->name);
                    printf("  전화:   %s\n", found->phone);
                    printf("  이메일: %s\n", found->email);
                } else {
                    printf("'%s'님의 정보를 찾을 수 없습니다.\n", search);
                }
                break;
            }
            case 3: {
                char del[NAME_SIZE];
                read_line("삭제할 이름: ", del, sizeof(del));
                if (cl_remove_by_name(cl, pi, del))
                    printf("'%s'님을 삭제했습니다.\n", del);
                else
                    fprintf(stderr, "'%s'님의 정보를 찾을 수 없습니다.\n", del);
                break;
            }
            case 4:
                cl_print_all(cl);
                break;
            case 5: {
                if (save_to_csv(cl, csv_file))
                    printf("'%s' 파일로 저장 완료! (%d명)\n", csv_file, cl->count);
                break;
            }
            case 6: {
                if (load_from_csv(cl, pi, csv_file))
                    printf("'%s'에서 불러오기 완료! (총 %d명)\n", csv_file, cl->count);
                break;
            }
            default:
                fprintf(stderr, "잘못된 선택입니다. 1~7을 입력하세요.\n");
        }
    }

    pi_destroy(pi);
    cl_destroy(cl);
    return 0;
}
