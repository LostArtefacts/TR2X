#include "game/lara/lara_misc.h"

#include "game/math.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

void __cdecl Lara_GetCollisionInfo(
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
    Lara_GetCollisionInfo(item, coll);

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

    enum DIRECTION dir = Math_GetDirection(item->pos.y_rot);
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
    default:
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
    default:
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

    enum DIRECTION dir = Math_GetDirection(item->pos.y_rot);
    switch (dir) {
    case DIR_NORTH:
    case DIR_SOUTH:
        item->pos.z += coll->shift.z;
        break;

    case DIR_EAST:
    case DIR_WEST:
        item->pos.x += coll->shift.x;
        break;

    default:
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

void __cdecl Lara_HangTest(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = NO_BAD_NEG;
    coll->bad_ceiling = 0;
    Lara_GetCollisionInfo(item, coll);
    bool flag = coll->side_front.floor < 200;

    item->gravity = 0;
    item->fall_speed = 0;
    g_Lara.move_angle = item->pos.y_rot;

    enum DIRECTION dir = Math_GetDirection(item->pos.y_rot);
    switch (dir) {
    case DIR_NORTH:
        item->pos.z += 2;
        break;
    case DIR_EAST:
        item->pos.x += 2;
        break;
    case DIR_SOUTH:
        item->pos.z -= 2;
        break;
    case DIR_WEST:
        item->pos.x -= 2;
        break;
    default:
        break;
    }

    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    Lara_GetCollisionInfo(item, coll);

    if (g_Lara.climb_status) {
        if (!(g_Input & IN_ACTION) || item->hit_points <= 0) {
            struct PHD_VECTOR pos = {
                .x = 0,
                .y = 0,
                .z = 0,
            };
            Collide_GetJointAbsPosition(item, &pos, 0);
            if (dir == DIR_NORTH || dir == DIR_SOUTH) {
                item->pos.x = pos.x;
            } else {
                item->pos.z = pos.z;
            }

            item->goal_anim_state = LS_FORWARD_JUMP;
            item->current_anim_state = LS_FORWARD_JUMP;
            item->anim_num = LA_FALL_DOWN;
            item->frame_num = g_Anims[item->anim_num].frame_base;
            item->pos.y += STEP_L;
            item->gravity = 1;
            item->speed = 2;
            item->fall_speed = 1;
            g_Lara.gun_status = LGS_ARMLESS;
            return;
        }

        if (!Lara_TestHangOnClimbWall(item, coll)) {
            item->pos.x = coll->old.x;
            item->pos.y = coll->old.y;
            item->pos.z = coll->old.z;
            item->goal_anim_state = LS_HANG;
            item->current_anim_state = LS_HANG;
            item->anim_num = LA_HANG;
            item->frame_num = g_Anims[item->anim_num].frame_base + 21;
            return;
        }

        if (item->anim_num == LA_HANG
            && item->frame_num == g_Anims[LA_HANG].frame_base + 21
            && Lara_TestClimbStance(item, coll)) {
            item->goal_anim_state = LS_CLIMB_STANCE;
        }
        return;
    }

    if (!(g_Input & IN_ACTION) || item->hit_points <= 0
        || coll->side_front.floor > 0) {
        item->goal_anim_state = LS_UP_JUMP;
        item->current_anim_state = LS_UP_JUMP;
        item->anim_num = LA_STOP_HANG;
        item->frame_num = g_Anims[item->anim_num].frame_base + 9;
        int16_t *bounds = Item_GetBoundsAccurate(item);
        item->pos.y += bounds[FBBOX_MAX_Y];
        item->pos.x += coll->shift.x;
        item->pos.z += coll->shift.z;
        item->gravity = 1;
        item->speed = 2;
        item->fall_speed = 1;
        g_Lara.gun_status = LGS_ARMLESS;
        return;
    }

    int16_t *bounds = Item_GetBoundsAccurate(item);
    int32_t hdif = coll->side_front.floor - bounds[FBBOX_MIN_Y];

    if (ABS(coll->side_left.floor - coll->side_right.floor) >= SLOPE_DIF
        || coll->side_mid.ceiling >= 0 || coll->coll_type != COLL_FRONT || flag
        || coll->hit_static || hdif < -SLOPE_DIF || hdif > SLOPE_DIF) {
        item->pos.x = coll->old.x;
        item->pos.y = coll->old.y;
        item->pos.z = coll->old.z;
        if (item->current_anim_state == LS_HANG_LEFT
            || item->current_anim_state == LS_HANG_RIGHT) {
            item->goal_anim_state = LS_HANG;
            item->current_anim_state = LS_HANG;
            item->anim_num = LA_HANG;
            item->frame_num = g_Anims[item->anim_num].frame_base + 21;
        }
        return;
    }

    switch (dir) {
    case DIR_NORTH:
    case DIR_SOUTH:
        item->pos.z += coll->shift.z;
        break;

    case DIR_EAST:
    case DIR_WEST:
        item->pos.x += coll->shift.x;
        break;

    default:
        break;
    }

    item->pos.y += hdif;
}

int32_t __cdecl Lara_TestEdgeCatch(
    struct ITEM_INFO *item, struct COLL_INFO *coll, int32_t *edge)
{
    int16_t *bounds = Item_GetBoundsAccurate(item);
    int32_t hdif1 = coll->side_front.floor - bounds[FBBOX_MIN_Y];
    int32_t hdif2 = hdif1 + item->fall_speed;
    if ((hdif1 < 0 && hdif2 < 0) || (hdif1 > 0 && hdif2 > 0)) {
        hdif1 = item->pos.y + bounds[FBBOX_MIN_Y];
        hdif2 = hdif1 + item->fall_speed;
        if ((hdif1 >> (WALL_SHIFT - 2)) == (hdif2 >> (WALL_SHIFT - 2))) {
            return 0;
        }
        if (item->fall_speed > 0) {
            *edge = hdif2 & ~(STEP_L - 1);
        } else {
            *edge = hdif1 & ~(STEP_L - 1);
        }
        return -1;
    }

    return ABS(coll->side_left.floor - coll->side_right.floor) < SLOPE_DIF;
}

int32_t __cdecl Lara_TestHangJumpUp(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (coll->coll_type != COLL_FRONT || !(g_Input & IN_ACTION)
        || g_Lara.gun_status != LGS_ARMLESS || coll->hit_static
        || coll->side_mid.ceiling > -STEPUP_HEIGHT) {
        return 0;
    }

    int32_t edge;
    int32_t edge_catch = Lara_TestEdgeCatch(item, coll, &edge);
    if (!edge_catch
        || (edge_catch < 0 && !Lara_TestHangOnClimbWall(item, coll))) {
        return 0;
    }

    enum DIRECTION dir =
        Math_GetDirectionCone(item->pos.y_rot, LARA_HANG_ANGLE);
    if (dir == DIR_UNKNOWN) {
        return 0;
    }
    int16_t angle = Math_DirectionToAngle(dir);

    item->goal_anim_state = LS_HANG;
    item->current_anim_state = LS_HANG;
    item->anim_num = LA_HANG;
    item->frame_num = g_Anims[item->anim_num].frame_base + 12;

    int16_t *bounds = Item_GetBoundsAccurate(item);
    if (edge_catch > 0) {
        item->pos.y += coll->side_front.floor - bounds[FBBOX_MIN_Y];
    } else {
        item->pos.y = edge - bounds[FBBOX_MIN_Y];
    }
    item->pos.x += coll->shift.x;
    item->pos.z += coll->shift.z;
    item->pos.y_rot = angle;
    item->speed = 0;
    item->gravity = 0;
    item->fall_speed = 0;
    g_Lara.gun_status = LGS_HANDS_BUSY;
    return 1;
}

int32_t __cdecl Lara_TestHangJump(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (coll->coll_type != COLL_FRONT || !(g_Input & IN_ACTION)
        || g_Lara.gun_status != LGS_ARMLESS || coll->hit_static
        || coll->side_mid.ceiling > -STEPUP_HEIGHT
        || coll->side_mid.floor < 200) {
        return 0;
    }

    int32_t edge;
    int32_t edge_catch = Lara_TestEdgeCatch(item, coll, &edge);
    if (!edge_catch
        || (edge_catch < 0 && !Lara_TestHangOnClimbWall(item, coll))) {
        return 0;
    }

    enum DIRECTION dir =
        Math_GetDirectionCone(item->pos.y_rot, LARA_HANG_ANGLE);
    if (dir == DIR_UNKNOWN) {
        return 0;
    }
    int16_t angle = Math_DirectionToAngle(dir);

    if (Lara_TestHangSwingIn(item, angle)) {
        item->anim_num = LA_GRAB_LEDGE_IN;
        item->frame_num = g_Anims[item->anim_num].frame_base;
    } else {
        item->anim_num = LA_GRAB_LEDGE;
        item->frame_num = g_Anims[item->anim_num].frame_base;
    }
    item->current_anim_state = LS_HANG;
    item->goal_anim_state = LS_HANG;

    int16_t *bounds = Item_GetBoundsAccurate(item);
    if (edge_catch > 0) {
        item->pos.y += coll->side_front.floor - bounds[FBBOX_MIN_Y];
        item->pos.x += coll->shift.x;
        item->pos.z += coll->shift.z;
    } else {
        item->pos.y = edge - bounds[FBBOX_MIN_Y];
    }

    item->pos.y_rot = angle;
    item->speed = 2;
    item->gravity = 1;
    item->fall_speed = 1;
    g_Lara.gun_status = LGS_HANDS_BUSY;
    return 1;
}

int32_t __cdecl Lara_TestHangSwingIn(struct ITEM_INFO *item, PHD_ANGLE angle)
{
    int32_t x = item->pos.x;
    int32_t y = item->pos.y;
    int32_t z = item->pos.z;
    int16_t room_num = item->room_num;
    switch (angle) {
    case 0:
        z += STEP_L;
        break;
    case PHD_90:
        x += STEP_L;
        break;
    case -PHD_180:
        z -= STEP_L;
        break;
    case -PHD_90:
        x -= STEP_L;
        break;
    }

    const struct FLOOR_INFO *floor = Room_GetFloor(x, y, z, &room_num);
    int32_t height = Room_GetHeight(floor, x, y, z);
    int32_t ceiling = Room_GetCeiling(floor, x, y, z);
    return height != NO_HEIGHT && height - y > 0 && ceiling - y < -400;
}
