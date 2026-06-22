#include <stdio.h>

#define PERM_READ  (1U << 0)
#define PERM_WRITE (1U << 1)
#define PERM_EXEC  (1U << 2)

void print_permissions(unsigned int flags) {
    printf("현재 권한: ");
    printf("%c", (flags & PERM_READ) != 0U ? 'R' : '-');
    printf("%c", (flags & PERM_WRITE) != 0U ? 'W' : '-');
    printf("%c\n", (flags & PERM_EXEC) != 0U ? 'X' : '-');
}

int main(void) {
    unsigned int flags = 0U;
    int menu = 0;

    while (1) {
        print_permissions(flags);
        printf("1) READ 토글 2) WRITE 토글 3) EXEC 토글 4) 종료\n선택: ");
        if (scanf("%d", &menu) != 1) {
            fprintf(stderr, "입력 오류\n");
            return 1;
        }

        if (menu == 1) {
            flags ^= PERM_READ;
        } else if (menu == 2) {
            flags ^= PERM_WRITE;
        } else if (menu == 3) {
            flags ^= PERM_EXEC;
        } else if (menu == 4) {
            break;
        } else {
            fprintf(stderr, "잘못된 메뉴입니다.\n");
        }
    }

    printf("프로그램을 종료합니다.\n");
    return 0;
}
