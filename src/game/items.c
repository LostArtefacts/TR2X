#include "game/items.h"

#include "global/const.h"
#include "global/vars.h"

#include <assert.h>

void __cdecl Item_InitialiseArray(const int32_t num_items)
{
    assert(num_items > 0);
    g_NextItemFree = g_LevelItemCount;
    g_PrevItemActive = NO_ITEM;
    g_NextItemActive = NO_ITEM;
    for (int i = g_NextItemFree; i < num_items - 1; i++) {
        struct ITEM_INFO *const item = &g_Items[i];
        item->active = 1;
        item->next_item = i + 1;
    }
    g_Items[num_items - 1].next_item = NO_ITEM;
}

int16_t __cdecl Item_Create(void)
{
    const int16_t result = g_NextItemFree;
    if (result != NO_ITEM) {
        g_Items[result].flags = 0;
        g_NextItemFree = g_Items[result].next_item;
    }
    return result;
}

void __cdecl Item_Kill(const int16_t item_num)
{
    struct ITEM_INFO *const item = &g_Items[item_num];
    item->active = 0;

    int16_t link_num = g_NextItemActive;
    if (link_num == item_num) {
        g_NextItemActive = item->next_active;
    } else {
        while (link_num != NO_ITEM) {
            if (g_Items[link_num].next_active == item_num) {
                g_Items[link_num].next_active = item->next_active;
                break;
            }
            link_num = g_Items[link_num].next_active;
        }
    }

    if (item->room_num != NO_ROOM) {
        link_num = g_Rooms[item->room_num].item_num;
        if (link_num == item_num) {
            g_Rooms[item->room_num].item_num = item->next_item;
        } else {
            while (link_num != NO_ITEM) {
                if (g_Items[link_num].next_item == item_num) {
                    g_Items[link_num].next_item = item->next_item;
                    break;
                }
                link_num = g_Items[link_num].next_item;
            }
        }
    }

    if (item == g_Lara.target) {
        g_Lara.target = NULL;
    }

    if (item_num < g_LevelItemCount) {
        item->flags |= IF_KILLED_ITEM;
    } else {
        item->next_item = g_NextItemFree;
        g_NextItemFree = item_num;
    }
}

bool Item_IsSmashable(const struct ITEM_INFO *item)
{
    return (item->object_num == O_WINDOW_1 || item->object_num == O_BELL);
}
