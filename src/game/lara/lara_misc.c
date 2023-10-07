#include "game/lara/lara_misc.h"

#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

void __cdecl Lara_GetLaraCollisionInfo(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    coll->facing = g_Lara.move_angle;
    Collide_GetCollisionInfo(
        coll, item->pos.x, item->pos.y, item->pos.z, item->room_num,
        LARA_HEIGHT);
}

void __cdecl Lara_SlideSlope(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = -STEP_L * 2;
    coll->bad_ceiling = 0;
    Lara_GetLaraCollisionInfo(item, coll);

    if (Lara_HitCeiling(item, coll)) {
        return;
    }

    Lara_DeflectEdge(item, coll);

    if (coll->side_mid.floor > 200) {
        if (item->current_anim_state == LS_SLIDE) {
            item->goal_anim_state = LS_FORWARD_JUMP;
            item->current_anim_state = LS_FORWARD_JUMP;
            item->anim_num = LS_SURF_SWIM;
            item->frame_num = g_Anims[item->anim_num].frame_base;
        } else {
            item->goal_anim_state = LS_FALL_BACK;
            item->current_anim_state = LS_FALL_BACK;
            item->anim_num = LA_FALL_BACK;
            item->frame_num = g_Anims[item->anim_num].frame_base;
        }
        item->gravity = 1;
        item->fall_speed = 0;
        return;
    }

    Lara_TestSlide(item, coll);
    item->pos.y += coll->side_mid.floor;
    if (ABS(coll->x_tilt) <= 2 && ABS(coll->z_tilt) <= 2) {
        item->goal_anim_state = LS_STOP;
    }
}

int32_t __cdecl Lara_HitCeiling(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (coll->coll_type != COLL_TOP && coll->coll_type != COLL_CLAMP) {
        return 0;
    }

    item->pos.x = coll->old.x;
    item->pos.y = coll->old.y;
    item->pos.z = coll->old.z;
    item->goal_anim_state = LS_STOP;
    item->current_anim_state = LS_STOP;
    item->anim_num = LS_REACH;
    item->frame_num = g_Anims[item->anim_num].frame_base;
    item->speed = 0;
    item->gravity = 0;
    item->fall_speed = 0;
    return 1;
}

int32_t __cdecl Lara_DeflectEdge(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    switch (coll->coll_type) {
    case COLL_FRONT:
    case COLL_TOPFRONT:
        Item_ShiftCol(item, coll);
        item->goal_anim_state = LS_STOP;
        item->current_anim_state = LS_STOP;
        item->gravity = 0;
        item->speed = 0;
        return 1;

    case COLL_LEFT:
        Item_ShiftCol(item, coll);
        item->pos.y_rot += LARA_DEFLECT_ANGLE;
        return 0;

    case COLL_RIGHT:
        Item_ShiftCol(item, coll);
        item->pos.y_rot -= LARA_DEFLECT_ANGLE;
        return 0;

    default:
        return 0;
    }
}
