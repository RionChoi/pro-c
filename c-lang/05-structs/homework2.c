/*
 * 세션 05 — 과제 2: 주소록 관리 시스템
 *
 * 기능:
 *   - 연락처(이름, 전화번호, 이메일)를 구조체로 관리
 *   - 메뉴 기반: 추가 / 검색 / 전체 출력 / 삭제 / 종료
 *   - 최대 10명까지 관리
 */

#include <stdio.h>
#include <string.h>

#define MAX_CONTACTS 10
#define NAME_SIZE    64
#define PHONE_SIZE   20
#define EMAIL_SIZE   64

struct Contact {
    char name[NAME_SIZE];
    char phone[PHONE_SIZE];
    char email[EMAIL_SIZE];
    int  active;  /* 1: 사용 중, 0: 삭제됨 */
};

static struct Contact contacts[MAX_CONTACTS];
static int contact_count = 0;

/* 연락처를 추가한다 */
void add_contact(void) {
    if (contact_count >= MAX_CONTACTS) {
        fprintf(stderr, "주소록이 가득 찼습니다 (최대 %d명).\n", MAX_CONTACTS);
        return;
    }

    struct Contact *c = &contacts[contact_count];

    printf("\n[연락처 추가]\n");
    printf("  이름: ");
    if (fgets(c->name, NAME_SIZE, stdin) == NULL) {
        fprintf(stderr, "입력 오류\n");
        return;
    }
    c->name[strcspn(c->name, "\n")] = '\0';

    printf("  전화번호: ");
    if (fgets(c->phone, PHONE_SIZE, stdin) == NULL) {
        fprintf(stderr, "입력 오류\n");
        return;
    }
    c->phone[strcspn(c->phone, "\n")] = '\0';

    printf("  이메일: ");
    if (fgets(c->email, EMAIL_SIZE, stdin) == NULL) {
        fprintf(stderr, "입력 오류\n");
        return;
    }
    c->email[strcspn(c->email, "\n")] = '\0';

    c->active = 1;
    contact_count++;
    printf("  → '%s' 등록 완료!\n", c->name);
}

/* 이름으로 연락처를 검색한다 */
void search_contact(void) {
    char query[NAME_SIZE];
    printf("\n검색할 이름: ");
    if (fgets(query, NAME_SIZE, stdin) == NULL) {
        fprintf(stderr, "입력 오류\n");
        return;
    }
    query[strcspn(query, "\n")] = '\0';

    int found = 0;
    for (int i = 0; i < contact_count; i++) {
        if (contacts[i].active && strstr(contacts[i].name, query) != NULL) {
            printf("  [%d] %s | %s | %s\n",
                   i + 1, contacts[i].name, contacts[i].phone, contacts[i].email);
            found = 1;
        }
    }

    if (!found) {
        printf("  '%s'에 해당하는 연락처를 찾을 수 없습니다.\n", query);
    }
}

/* 전체 연락처를 출력한다 */
void list_contacts(void) {
    printf("\n=== 주소록 (%d명) ===\n", contact_count);
    int displayed = 0;
    for (int i = 0; i < contact_count; i++) {
        if (contacts[i].active) {
            printf("  [%d] %s | %s | %s\n",
                   i + 1, contacts[i].name, contacts[i].phone, contacts[i].email);
            displayed++;
        }
    }
    if (displayed == 0) {
        printf("  (비어 있음)\n");
    }
}

/* 번호로 연락처를 삭제한다 */
void delete_contact(void) {
    int num;
    printf("\n삭제할 번호: ");
    if (scanf("%d", &num) != 1) {
        fprintf(stderr, "입력 오류\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    if (num < 1 || num > contact_count || !contacts[num - 1].active) {
        fprintf(stderr, "유효하지 않은 번호입니다.\n");
        return;
    }

    contacts[num - 1].active = 0;
    printf("  → '%s' 삭제 완료.\n", contacts[num - 1].name);
}

/* 메뉴를 출력한다 */
void print_menu(void) {
    printf("\n--- 주소록 메뉴 ---\n");
    printf("  1. 연락처 추가\n");
    printf("  2. 연락처 검색\n");
    printf("  3. 전체 목록\n");
    printf("  4. 연락처 삭제\n");
    printf("  5. 종료\n");
    printf("선택: ");
}

int main(void) {
    int choice;

    printf("=== 주소록 관리 시스템 ===\n");

    while (1) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "숫자를 입력하세요.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1: add_contact();    break;
            case 2: search_contact(); break;
            case 3: list_contacts();  break;
            case 4: delete_contact(); break;
            case 5:
                printf("\n주소록을 종료합니다. 안녕히가세요!\n");
                return 0;
            default:
                fprintf(stderr, "1~5 사이의 번호를 선택하세요.\n");
                break;
        }
    }

    return 0;
}
