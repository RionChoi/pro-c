/*
 * game1.c — 최종 완성版: 개인 주소록 관리 시스템
 *
 * homework.c + homework2.c의 모든 기능을 통합한 완성版:
 * - 연결 리스트 기반 연락처 저장
 * - 해시 테이블 기반 전화번호 중복 체크
 * - CSV 파일 저장 / 불러오기
 * - 이름순 정렬
 * - 페이지 네비게이션 (한 화면에 5명씩)
 * - 그룹 필터링 (전체/가족/친구/회사)
 *
 * 사용법: contacts [CSV파일명]
 *   기본 CSV 파일: contacts.csv
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define NAME_SIZE    64
#define PHONE_SIZE   32
#define EMAIL_SIZE   64
#define GROUP_SIZE   32
#define TABLE_SIZE   63
#define LINE_SIZE   512
#define PAGE_SIZE    5
#define DEFAULT_FN  "contacts.csv"

/* ── Group type ── */
typedef enum {
    GROUP_NONE = 0,
    GROUP_FAMILY,
    GROUP_FRIEND,
    GROUP_WORK,
    GROUP_COUNT
} Group;

static const char *group_names[GROUP_COUNT] = {
    "없음", "가족", "친구", "회사"
};

/* ── Contact ── */
typedef struct Contact {
    char            name[NAME_SIZE];
    char            phone[PHONE_SIZE];
    char            email[EMAIL_SIZE];
    Group           group;
    struct Contact *next;
} Contact;

typedef struct {
    Contact *head;
    int      count;
} ContactList;

/* ── Hash Table for phone index ── */
typedef struct HNode {
    char           phone[PHONE_SIZE];
    Contact       *contact;
    struct HNode  *next;
} HNode;

typedef struct {
    HNode **buckets;
    int     size;
} PhoneIndex;

/* ── PhoneIndex implementation ── */
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
            HNode *t = *pp;
            *pp = t->next;
            free(t);
            return;
        }
        pp = &(*pp)->next;
    }
}

/* ── ContactList implementation ── */
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

static Contact **cl_to_array(const ContactList *cl, int *out_n)
{
    *out_n = cl->count;
    if (cl->count == 0) return NULL;
    Contact **arr = malloc((size_t)cl->count * sizeof(Contact *));
    if (arr == NULL) return NULL;
    int i = 0;
    for (Contact *c = cl->head; c != NULL; c = c->next)
        arr[i++] = c;
    return arr;
}

static int cmp_name(const void *a, const void *b)
{
    return strcmp((*(Contact **)a)->name, (*(Contact **)b)->name);
}

static void cl_sort_by_name(ContactList *cl)
{
    int n;
    Contact **arr = cl_to_array(cl, &n);
    if (arr == NULL) return;

    qsort(arr, (size_t)n, sizeof(Contact *), cmp_name);

    cl->head = NULL;
    for (int i = n - 1; i >= 0; i--) {
        arr[i]->next = cl->head;
        cl->head = arr[i];
    }
    free(arr);
}

static Contact *cl_find_by_name(const ContactList *cl, const char *name)
{
    for (Contact *c = cl->head; c != NULL; c = c->next)
        if (strcmp(c->name, name) == 0) return c;
    return NULL;
}

static Contact *cl_find_by_phone(const ContactList *cl, const char *phone)
{
    for (Contact *c = cl->head; c != NULL; c = c->next)
        if (strcmp(c->phone, phone) == 0) return c;
    return NULL;
}

static bool cl_add(ContactList *cl, PhoneIndex *pi,
                   const char *name, const char *phone, const char *email, Group grp)
{
    if (cl == NULL || name == NULL || phone == NULL) return false;

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
    node->group = grp;
    node->next = cl->head;
    cl->head = node;
    cl->count++;

    pi_insert(pi, node);
    return true;
}

static bool cl_remove_by_name(ContactList *cl, PhoneIndex *pi, const char *name)
{
    if (cl == NULL) return false;
    Contact **pp = &cl->head;
    while (*pp != NULL) {
        if (strcmp((*pp)->name, name) == 0) {
            pi_remove(pi, (*pp)->phone);
            Contact *t = *pp;
            *pp = t->next;
            free(t);
            cl->count--;
            return true;
        }
        pp = &(*pp)->next;
    }
    return false;
}

static void cl_print_page(const ContactList *cl, int page, Group filter)
{
    int n;
    Contact **arr = cl_to_array(cl, &n);
    if (arr == NULL) return;

    qsort(arr, (size_t)n, sizeof(Contact *), cmp_name);

    /* filter */
    int *idx_arr = NULL;
    int filtered_n = 0;
    if (filter == GROUP_NONE) {
        filtered_n = n;
        idx_arr = malloc((size_t)n * sizeof(int));
        for (int i = 0; i < n; i++) idx_arr[i] = i;
    } else {
        idx_arr = malloc((size_t)n * sizeof(int));
        for (int i = 0; i < n; i++)
            if (arr[i]->group == filter) idx_arr[filtered_n++] = i;
    }

    int total_pages = (filtered_n + PAGE_SIZE - 1) / PAGE_SIZE;
    if (page < 1) page = 1;
    if (page > total_pages && total_pages > 0) page = total_pages;

    const char *grp_label = (filter == GROUP_NONE) ? "전체" : group_names[filter];
    printf("\n═══════ 주소록: %s (총 %d명, %d/%d페이지) ═══════\n",
           grp_label, filtered_n, page, total_pages);
    printf("%-4s %-18s %-15s %-12s %-20s\n",
           "No.", "이름", "전화번호", "그룹", "이메일");
    printf("---------------------------------------------------------------------\n");

    int start = (page - 1) * PAGE_SIZE;
    int end = start + PAGE_SIZE;
    for (int i = start; i < end && i < filtered_n; i++) {
        Contact *c = arr[idx_arr[i]];
        printf("%4d  %-18s %-15s %-12s %-20s\n",
               i + 1, c->name, c->phone,
               group_names[c->group], c->email);
    }
    printf("=====================================================================\n");
    printf("  ◀ 이전(p/P)  │  페이지 %d/%d  │  다음(n/N) ▶\n", page, total_pages);
    printf("  정렬(s/S)     │  필터(f/F)     │  메인 메뉴(q/Q)\n", page, total_pages);

    free(idx_arr);
    free(arr);
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
    fprintf(fp, "이름,전화번호,이메일,그룹\n");
    for (Contact *c = cl->head; c != NULL; c = c->next)
        fprintf(fp, "%s,%s,%s,%s\n", c->name, c->phone, c->email, group_names[c->group]);
    fclose(fp);
    return true;
}

static bool load_from_csv(ContactList *cl, PhoneIndex *pi, const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) return false;

    char line[LINE_SIZE];
    if (fgets(line, sizeof(line), fp) == NULL) { fclose(fp); return true; }

    while (fgets(line, sizeof(line), fp) != NULL) {
        strip_newline(line);
        if (line[0] == '\0') continue;

        char name[NAME_SIZE] = {0};
        char phone[PHONE_SIZE] = {0};
        char email[EMAIL_SIZE] = {0};
        char grp_str[GROUP_SIZE] = {0};

        int commas[3] = { -1, -1, -1 };
        int cnt = 0;
        for (int i = 0; line[i] && cnt < 3; i++)
            if (line[i] == ',') commas[cnt++] = i;

        if (commas[0] < 0 || commas[1] < 0) continue;

        strncpy(name, line, (size_t)commas[0] < NAME_SIZE - 1 ? (size_t)commas[0] : NAME_SIZE - 1);
        name[NAME_SIZE - 1] = '\0';

        size_t ph_len = (size_t)(commas[1] - commas[0] - 1);
        strncpy(phone, line + commas[0] + 1, ph_len < PHONE_SIZE - 1 ? ph_len : PHONE_SIZE - 1);
        phone[PHONE_SIZE - 1] = '\0';

        int email_start = commas[1] + 1;
        int email_end = commas[2] > 0 ? commas[2] : (int)strlen(line);
        size_t em_len = (size_t)(email_end - email_start);
        strncpy(email, line + email_start, em_len < EMAIL_SIZE - 1 ? em_len : EMAIL_SIZE - 1);
        email[EMAIL_SIZE - 1] = '\0';

        Group grp = GROUP_NONE;
        if (commas[2] > 0) {
            strncpy(grp_str, line + commas[2] + 1, GROUP_SIZE - 1);
            grp_str[GROUP_SIZE - 1] = '\0';
            for (int i = 1; i < GROUP_COUNT; i++)
                if (strcmp(grp_str, group_names[i]) == 0) { grp = i; break; }
        }

        if (!pi_has(pi, phone))
            cl_add(cl, pi, name, phone, email, grp);
    }

    fclose(fp);
    return true;
}

/* ── Helpers ── */
static void trim_newline(char *s) { s[strcspn(s, "\n")] = '\0'; }

static Group select_group(void)
{
    printf("그룹을 선택하세요:\n");
    for (int i = 1; i < GROUP_COUNT; i++)
        printf("  %d. %s\n", i, group_names[i]);
    printf("선택 (기본: 없음): ");

    char buf[16];
    if (fgets(buf, sizeof(buf), stdin) == NULL) return GROUP_NONE;
    trim_newline(buf);
    int v = atoi(buf);
    if (v >= 1 && v < GROUP_COUNT) return (Group)v;
    return GROUP_NONE;
}

static void read_line(const char *prompt, char *buf, int size)
{
    printf("%s", prompt);
    if (fgets(buf, size, stdin) != NULL)
        trim_newline(buf);
}

static void print_banner(void)
{
    printf("\n");
    printf("  ╔═══════════════════════════════════════════════╗\n");
    printf("  ║         📒  개인 주소록 관리 시스템  📒        ║\n");
    printf("  ║          C Language Final Project             ║\n");
    printf("  ╚═══════════════════════════════════════════════╝\n");
}

static void print_main_menu(void)
{
    printf("\n────────── 메인 메뉴 ──────────\n");
    printf("  1. 연락처 추가\n");
    printf("  2. 연락처 검색\n");
    printf("  3. 연락처 삭제\n");
    printf("  4. 주소록 목록 (페이지)\n");
    printf("  5. 이름순 정렬\n");
    printf("  6. CSV 저장\n");
    printf("  7. CSV 불러오기\n");
    printf("  8. 종료\n");
    printf("──────────────────────────────\n");
    printf("선택: ");
}

int main(int argc, char *argv[])
{
    ContactList *cl = cl_create();
    PhoneIndex  *pi = pi_create(TABLE_SIZE);
    if (cl == NULL || pi == NULL) return 1;

    char csv_file[NAME_SIZE];
    strncpy(csv_file, DEFAULT_FN, NAME_SIZE - 1);
    csv_file[NAME_SIZE - 1] = '\0';
    if (argc >= 2) {
        strncpy(csv_file, argv[1], NAME_SIZE - 1);
        csv_file[NAME_SIZE - 1] = '\0';
    }

    /* CSV 파일이 있으면 자동 불러오기 */
    load_from_csv(cl, pi, csv_file);

    char buf[256];
    int current_page = 1;
    Group current_filter = GROUP_NONE;

    print_banner();
    printf("  데이터 파일: %s\n", csv_file);
    printf("  현재 주소록: %d명\n", cl->count);

    while (true) {
        print_main_menu();
        if (fgets(buf, sizeof(buf), stdin) == NULL) break;
        int choice = atoi(buf);

        if (choice == 8) {
            printf("주소록을 종료합니다.\n");
            break;
        }

        switch (choice) {
            case 1: {
                char name[NAME_SIZE], phone[PHONE_SIZE], email[EMAIL_SIZE];
                read_line("  이름: ", name, sizeof(name));
                read_line("  전화번호: ", phone, sizeof(phone));
                read_line("  이메일: ", email, sizeof(email));
                Group grp = select_group();
                if (name[0] && phone[0]) {
                    if (cl_add(cl, pi, name, phone, email, grp)) {
                        printf("  ✓ '%s' 추가 완료! (총 %d명)\n", name, cl->count);
                    }
                }
                break;
            }
            case 2: {
                char search[NAME_SIZE];
                read_line("  검색할 이름: ", search, sizeof(search));
                Contact *found = cl_find_by_name(cl, search);
                if (found) {
                    printf("\n  ┌─────────────────────────────┐\n");
                    printf("  │ 이름:   %-20s │\n", found->name);
                    printf("  │ 전화:   %-20s │\n", found->phone);
                    printf("  │ 이메일: %-20s │\n", found->email);
                    printf("  │ 그룹:   %-20s │\n", group_names[found->group]);
                    printf("  └─────────────────────────────┘\n");
                } else {
                    printf("  '%s'님의 정보를 찾을 수 없습니다.\n", search);
                }
                break;
            }
            case 3: {
                char del[NAME_SIZE];
                read_line("  삭제할 이름: ", del, sizeof(del));
                if (cl_remove_by_name(cl, pi, del))
                    printf("  ✓ '%s'님을 삭제했습니다. (총 %d명)\n", del, cl->count);
                else
                    fprintf(stderr, "  ✗ '%s'님의 정보를 찾을 수 없습니다.\n", del);
                break;
            }
            case 4:
                cl_print_page(cl, current_page, current_filter);
                printf("\n  명령: p/P(이전)  n/N(다음)  f/F(필터)  q/Q(메인)\n");
                printf("  > ");
                if (fgets(buf, sizeof(buf), stdin) != NULL) {
                    trim_newline(buf);
                    if (strcmp(buf, "n") == 0 || strcmp(buf, "N") == 0) current_page++;
                    else if (strcmp(buf, "p") == 0 || strcmp(buf, "P") == 0) current_page--;
                    else if (strcmp(buf, "f") == 0 || strcmp(buf, "F") == 0) {
                        printf("  필터: 0(전체) 1(가족) 2(친구) 3(회사) > ");
                        if (fgets(buf, sizeof(buf), stdin) != NULL) {
                            trim_newline(buf);
                            current_filter = (Group)atoi(buf);
                            if (current_filter >= GROUP_COUNT) current_filter = GROUP_NONE;
                            current_page = 1;
                        }
                    }
                }
                break;
            case 5:
                cl_sort_by_name((ContactList *)cl);
                printf("  ✓ 이름순으로 정렬 완료!\n");
                current_page = 1;
                break;
            case 6:
                if (save_to_csv(cl, csv_file))
                    printf("  ✓ '%s' 파일로 저장 완료! (%d명)\n", csv_file, cl->count);
                break;
            case 7:
                if (load_from_csv(cl, pi, csv_file))
                    printf("  ✓ '%s'에서 불러오기 완료! (총 %d명)\n", csv_file, cl->count);
                else
                    fprintf(stderr, "  ✗ 파일을 열 수 없습니다: %s\n", csv_file);
                break;
            default:
                fprintf(stderr, "  ✗ 잘못된 선택입니다. 1~8을 입력하세요.\n");
        }
    }

    pi_destroy(pi);
    cl_destroy(cl);
    return 0;
}
