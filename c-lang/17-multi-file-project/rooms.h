/*
 * rooms.h — 방 모듈 헤더
 */

#ifndef ROOMS_H
#define ROOMS_H

#include <stdbool.h>

typedef struct Room Room;
extern void   rooms_init(void);
extern Room  *rooms_get_current(void);
extern bool   rooms_move(const char *direction);
extern void   rooms_describe(void);
extern int    rooms_get_current_room_id(void);

#endif /* ROOMS_H */
