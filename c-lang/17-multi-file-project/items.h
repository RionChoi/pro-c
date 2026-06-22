/*
 * items.h — 아이템 모듈 헤더
 */

#ifndef ITEMS_H
#define ITEMS_H

#include <stdbool.h>

typedef struct Item Item;
extern void   items_init(void);
extern Item  *items_at_current_room(void);
extern bool   items_take(const char *name);
extern bool   items_use(const char *name);
extern bool   items_has(const char *name);
extern void   items_list_inventory(void);
extern const char *items_get_name(Item *it);

#endif /* ITEMS_H */
