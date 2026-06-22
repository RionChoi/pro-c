/*
 * rooms.c — 방 데이터 및 탐색 (텍스트 어드벤처)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "rooms.h"

typedef struct Room {
    const char *name;
    const char *description;
    const char *exits[4];   /* north, south, east, west */
    int         room_id;
} Room;

static Room rooms[4];

static void room_set(int id, const char *name, const char *desc,
                     const char *n, const char *s, const char *e, const char *w)
{
    rooms[id].name        = name;
    rooms[id].description = desc;
    rooms[id].exits[0]    = n;
    rooms[id].exits[1]    = s;
    rooms[id].exits[2]    = e;
    rooms[id].exits[3]    = w;
    rooms[id].room_id     = id;
}

void rooms_init(void)
{
    room_set(0, "입구",      "어두운 던전 입구입니다. 북쪽으로 진행하세요.",
             "1",  NULL,  NULL,  NULL);
    room_set(1, "복도",      "긴 복도입니다. 북동쪽에 문이 보입니다.",
             "2",  "0",   "3",   NULL);
    room_set(2, "보스 방",   "흉계한 보스가 기다리고 있습니다! 열쇠가 필요합니다.",
             NULL, "1",   NULL,   NULL);
    room_set(3, "보물 방",   "보물 상자가 있습니다! 열쇠가 필요합니다.",
             NULL, NULL,   NULL,   "1");
}

static int current_room = 0;

Room *rooms_get_current(void)
{
    return &rooms[current_room];
}

bool rooms_move(const char *direction)
{
    Room *r = &rooms[current_room];

    if (strcmp(direction, "north") == 0 && r->exits[0] != NULL) {
        current_room = atoi(r->exits[0]);
        return true;
    }
    if (strcmp(direction, "south") == 0 && r->exits[1] != NULL) {
        current_room = atoi(r->exits[1]);
        return true;
    }
    if (strcmp(direction, "east") == 0 && r->exits[2] != NULL) {
        current_room = atoi(r->exits[2]);
        return true;
    }
    if (strcmp(direction, "west") == 0 && r->exits[3] != NULL) {
        current_room = atoi(r->exits[3]);
        return true;
    }
    return false;
}

void rooms_describe(void)
{
    Room *r = &rooms[current_room];
    printf("\n【%s】\n", r->name);
    printf("%s\n", r->description);
    printf("이동 가능: ");
    bool any = false;
    for (int i = 0; i < 4; i++) {
        if (r->exits[i] != NULL) {
            const char *dirs[] = { "north", "south", "east", "west" };
            printf("%s ", dirs[i]);
            any = true;
        }
    }
    if (!any) printf("없음");
    printf("\n");
}

int rooms_get_current_room_id(void)
{
    return current_room;
}
