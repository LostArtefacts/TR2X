#include "game/lara/lara_col.h"

#include "game/sound.h"
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

    if (Lara_HitCeiling(item, coll) || Lara_TestVault(item, coll)) {
        return;
    }

    if (Lara_DeflectEdge(item, coll)) {
        if (item->frame_num >= 29 && item->frame_num <= 47) {
            item->anim_num = LA_STOP_RIGHT;
            item->frame_num = g_Anims[item->anim_num].frame_base;
        } else if (
            (item->frame_num >= 22 && item->frame_num <= 28)
            || (item->frame_num >= 48 && item->frame_num <= 57)) {
            item->anim_num = LA_STOP_LEFT;
            item->frame_num = g_Anims[item->anim_num].frame_base;
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
            item->frame_num = g_Anims[item->anim_num].frame_base;
        } else {
            item->anim_num = LA_WALK_STEP_DOWN_LEFT;
            item->frame_num = g_Anims[item->anim_num].frame_base;
        }
    }

    if (coll->side_mid.floor >= -STEPUP_HEIGHT
        && coll->side_mid.floor < -STEP_L / 2) {
        if (item->frame_num >= 27 && item->frame_num <= 44) {
            item->anim_num = LA_WALK_STEP_UP_RIGHT;
            item->frame_num = g_Anims[item->anim_num].frame_base;
        } else {
            item->anim_num = LA_WALK_STEP_UP_LEFT;
            item->frame_num = g_Anims[item->anim_num].frame_base;
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

    if (Lara_HitCeiling(item, coll) || Lara_TestVault(item, coll)) {
        return;
    }

    if (Lara_DeflectEdge(item, coll)) {
        item->pos.z_rot = 0;
        if (item->anim_num != LA_RUN_START
            && Lara_TestWall(item, STEP_L, 0, -STEP_L * 5 / 2)) {
            item->current_anim_state = LS_SPLAT;
            if (item->frame_num >= 0 && item->frame_num <= 9) {
                item->anim_num = LA_HIT_WALL_LEFT;
                item->frame_num = g_Anims[item->anim_num].frame_base;
                return;
            }
            if (item->frame_num >= 10 && item->frame_num <= 21) {
                item->anim_num = LA_HIT_WALL_RIGHT;
                item->frame_num = g_Anims[item->anim_num].frame_base;
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
            item->frame_num = g_Anims[item->anim_num].frame_base;
        } else {
            item->anim_num = LA_RUN_STEP_UP_RIGHT;
            item->frame_num = g_Anims[item->anim_num].frame_base;
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

    if (Lara_HitCeiling(item, coll) || Lara_Fallen(item, coll)
        || Lara_TestSlide(item, coll)) {
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

void __cdecl Lara_Col_FastBack(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    item->gravity = 0;
    item->fall_speed = 0;
    g_Lara.move_angle = item->pos.y_rot + PHD_180;
    coll->slopes_are_pits = 1;
    coll->slopes_are_walls = 1;
    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;

    Lara_GetLaraCollisionInfo(item, coll);
    if (Lara_HitCeiling(item, coll)) {
        return;
    }

    if (coll->side_mid.floor <= 200) {
        if (Lara_DeflectEdge(item, coll)) {
            Lara_CollideStop(item, coll);
        }
        item->pos.y += coll->side_mid.floor;
    } else {
        item->anim_num = LA_FALL_BACK;
        item->frame_num = g_Anims[item->anim_num].frame_base;
        item->current_anim_state = LS_FALL_BACK;
        item->goal_anim_state = LS_FALL_BACK;
        item->gravity = 1;
        item->fall_speed = 0;
    }
}

void __cdecl Lara_Col_TurnRight(struct ITEM_INFO *item, struct COLL_INFO *coll)
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

    if (coll->side_mid.floor <= 100) {
        if (!Lara_TestSlide(item, coll)) {
            item->pos.y += coll->side_mid.floor;
        }
    } else {
        item->anim_num = LA_FALL_DOWN;
        item->frame_num = g_Anims[item->anim_num].frame_base;
        item->current_anim_state = LS_FORWARD_JUMP;
        item->goal_anim_state = LS_FORWARD_JUMP;
        item->gravity = 1;
        item->fall_speed = 0;
    }
}

void __cdecl Lara_Col_TurnLeft(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    Lara_Col_TurnRight(item, coll);
}

void __cdecl Lara_Col_Death(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    Sound_StopEffect(SFX_LARA_FALL);
    g_Lara.move_angle = item->pos.y_rot;
    coll->bad_pos = STEPUP_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    coll->radius = 400;

    Lara_GetLaraCollisionInfo(item, coll);
    Item_ShiftCol(item, coll);

    item->pos.y += coll->side_mid.floor;
    item->hit_points = -1;
    g_Lara.air = -1;
}

void __cdecl Lara_Col_FastFall(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    item->gravity = 1;
    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = BAD_JUMP_CEILING;

    Lara_GetLaraCollisionInfo(item, coll);
    Lara_SlideEdgeJump(item, coll);
    if (coll->side_mid.floor > 0) {
        return;
    }

    if (Lara_LandedBad(item, coll)) {
        item->goal_anim_state = LS_DEATH;
    } else {
        item->goal_anim_state = LS_STOP;
        item->current_anim_state = LS_STOP;
        item->anim_num = LA_LAND_FAR;
        item->frame_num = g_Anims[item->anim_num].frame_base;
    }

    Sound_StopEffect(SFX_LARA_FALL);
    item->gravity = 0;
    item->fall_speed = 0;
    item->pos.y += coll->side_mid.floor;
}

void __cdecl Lara_Col_Hang(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    Lara_HangTest(item, coll);
    if (item->goal_anim_state != LS_HANG) {
        return;
    }

    if ((g_Input & IN_FORWARD)) {
        if (coll->side_front.floor <= -850 || coll->side_front.floor >= -650
            || coll->side_front.floor - coll->side_front.ceiling < 0
            || coll->side_left.floor - coll->side_left.ceiling < 0
            || coll->side_right.floor - coll->side_right.ceiling < 0
            || coll->hit_static) {
            if (g_Lara.climb_status && item->anim_num == LA_GRAB_LEDGE
                && item->frame_num == g_Anims[item->anim_num].frame_base + 21
                && coll->side_mid.ceiling <= -256) {
                item->goal_anim_state = LS_HANG;
                item->current_anim_state = LS_HANG;
                item->anim_num = LA_HANG_UP;
                item->frame_num = g_Anims[item->anim_num].frame_base;
            }
        } else if (g_Input & IN_SLOW) {
            item->goal_anim_state = LS_GYMNAST;
        } else {
            item->goal_anim_state = LS_NULL;
        }
    } else if (
        (g_Input & IN_BACK) && g_Lara.climb_status
        && item->anim_num == LA_GRAB_LEDGE
        && item->frame_num == g_Anims[item->anim_num].frame_base + 21) {
        item->goal_anim_state = LS_HANG;
        item->current_anim_state = LS_HANG;
        item->anim_num = LA_HANG_DOWN;
        item->frame_num = g_Anims[item->anim_num].frame_base;
    }
}

void __cdecl Lara_Col_Reach(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    item->gravity = 1;
    g_Lara.move_angle = item->pos.y_rot;
    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = 0;
    coll->bad_ceiling = BAD_JUMP_CEILING;

    Lara_GetLaraCollisionInfo(item, coll);
    if (Lara_TestHangJump(item, coll)) {
        return;
    }

    Lara_SlideEdgeJump(item, coll);
    if (item->fall_speed <= 0 || coll->side_mid.floor > 0) {
        return;
    }

    if (Lara_LandedBad(item, coll)) {
        item->goal_anim_state = LS_DEATH;
    } else {
        item->goal_anim_state = LS_STOP;
    }
    item->gravity = 0;
    item->fall_speed = 0;
    item->pos.y += coll->side_mid.floor;
}

void __cdecl Lara_Col_Splat(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.move_angle = item->pos.y_rot;
    coll->slopes_are_pits = 1;
    coll->slopes_are_walls = 1;
    coll->bad_pos = STEPUP_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;

    Lara_GetLaraCollisionInfo(item, coll);
    Item_ShiftCol(item, coll);

    if (coll->side_mid.floor > -STEP_L && coll->side_mid.floor < STEP_L) {
        item->pos.y += coll->side_mid.floor;
    }
}

void __cdecl Lara_Col_Land(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    Lara_Col_Stop(item, coll);
}

void __cdecl Lara_Col_Compress(ITEM_INFO *item, COLL_INFO *coll)
{
    item->gravity = 0;
    item->fall_speed = 0;
    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = NO_HEIGHT;
    coll->bad_ceiling = 0;

    Lara_GetLaraCollisionInfo(item, coll);

    if (coll->side_mid.ceiling > -100) {
        item->anim_num = LA_STOP;
        item->frame_num = g_Anims[item->anim_num].frame_base;
        item->goal_anim_state = LS_STOP;
        item->current_anim_state = LS_STOP;
        item->gravity = 0;
        item->speed = 0;
        item->fall_speed = 0;
        item->pos.x = coll->old.x;
        item->pos.y = coll->old.y;
        item->pos.z = coll->old.z;
    }

    if (coll->side_mid.floor > -STEP_L && coll->side_mid.floor < STEP_L) {
        item->pos.y += coll->side_mid.floor;
    }
}

void __cdecl Lara_Col_Back(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    item->gravity = 0;
    item->fall_speed = 0;
    g_Lara.move_angle = item->pos.y_rot + PHD_180;
    if (g_Lara.water_status == LWS_WADE) {
        coll->bad_pos = NO_BAD_POS;
    } else {
        coll->bad_pos = STEPUP_HEIGHT;
    }
    coll->slopes_are_pits = 1;
    coll->slopes_are_walls = 1;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;

    Lara_GetLaraCollisionInfo(item, coll);
    if (Lara_HitCeiling(item, coll)) {
        return;
    }

    if (Lara_DeflectEdge(item, coll)) {
        Lara_CollideStop(item, coll);
    }

    if (Lara_Fallen(item, coll)) {
        return;
    }

    if (coll->side_mid.floor > STEP_L / 2
        && coll->side_mid.floor < STEPUP_HEIGHT) {
        if (item->frame_num >= 964 && item->frame_num <= 993) {
            item->anim_num = LA_BACK_STEP_DOWN_RIGHT;
            item->frame_num = g_Anims[item->anim_num].frame_base;
        } else {
            item->anim_num = LA_BACK_STEP_DOWN_LEFT;
            item->frame_num = g_Anims[item->anim_num].frame_base;
        }
    }

    if (!Lara_TestSlide(item, coll)) {
        item->pos.y += coll->side_mid.floor;
    }
}

void __cdecl Lara_Col_StepRight(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (item->current_anim_state == LS_STEP_RIGHT) {
        g_Lara.move_angle = item->pos.y_rot + PHD_90;
    } else {
        g_Lara.move_angle = item->pos.y_rot - PHD_90;
    }

    item->gravity = 0;
    item->fall_speed = 0;
    if (g_Lara.water_status == LWS_WADE) {
        coll->bad_pos = NO_BAD_POS;
    } else {
        coll->bad_pos = STEP_L / 2;
    }
    coll->slopes_are_pits = 1;
    coll->slopes_are_walls = 1;
    coll->bad_neg = -STEP_L / 2;
    coll->bad_ceiling = 0;

    Lara_GetLaraCollisionInfo(item, coll);
    if (Lara_HitCeiling(item, coll)) {
        return;
    }

    if (Lara_DeflectEdge(item, coll)) {
        Lara_CollideStop(item, coll);
    }

    if (!Lara_Fallen(item, coll) && !Lara_TestSlide(item, coll)) {
        item->pos.y += coll->side_mid.floor;
    }
}

void __cdecl Lara_Col_StepLeft(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    Lara_Col_StepRight(item, coll);
}

void __cdecl Lara_Col_Slide(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.move_angle = item->pos.y_rot;
    Lara_Slide_Slope(item, coll);
}

void __cdecl Lara_Col_BackJump(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.move_angle = item->pos.y_rot + PHD_180;
    Lara_Col_Jumper(item, coll);
}

void __cdecl Lara_Col_RightJump(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.move_angle = item->pos.y_rot + PHD_90;
    Lara_Col_Jumper(item, coll);
}

void __cdecl Lara_Col_LeftJump(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.move_angle = item->pos.y_rot - PHD_90;
    Lara_Col_Jumper(item, coll);
}

void __cdecl Lara_Col_UpJump(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.move_angle = item->pos.y_rot;
    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = BAD_JUMP_CEILING;
    if (item->speed < 0) {
        coll->facing = g_Lara.move_angle + PHD_180;
    } else {
        coll->facing = g_Lara.move_angle;
    }

    Collide_GetCollisionInfo(
        coll, item->pos.x, item->pos.y, item->pos.z, item->room_num, 870);
    if (Lara_TestHangJumpUp(item, coll)) {
        return;
    }

    Lara_SlideEdgeJump(item, coll);
    if (coll->coll_type != COLL_NONE) {
        item->speed = item->speed > 0 ? 2 : -2;
    } else if (item->fall_speed < -70) {
        if (g_Input & IN_FORWARD && item->speed < 5) {
            item->speed++;
        } else if (g_Input & IN_BACK && item->speed > -5) {
            item->speed -= 2;
        }
    }

    if (item->fall_speed <= 0 || coll->side_mid.floor > 0) {
        return;
    }

    if (Lara_LandedBad(item, coll)) {
        item->goal_anim_state = LS_DEATH;
    } else {
        item->goal_anim_state = LS_STOP;
    }
    item->gravity = 0;
    item->fall_speed = 0;
    item->pos.y += coll->side_mid.floor;
}

void __cdecl Lara_Col_Fallback(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.move_angle = item->pos.y_rot + PHD_180;
    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = BAD_JUMP_CEILING;

    Lara_GetLaraCollisionInfo(item, coll);
    Lara_DeflectEdgeJump(item, coll);

    if (coll->side_mid.floor > 0 || item->fall_speed <= 0) {
        return;
    }

    if (Lara_LandedBad(item, coll)) {
        item->goal_anim_state = LS_DEATH;
    } else {
        item->goal_anim_state = LS_STOP;
    }

    item->gravity = 0;
    item->fall_speed = 0;
    item->pos.y += coll->side_mid.floor;
}

void __cdecl Lara_Col_HangLeft(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.move_angle = item->pos.y_rot - PHD_90;
    Lara_HangTest(item, coll);
    g_Lara.move_angle = item->pos.y_rot - PHD_90;
}

void __cdecl Lara_Col_HangRight(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.move_angle = item->pos.y_rot + PHD_90;
    Lara_HangTest(item, coll);
    g_Lara.move_angle = item->pos.y_rot + PHD_90;
}

void __cdecl Lara_Col_SlideBack(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.move_angle = item->pos.y_rot + PHD_180;
    Lara_Slide_Slope(item, coll);
}

void __cdecl Lara_Col_Null(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    Lara_Col_Default(item, coll);
}

void __cdecl Lara_Col_Roll(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    item->gravity = 0;
    item->fall_speed = 0;
    g_Lara.move_angle = item->pos.y_rot;
    coll->slopes_are_walls = 1;
    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;

    Lara_GetLaraCollisionInfo(item, coll);
    if (Lara_HitCeiling(item, coll) || Lara_TestSlide(item, coll)
        || Lara_Fallen(item, coll)) {
        return;
    }

    Item_ShiftCol(item, coll);
    item->pos.y += coll->side_mid.floor;
}

void __cdecl Lara_Col_Roll2(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    item->gravity = 0;
    item->fall_speed = 0;
    g_Lara.move_angle = item->pos.y_rot + PHD_180;
    coll->slopes_are_walls = 1;
    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;

    Lara_GetLaraCollisionInfo(item, coll);
    if (Lara_HitCeiling(item, coll) || Lara_TestSlide(item, coll)) {
        return;
    }

    if (coll->side_mid.floor > 200) {
        item->anim_num = LA_FALL_BACK;
        item->frame_num = g_Anims[item->anim_num].frame_base;
        item->current_anim_state = LS_FALL_BACK;
        item->goal_anim_state = LS_FALL_BACK;
        item->gravity = 1;
        item->fall_speed = 0;
    } else {
        Item_ShiftCol(item, coll);
        item->pos.y += coll->side_mid.floor;
    }
}

void __cdecl Lara_Col_SwanDive(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.move_angle = item->pos.y_rot;
    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = BAD_JUMP_CEILING;

    Lara_GetLaraCollisionInfo(item, coll);
    Lara_DeflectEdgeJump(item, coll);
    if (coll->side_mid.floor > 0 || item->fall_speed <= 0) {
        return;
    }

    item->goal_anim_state = LS_STOP;
    item->gravity = 0;
    item->fall_speed = 0;
    item->pos.y += coll->side_mid.floor;
}

void __cdecl Lara_Col_FastDive(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.move_angle = item->pos.y_rot;
    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = BAD_JUMP_CEILING;

    Lara_GetLaraCollisionInfo(item, coll);
    Lara_DeflectEdgeJump(item, coll);

    if (coll->side_mid.floor > 0 || item->fall_speed <= 0) {
        return;
    }

    if (item->fall_speed > 133) {
        item->goal_anim_state = LS_DEATH;
    } else {
        item->goal_anim_state = LS_STOP;
    }
    item->gravity = 0;
    item->fall_speed = 0;
    item->pos.y += coll->side_mid.floor;
}

void __cdecl Lara_Col_Wade(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.move_angle = item->pos.y_rot;
    coll->slopes_are_walls = 1;
    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;

    Lara_GetLaraCollisionInfo(item, coll);
    if (Lara_HitCeiling(item, coll) || Lara_TestVault(item, coll)) {
        return;
    }

    if (Lara_DeflectEdge(item, coll)) {
        item->pos.z_rot = 0;
        if (coll->side_front.type != COLL_NONE
            && coll->side_front.floor < -STEP_L * 5 / 2) {
            item->current_anim_state = LS_SPLAT;
            if (item->frame_num >= 0 && item->frame_num <= 9) {
                item->anim_num = LA_HIT_WALL_LEFT;
                item->frame_num = g_Anims[item->anim_num].frame_base;
                return;
            }
            if (item->frame_num >= 10 && item->frame_num <= 21) {
                item->anim_num = LA_HIT_WALL_RIGHT;
                item->frame_num = g_Anims[item->anim_num].frame_base;
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
            item->frame_num = g_Anims[item->anim_num].frame_base;
        } else {
            item->anim_num = LA_RUN_STEP_UP_RIGHT;
            item->frame_num = g_Anims[item->anim_num].frame_base;
        }
    }

    if (Lara_TestSlide(item, coll)) {
        return;
    }

    item->pos.y += MIN(coll->side_mid.floor, 50);
}

void __cdecl Lara_Col_Default(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.move_angle = item->pos.y_rot;
    coll->slopes_are_walls = 1;
    coll->slopes_are_pits = 1;
    coll->bad_pos = STEPUP_HEIGHT;
    coll->bad_neg = -STEPUP_HEIGHT;
    coll->bad_ceiling = 0;
    Lara_GetLaraCollisionInfo(item, coll);
}
