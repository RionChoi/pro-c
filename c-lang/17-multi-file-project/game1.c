/*
 * game1.c — 텍스트 어드벤처 (멀티파일 프로젝트)
 *
 * 파일 구성:
 *   rooms.c  — 방 데이터 및 탐색 로직
 *   items.c  — 아이템 데이터
 *   main.c   — 게임 엔진 (본 파일)
 *
 * 분할 컴파일:
 *   gcc rooms.c items.c game1.c -o adventure
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* ── 방 모듈 (rooms.c) ── */
typedef struct Room Room;
extern void   rooms_init(void);
extern Room  *rooms_get_current(void);
extern bool   rooms_move(const char *direction);
extern void   rooms_describe(void);

/* ── 아이템 모듈 (items.c) ── */
#include "items.h"

/* ── 게임 상태 ── */
static bool game_over = false;
static bool has_sword   = false;
static bool has_key     = false;
static bool door_unlocked = false;

int main(void)
{
    printf("=== 텍스트 어드벤처 — 다크 던전 ===\n");
    printf("명령: look, go <방향>, take <아이템>, use <아이템>, inventory, quit\n\n");

    rooms_init();
    items_init();

    rooms_describe();

    char command[64];
    while (!game_over) {
        printf("\n> ");
        if (fgets(command, sizeof(command), stdin) == NULL) break;
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "quit") == 0 || strcmp(command, "q") == 0) {
            printf("게임을 종료합니다.\n");
            break;
        }

        if (strcmp(command, "look") == 0) {
            rooms_describe();
            Item *items = items_at_current_room();
            if (items != NULL)
                printf("이 방에서 발견: %s\n", items_get_name(items));
            continue;
        }

        if (strncmp(command, "go ", 3) == 0) {
            const char *dir = command + 3;
            if (rooms_move(dir)) {
                rooms_describe();
            } else {
                fprintf(stderr, "해당 방향으로 갈 수 없습니다.\n");
            }
            continue;
        }

        if (strncmp(command, "take ", 5) == 0) {
            const char *item_name = command + 5;
            if (items_take(item_name)) {
                if (strcmp(item_name, "sword") == 0) has_sword = true;
                if (strcmp(item_name, "key") == 0)   has_key   = true;
                printf("'%s'을(를) 주웠습니다.\n", item_name);
            } else {
                fprintf(stderr, "해당 아이템이 여기에 없습니다.\n");
            }
            continue;
        }

        if (strncmp(command, "use ", 4) == 0) {
            const char *item_name = command + 4;
            if (items_use(item_name)) {
                if (strcmp(item_name, "key") == 0 && !door_unlocked) {
                    printf("열쇠로 문을 열었습니다! 탈출에 성공했습니다!\n");
                    door_unlocked = true;
                    game_over = true;
                } else if (strcmp(item_name, "potion") == 0) {
                    printf("체력이 회복되었습니다! 전투를 계속하세요!\n");
                } else {
                    printf("효과가 없습니다.\n");
                }
            } else {
                fprintf(stderr, "해당 아이템이 인벤토리에 없습니다.\n");
            }
            continue;
        }

        if (strcmp(command, "inventory") == 0 || strcmp(command, "inv") == 0) {
            items_list_inventory();
            continue;
        }

        fprintf(stderr, "알 수 없는 명령입니다.\n");
    }

    if (game_over)
        printf("\n=== 게임 클리어! ===\n");
    else
        printf("\n다음에 다시 만나요!\n");
    return 0;
}
