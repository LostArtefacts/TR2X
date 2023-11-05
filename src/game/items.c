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

bool Item_IsSmashable(const struct ITEM_INFO *item)
{
    return (item->object_num == O_WINDOW_1 || item->object_num == O_BELL);
}
