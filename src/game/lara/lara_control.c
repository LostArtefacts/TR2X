#include "game/lara/lara_control.h"

#include "game/lara/lara_look.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"

void __cdecl Lara_HandleAboveWater(
    struct ITEM_INFO *const item, struct COLL_INFO *const coll)
{
    coll->old.x = item->pos.x;
    coll->old.y = item->pos.y;
    coll->old.z = item->pos.z;
    coll->old_anim_state = item->current_anim_state;
    coll->old_anim_num = item->anim_num;
    coll->old_frame_num = item->frame_num;
    coll->radius = LARA_RADIUS;
    coll->trigger = NULL;

    coll->slopes_are_walls = 0;
    coll->slopes_are_pits = 0;
    coll->lava_is_pit = 0;
    coll->enable_baddie_push = 1;
    coll->enable_spaz = 1;

    if ((g_Input & IN_LOOK) && !g_Lara.extra_anim && g_Lara.look) {
        Lara_LookLeftRight();
    } else {
        Lara_ResetLook();
    }

    g_Lara.look = 1;

    if (g_Lara.skidoo != NO_ITEM) {
        if (g_Items[g_Lara.skidoo].object_num == O_SKIDOO_FAST) {
            if (SkidooControl()) {
                return;
            }
        } else {
            LaraGun();
            return;
        }
    }

    if (g_Lara.extra_anim) {
        g_ExtraControlRoutines[item->current_anim_state](item, coll);
    } else {
        g_LaraControlRoutines[item->current_anim_state](item, coll);
    }

    if (item->pos.z_rot < -LARA_LEAN_UNDO) {
        item->pos.z_rot += LARA_LEAN_UNDO;
    } else if (item->pos.z_rot > LARA_LEAN_UNDO) {
        item->pos.z_rot -= LARA_LEAN_UNDO;
    } else {
        item->pos.z_rot = 0;
    }

    if (g_Lara.turn_rate < -LARA_TURN_UNDO) {
        g_Lara.turn_rate += LARA_TURN_UNDO;
    } else if (g_Lara.turn_rate > LARA_TURN_UNDO) {
        g_Lara.turn_rate -= LARA_TURN_UNDO;
    } else {
        g_Lara.turn_rate = 0;
    }
    item->pos.y_rot += g_Lara.turn_rate;

    Lara_Animate(item);

    if (!g_Lara.extra_anim) {
        Lara_BaddieCollision(item, coll);
        if (g_Lara.skidoo == NO_ITEM) {
            g_LaraCollisionRoutines[item->current_anim_state](item, coll);
        }
    }

    Item_UpdateRoom(item, -LARA_HEIGHT / 2);
    LaraGun();
    Room_TestTriggers(coll->trigger, 0);
}
