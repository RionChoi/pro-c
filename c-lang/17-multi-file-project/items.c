/*
 * items.c — 아이템 관리 (텍스트 어드벤처)
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "items.h"

extern int rooms_get_current_room_id(void);

#define MAX_INV  10
#define MAX_NAME 32

typedef struct Item {
    const char *name;
    const char *description;
    int         room_id;
} Item;

static Item all_items[5] = {
    { "sword",   "날카로운 검",    1 },
    { "key",     "낡은 열쇠",      3 },
    { "potion",  "빨간 포션",      0 },
    { "shield",  "강철 방패",       1 },
    { "torch",   "횃불",           0 }
};

static const char *inventory[MAX_INV];
static int inv_count = 0;

void items_init(void)
{
    inv_count = 0;
}

Item *items_at_current_room(void)
{
    extern int rooms_get_current_room_id(void);
    int rid = rooms_get_current_room_id();
    for (int i = 0; i < 5; i++) {
        if (all_items[i].room_id == rid)
            return &all_items[i];
    }
    return NULL;
}

bool items_take(const char *name)
{
    Item *found = NULL;
    for (int i = 0; i < 5; i++) {
        if (strcmp(all_items[i].name, name) == 0) {
            found = &all_items[i];
            break;
        }
    }
    if (found == NULL) return false;

    /* 이미 인벤토리에 있는지 확인 */
    for (int i = 0; i < inv_count; i++) {
        if (strcmp(inventory[i], name) == 0)
            return false; /* 이미 소유 */
    }
    if (inv_count >= MAX_INV) return false;

    inventory[inv_count++] = found->name;
    return true;
}

bool items_use(const char *name)
{
    for (int i = 0; i < inv_count; i++) {
        if (strcmp(inventory[i], name) == 0)
            return true;
    }
    return false;
}

bool items_has(const char *name)
{
    for (int i = 0; i < inv_count; i++) {
        if (strcmp(inventory[i], name) == 0)
            return true;
    }
    return false;
}

void items_list_inventory(void)
{
    if (inv_count == 0) {
        printf("(인벤토리가 비어 있습니다)\n");
        return;
    }
    printf("인벤토리: ");
    for (int i = 0; i < inv_count; i++)
        printf("%s ", inventory[i]);
    printf("\n");
}

const char *items_get_name(Item *it)
{
    if (it == NULL) return "(없음)";
    return it->name;
}
