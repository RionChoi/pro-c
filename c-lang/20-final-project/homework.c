/*
 * homework.c — 주소록 핵심 기능 (연결 리스트 기반)
 *
 * 기능: 추가, 검색, 삭제, 전체 목록
 * 데이터는 메모리에만 유지 (file I/O 없음)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NAME_SIZE  64
#define PHONE_SIZE 32
#define EMAIL_SIZE 64

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

/* ── ContactList operations ── */

static ContactList *cl_create(void)
{
    ContactList *cl = malloc(sizeof(ContactList));
    if (cl == NULL) {
        fprintf(stderr, "주소록 생성 실패\n");
        return NULL;
    }
    cl->head  = NULL;
    cl->count = 0;
    return cl;
}

static void cl_destroy(ContactList *cl)
{
    if (cl == NULL) return;
    Contact *curr = cl->head;
    while (curr != NULL) {
        Contact *next = curr->next;
        free(curr);
        curr = next;
    }
    free(cl);
}

static bool cl_is_empty(const ContactList *cl)
{
    return cl == NULL || cl->head == NULL;
}

static Contact *cl_find_by_name(const ContactList *cl, const char *name)
{
    for (Contact *c = cl->head; c != NULL; c = c->next) {
        if (strcmp(c->name, name) == 0)
            return c;
    }
    return NULL;
}

static Contact *cl_find_by_phone(const ContactList *cl, const char *phone)
{
    for (Contact *c = cl->head; c != NULL; c = c->next) {
        if (strcmp(c->phone, phone) == 0)
            return c;
    }
    return NULL;
}

static bool cl_add(ContactList *cl, const char *name, const char *phone, const char *email)
{
    if (cl == NULL || name == NULL || phone == NULL) return false;

    /* 중복 전화번호 체크 */
    (void)cl_find_by_phone; /* suppress warning if not used below */
    if (cl_find_by_phone(cl, phone) != NULL) {
        fprintf(stderr, "오류: 전화번호 '%s'는 이미 등록되어 있습니다.\n", phone);
        return false;
    }

    Contact *node = malloc(sizeof(Contact));
    if (node == NULL) {
        fprintf(stderr, "메모리 할당 실패\n");
        return false;
    }

    strncpy(node->name, name, NAME_SIZE - 1);
    node->name[NAME_SIZE - 1] = '\0';
    strncpy(node->phone, phone, PHONE_SIZE - 1);
    node->phone[PHONE_SIZE - 1] = '\0';
    strncpy(node->email, email, EMAIL_SIZE - 1);
    node->email[EMAIL_SIZE - 1] = '\0';
    node->next = cl->head;
    cl->head = node;
    cl->count++;
    return true;
}

static bool cl_remove_by_name(ContactList *cl, const char *name)
{
    if (cl == NULL || cl_is_empty(cl)) return false;

    Contact **pp = &cl->head;
    while (*pp != NULL) {
        if (strcmp((*pp)->name, name) == 0) {
            Contact *to_free = *pp;
            *pp = (*pp)->next;
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
    if (cl == NULL || cl_is_empty(cl)) {
        printf("주소록이 비어 있습니다.\n");
        return;
    }

    printf("\n═══════ 주소록 (총 %d명) ═══════\n", cl->count);
    printf("%-20s %-15s %-25s\n", "이름", "전화번호", "이메일");
    printf("------------------------------------------------\n");

    int idx = 1;
    for (Contact *c = cl->head; c != NULL; c = c->next, idx++) {
        printf("%2d. %-18s %-15s %-25s\n", idx, c->name, c->phone, c->email);
    }
    printf("================================================\n");
}

static void print_contact(const Contact *c)
{
    if (c == NULL) return;
    printf("  이름:   %s\n", c->name);
    printf("  전화:   %s\n", c->phone);
    printf("  이메일: %s\n", c->email);
}

/* ── Interactive helpers ── */

static void read_line(const char *prompt, char *buf, int size)
{
    printf("%s", prompt);
    if (fgets(buf, size, stdin) != NULL) {
        buf[strcspn(buf, "\n")] = '\0';
    }
    if (buf[0] == '\0') {
        fprintf(stderr, "입력 오류: 빈 문자열은 사용할 수 없습니다.\n");
    }
}

static void menu(void)
{
    printf("\n═══════ 주소록 메뉴 ═══════\n");
    printf("  1. 추가\n");
    printf("  2. 검색 (이름)\n");
    printf("  3. 삭제\n");
    printf("  4. 전체 목록\n");
    printf("  5. 종료\n");
    printf("선택: ");
}

int main(void)
{
    ContactList *cl = cl_create();
    if (cl == NULL) return 1;

    char buf[256];

    printf("=== 주소록 관리 시스템 (연결 리스트) ===\n");

    while (true) {
        menu();
        if (fgets(buf, sizeof(buf), stdin) == NULL) break;
        int choice = atoi(buf);

        if (choice == 5) {
            printf("종료합니다.\n");
            break;
        }

        switch (choice) {
            case 1: {
                char name[NAME_SIZE];
                char phone[PHONE_SIZE];
                char email[EMAIL_SIZE];
                read_line("이름: ", name, sizeof(name));
                read_line("전화번호: ", phone, sizeof(phone));
                read_line("이메일: ", email, sizeof(email));
                if (name[0] && phone[0]) {
                    if (cl_add(cl, name, phone, email))
                        printf("'%s' 추가 완료!\n", name);
                    else
                        fprintf(stderr, "추가 실패\n");
                }
                break;
            }
            case 2: {
                char search[NAME_SIZE];
                read_line("검색할 이름: ", search, sizeof(search));
                Contact *found = cl_find_by_name(cl, search);
                if (found)
                    print_contact(found);
                else
                    printf("'%s'님의 정보를 찾을 수 없습니다.\n", search);
                break;
            }
            case 3: {
                char del[NAME_SIZE];
                read_line("삭제할 이름: ", del, sizeof(del));
                if (cl_remove_by_name(cl, del))
                    printf("'%s'님을 삭제했습니다.\n", del);
                else
                    fprintf(stderr, "'%s'님의 정보를 찾을 수 없습니다.\n", del);
                break;
            }
            case 4:
                cl_print_all(cl);
                break;
            default:
                fprintf(stderr, "잘못된 선택입니다. 1~5를 입력하세요.\n");
        }
    }

    cl_destroy(cl);
    return 0;
}
