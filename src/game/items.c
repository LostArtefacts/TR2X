#include "game/items.h"

bool Item_IsSmashable(const struct ITEM_INFO *item)
{
    return (item->object_num == O_WINDOW_1 || item->object_num == O_BELL);
}
