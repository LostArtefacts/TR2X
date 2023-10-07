#include "game/lara/lara_misc.h"

#include "game/math.h"
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

void __cdecl Lara_DeflectEdgeJump(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    Item_ShiftCol(item, coll);
    switch (coll->coll_type) {
    case COLL_FRONT:
    case COLL_TOPFRONT:
        if (!g_Lara.climb_status || item->speed != 2) {
            if (coll->side_mid.floor > 512) {
                item->goal_anim_state = LS_FAST_FALL;
                item->current_anim_state = LS_FAST_FALL;
                item->anim_num = LA_FAST_FALL;
                item->frame_num = g_Anims[item->anim_num].frame_base + 1;
            } else if (coll->side_mid.floor <= 128) {
                item->goal_anim_state = LS_LAND;
                item->current_anim_state = LS_LAND;
                item->anim_num = LA_LAND;
                item->frame_num = g_Anims[item->anim_num].frame_base;
            }
            item->speed /= 4;
            g_Lara.move_angle += PHD_180;
            CLAMPL(item->fall_speed, 1);
        }
        break;

    case COLL_LEFT:
        item->pos.y_rot += LARA_DEFLECT_ANGLE;
        break;

    case COLL_RIGHT:
        item->pos.y_rot -= LARA_DEFLECT_ANGLE;
        break;

    case COLL_TOP:
        CLAMPL(item->fall_speed, 1);
        break;

    case COLL_CLAMP:
        item->pos.z -= (Math_Cos(coll->facing) * 100) >> W2V_SHIFT;
        item->pos.x -= (Math_Sin(coll->facing) * 100) >> W2V_SHIFT;
        item->speed = 0;
        coll->side_mid.floor = 0;
        if (item->fall_speed <= 0) {
            item->fall_speed = 16;
        }
        break;

    default:
        break;
    }
}

void __cdecl Lara_SlideEdgeJump(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    Item_ShiftCol(item, coll);

    switch (coll->coll_type) {
    case COLL_LEFT:
        item->pos.y_rot += LARA_DEFLECT_ANGLE;
        break;

    case COLL_RIGHT:
        item->pos.y_rot -= LARA_DEFLECT_ANGLE;
        break;

    case COLL_TOP:
    case COLL_TOPFRONT:
        CLAMPL(item->fall_speed, 1);
        break;

    case COLL_CLAMP:
        item->pos.z -= (Math_Cos(coll->facing) * 100) >> W2V_SHIFT;
        item->pos.x -= (Math_Sin(coll->facing) * 100) >> W2V_SHIFT;
        item->speed = 0;
        coll->side_mid.floor = 0;
        if (item->fall_speed <= 0) {
            item->fall_speed = 16;
        }
        break;

    default:
        break;
    }
}

int32_t __cdecl Lara_TestWall(
    struct ITEM_INFO *item, int32_t front, int32_t right, int32_t down)
{
    int32_t x = item->pos.x;
    int32_t y = item->pos.y + down;
    int32_t z = item->pos.z;

    DIRECTION dir = Math_GetDirection(item->pos.y_rot);
    switch (dir) {
    case DIR_NORTH:
        x -= right;
        break;
    case DIR_EAST:
        z -= right;
        break;
    case DIR_SOUTH:
        x += right;
        break;
    case DIR_WEST:
        z += right;
        break;
    }

    int16_t room_num = item->room_num;
    Room_GetFloor(x, y, z, &room_num);

    switch (dir) {
    case DIR_NORTH:
        z += front;
        break;
    case DIR_EAST:
        x += front;
        break;
    case DIR_SOUTH:
        z -= front;
        break;
    case DIR_WEST:
        x -= front;
        break;
    }

    const struct FLOOR_INFO *floor = Room_GetFloor(x, y, z, &room_num);
    int32_t height = Room_GetHeight(floor, x, y, z);
    int32_t ceiling = Room_GetCeiling(floor, x, y, z);
    if (height == NO_HEIGHT) {
        return 1;
    }
    if (height - y > 0 && ceiling - y < 0) {
        return 0;
    }
    return 2;
}

int32_t __cdecl Lara_TestHangOnClimbWall(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (!g_Lara.climb_status || item->fall_speed < 0) {
        return 0;
    }

    DIRECTION dir = Math_GetDirection(item->pos.y_rot);
    switch (dir) {
    case DIR_NORTH:
    case DIR_SOUTH:
        item->pos.z += coll->shift.z;
        break;

    case DIR_EAST:
    case DIR_WEST:
        item->pos.x += coll->shift.x;
        break;
    }

    int16_t *bounds = Item_GetBoundsAccurate(item);
    int32_t y = bounds[FBBOX_MIN_Y];
    int32_t h = bounds[FBBOX_MAX_Y] - y;

    int32_t shift;
    if (!Lara_TestClimbPos(item, coll->radius, coll->radius, y, h, &shift)) {
        return 0;
    }

    if (!Lara_TestClimbPos(item, coll->radius, -coll->radius, y, h, &shift)) {
        return 0;
    }

    int32_t result = Lara_TestClimbPos(item, coll->radius, 0, y, h, &shift);
    switch (result) {
    case 0:
    case 1:
        return result;

    default:
        item->pos.y += shift;
        return 1;
    }
}

int32_t __cdecl Lara_TestClimbStance(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    int32_t shift_r;
    int32_t result_r = Lara_TestClimbPos(
        item, coll->radius, coll->radius + LARA_CLIMB_WIDTH_RIGHT, -700,
        STEP_L * 2, &shift_r);
    if (result_r != 1) {
        return 0;
    }

    int32_t shift_l;
    int32_t result_l = Lara_TestClimbPos(
        item, coll->radius, -(coll->radius + LARA_CLIMB_WIDTH_LEFT), -700,
        STEP_L * 2, &shift_l);
    if (result_l != 1) {
        return 0;
    }

    int32_t shift = 0;
    if (shift_r) {
        if (shift_l) {
            if ((shift_r < 0) != (shift_l < 0)) {
                return 0;
            }
            if (shift_r < 0 && shift_l < shift_r) {
                shift = shift_l;
            } else if (shift_r > 0 && shift_l > shift_r) {
                shift = shift_l;
            } else {
                shift = shift_r;
            }
        } else {
            shift = shift_r;
        }
    } else if (shift_l) {
        shift = shift_l;
    }

    item->pos.y += shift;
    return 1;
}
