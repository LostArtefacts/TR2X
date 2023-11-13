#include "game/items.h"

#include "global/const.h"
#include "global/funcs.h"
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
    Item_RemoveActive(item_num);
    Item_RemoveDrawn(item_num);

    struct ITEM_INFO *const item = &g_Items[item_num];
    if (item == g_Lara.target) {
        g_Lara.target = NULL;
    }

    if (item_num < g_LevelItemCount) {
        item->flags |= IF_KILLED;
    } else {
        item->next_item = g_NextItemFree;
        g_NextItemFree = item_num;
    }
}

void __cdecl Item_Initialise(const int16_t item_num)
{
    struct ITEM_INFO *const item = &g_Items[item_num];
    item->anim_num = g_Objects[item->object_num].anim_idx;
    item->frame_num = g_Anims[item->anim_num].frame_base;
    item->goal_anim_state = g_Anims[item->anim_num].current_anim_state;
    item->current_anim_state = item->goal_anim_state;
    item->required_anim_state = 0;
    item->pos.x_rot = 0;
    item->pos.z_rot = 0;
    item->speed = 0;
    item->fall_speed = 0;
    item->hit_points = g_Objects[item->object_num].hit_points;
    item->timer = 0;
    item->mesh_bits = 0xFFFFFFFF;
    item->touch_bits = 0;
    item->data = NULL;

    item->active = 0;
    item->status = IS_INACTIVE;
    item->gravity = 0;
    item->hit_status = 0;
    item->collidable = 1;
    item->looked_at = 0;
    item->killed = 0;

    if ((item->flags & IF_INVISIBLE) != 0) {
        item->status = IS_INVISIBLE;
        item->flags &= ~IF_INVISIBLE;
    } else if (g_Objects[item->object_num].intelligent) {
        item->status = IS_INVISIBLE;
    }

    if (item->flags & IF_KILLED) {
        item->killed = 1;
        item->flags &= ~IF_KILLED;
    }

    if ((item->flags & IF_CODE_BITS) == IF_CODE_BITS) {
        item->flags &= ~IF_CODE_BITS;
        item->flags |= IF_REVERSE;
        Item_AddActive(item_num);
        item->status = IS_ACTIVE;
    }

    struct ROOM_INFO *const room = &g_Rooms[item->room_num];
    item->next_item = room->item_num;
    room->item_num = item_num;

    const int32_t dx = (item->pos.x - room->x) >> WALL_SHIFT;
    const int32_t dz = (item->pos.z - room->z) >> WALL_SHIFT;
    const struct FLOOR_INFO *const floor = &room->floor[dx * room->x_size + dz];
    item->floor = floor->floor << 8;

    if (g_SaveGame.bonus_flag && !g_IsDemoLevelType) {
        item->hit_points *= 2;
    }

    if (g_Objects[item->object_num].initialise != NULL) {
        g_Objects[item->object_num].initialise(item_num);
    }
}

void __cdecl Item_RemoveActive(const int16_t item_num)
{
    struct ITEM_INFO *const item = &g_Items[item_num];
    if (!item->active) {
        return;
    }

    item->active = 0;

    int16_t link_num = g_NextItemActive;
    if (link_num == item_num) {
        g_NextItemActive = item->next_active;
        return;
    }

    while (link_num != NO_ITEM) {
        if (g_Items[link_num].next_active == item_num) {
            g_Items[link_num].next_active = item->next_active;
            return;
        }
        link_num = g_Items[link_num].next_active;
    }
}

void __cdecl Item_RemoveDrawn(const int16_t item_num)
{
    const struct ITEM_INFO *const item = &g_Items[item_num];
    if (item->room_num == NO_ROOM) {
        return;
    }

    int16_t link_num = g_Rooms[item->room_num].item_num;
    if (link_num == item_num) {
        g_Rooms[item->room_num].item_num = item->next_item;
        return;
    }

    while (link_num != NO_ITEM) {
        if (g_Items[link_num].next_item == item_num) {
            g_Items[link_num].next_item = item->next_item;
            return;
        }
        link_num = g_Items[link_num].next_item;
    }
}

void __cdecl Item_AddActive(const int16_t item_num)
{
    ITEM_INFO *const item = &g_Items[item_num];
    if (g_Objects[item->object_num].control == NULL) {
        item->status = IS_INACTIVE;
        return;
    }

    if (item->active) {
        return;
    }

    item->active = 1;
    item->next_active = g_NextItemActive;
    g_NextItemActive = item_num;
}

bool Item_IsSmashable(const struct ITEM_INFO *item)
{
    return (item->object_num == O_WINDOW_1 || item->object_num == O_BELL);
}
