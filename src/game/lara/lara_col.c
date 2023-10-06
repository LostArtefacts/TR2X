#include "game/lara/lara_col.h"

#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

void __cdecl Lara_Col_Walk(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    item->gravity = 0;
    item->fall_speed = 0;
    g_Lara.move_angle = item->pos.y_rot;
    coll->slopes_are_pits = 1;
    coll->slopes_are_walls = 1;
    coll->lava_is_pit = 1;
    coll->bad_pos = STEPUP_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    Lara_GetLaraCollisionInfo(item, coll);

    if (Lara_HitCeiling(item, coll)) {
        return;
    }
    if (Lara_TestVault(item, coll)) {
        return;
    }

    if (Lara_DeflectEdge(item, coll)) {
        if (item->frame_num >= 29 && item->frame_num <= 47) {
            item->anim_num = LA_STOP_RIGHT;
            item->frame_num = g_Anims[LA_STOP_RIGHT].frame_base;
        } else if (
            (item->frame_num >= 22 && item->frame_num <= 28)
            || (item->frame_num >= 48 && item->frame_num <= 57)) {
            item->anim_num = LA_STOP_LEFT;
            item->frame_num = g_Anims[LA_STOP_LEFT].frame_base;
        } else {
            Lara_CollideStop(item, coll);
        }
    }

    if (Lara_Fallen(item, coll)) {
        return;
    }

    if (coll->side_mid.floor > STEP_L / 2) {
        if (item->frame_num >= 28 && item->frame_num <= 45) {
            item->anim_num = LA_WALK_STEP_DOWN_RIGHT;
            item->frame_num = g_Anims[LA_WALK_STEP_DOWN_RIGHT].frame_base;
        } else {
            item->anim_num = LA_WALK_STEP_DOWN_LEFT;
            item->frame_num = g_Anims[LA_WALK_STEP_DOWN_LEFT].frame_base;
        }
    }

    if (coll->side_mid.floor >= -STEPUP_HEIGHT
        && coll->side_mid.floor < -STEP_L / 2) {
        if (item->frame_num >= 27 && item->frame_num <= 44) {
            item->anim_num = LA_WALK_STEP_UP_RIGHT;
            item->frame_num = g_Anims[LA_WALK_STEP_UP_RIGHT].frame_base;
        } else {
            item->anim_num = LA_WALK_STEP_UP_LEFT;
            item->frame_num = g_Anims[LA_WALK_STEP_UP_LEFT].frame_base;
        }
    }

    if (Lara_TestSlide(item, coll)) {
        return;
    }

    item->pos.y += coll->side_mid.floor;
}

void __cdecl Lara_Col_Run(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.move_angle = item->pos.y_rot;
    coll->slopes_are_walls = 1;
    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    Lara_GetLaraCollisionInfo(item, coll);

    if (Lara_HitCeiling(item, coll)) {
        return;
    }
    if (Lara_TestVault(item, coll)) {
        return;
    }

    if (Lara_DeflectEdge(item, coll)) {
        item->pos.z_rot = 0;
        if (item->anim_num != LA_RUN_START
            && Lara_TestWall(item, 256, 0, -640)) {
            item->current_anim_state = LS_SPLAT;
            if (item->frame_num >= 0 && item->frame_num <= 9) {
                item->anim_num = LA_HIT_WALL_LEFT;
                item->frame_num = g_Anims[LS_FAST_DIVE].frame_base;
                return;
            }
            if (item->frame_num >= 10 && item->frame_num <= 21) {
                item->anim_num = LA_HIT_WALL_RIGHT;
                item->frame_num = g_Anims[LA_HIT_WALL_RIGHT].frame_base;
                return;
            }
        }
        Lara_CollideStop(item, coll);
    }

    if (Lara_Fallen(item, coll)) {
        return;
    }

    if (coll->side_mid.floor >= -STEPUP_HEIGHT
        && coll->side_mid.floor < -STEP_L / 2) {
        if (item->frame_num >= 3 && item->frame_num <= 14) {
            item->anim_num = LA_RUN_STEP_UP_LEFT;
            item->frame_num = g_Anims[LA_RUN_STEP_UP_LEFT].frame_base;
        } else {
            item->anim_num = LA_RUN_STEP_UP_RIGHT;
            item->frame_num = g_Anims[LA_RUN_STEP_UP_RIGHT].frame_base;
        }
    }

    if (Lara_TestSlide(item, coll)) {
        return;
    }

    item->pos.y += MIN(coll->side_mid.floor, 50);
}

void __cdecl Lara_Col_Stop(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    item->gravity = 0;
    item->fall_speed = 0;
    g_Lara.move_angle = item->pos.y_rot;
    coll->slopes_are_pits = 1;
    coll->slopes_are_walls = 1;
    coll->bad_pos = STEPUP_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    Lara_GetLaraCollisionInfo(item, coll);

    if (Lara_HitCeiling(item, coll)) {
        return;
    }
    if (Lara_Fallen(item, coll)) {
        return;
    }
    if (Lara_TestSlide(item, coll)) {
        return;
    }

    Item_ShiftCol(item, coll);
    item->pos.y += coll->side_mid.floor;
}

void __cdecl Lara_Col_ForwardJump(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (item->speed < 0) {
        g_Lara.move_angle = item->pos.y_rot + PHD_180;
    } else {
        g_Lara.move_angle = item->pos.y_rot;
    }

    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = BAD_JUMP_CEILING;

    Lara_GetLaraCollisionInfo(item, coll);
    Lara_DeflectEdgeJump(item, coll);

    if (item->speed < 0) {
        g_Lara.move_angle = item->pos.y_rot;
    }

    if (coll->side_mid.floor > 0 || item->fall_speed <= 0) {
        return;
    }

    if (Lara_LandedBad(item, coll)) {
        item->goal_anim_state = LS_DEATH;
    } else if (
        g_Lara.water_status != LWS_WADE && (g_Input & IN_FORWARD)
        && !(g_Input & IN_SLOW)) {
        item->goal_anim_state = LS_RUN;
    } else {
        item->goal_anim_state = LS_STOP;
    }

    item->gravity = 0;
    item->fall_speed = 0;
    item->pos.y += coll->side_mid.floor;
    item->speed = 0;
    Lara_Animate(item);
}
