#include "game/lara/lara_state.h"

#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

void __cdecl Lara_State_Walk(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = LS_STOP;
    }

    if (g_Input & IN_LEFT) {
        g_Lara.turn_rate -= LARA_TURN_RATE;
        CLAMPL(g_Lara.turn_rate, -LARA_SLOW_TURN);
    } else if (g_Input & IN_RIGHT) {
        g_Lara.turn_rate += LARA_TURN_RATE;
        CLAMPG(g_Lara.turn_rate, +LARA_SLOW_TURN);
    }

    if (g_Input & IN_FORWARD) {
        if (g_Lara.water_status == LWS_WADE) {
            item->goal_anim_state = LS_WADE;
        } else if (g_Input & IN_SLOW) {
            item->goal_anim_state = LS_WALK;
        } else {
            item->goal_anim_state = LS_RUN;
        }
    } else {
        item->goal_anim_state = LS_STOP;
    }
}

void __cdecl Lara_State_Run(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = LS_DEATH;
        return;
    }

    if (g_Input & IN_ROLL) {
        item->anim_num = LA_ROLL;
        item->current_anim_state = LS_ROLL;
        item->goal_anim_state = LS_STOP;
        item->frame_num = g_Anims[LA_ROLL].frame_base + 2;
        return;
    }

    if (g_Input & IN_LEFT) {
        g_Lara.turn_rate -= LARA_TURN_RATE;
        CLAMPL(g_Lara.turn_rate, -LARA_FAST_TURN);
        item->pos.z_rot -= LARA_LEAN_RATE;
        CLAMPL(item->pos.z_rot, -LARA_LEAN_MAX);
    } else if (g_Input & IN_RIGHT) {
        g_Lara.turn_rate += LARA_TURN_RATE;
        CLAMPG(g_Lara.turn_rate, +LARA_FAST_TURN);
        item->pos.z_rot += LARA_LEAN_RATE;
        CLAMPG(item->pos.z_rot, +LARA_LEAN_MAX);
    }

    if (item->anim_num == LA_RUN_START) {
        g_JumpPermitted = 0;
    } else if (item->anim_num != LA_RUN || item->frame_num == 4) {
        g_JumpPermitted = 1;
    }

    if ((g_Input & IN_JUMP) && g_JumpPermitted && !item->gravity) {
        item->goal_anim_state = LS_FORWARD_JUMP;
    } else if (g_Input & IN_FORWARD) {
        if (g_Lara.water_status == LWS_WADE) {
            item->goal_anim_state = LS_WADE;
        } else if (g_Input & IN_SLOW) {
            item->goal_anim_state = LS_WALK;
        } else {
            item->goal_anim_state = LS_RUN;
        }
    } else {
        item->goal_anim_state = LS_STOP;
    }
}

void __cdecl Lara_State_Stop(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = LS_DEATH;
        return;
    }

    if ((g_Input & IN_ROLL) && g_Lara.water_status != LWS_WADE) {
        item->anim_num = LA_ROLL;
        item->current_anim_state = LS_ROLL;
        item->goal_anim_state = LS_STOP;
        item->frame_num = g_Anims[LA_ROLL].frame_base + 2;
        return;
    }

    item->goal_anim_state = LS_STOP;
    if (g_Input & IN_LOOK) {
        Lara_LookUpDown();
    }

    if (g_Input & IN_STEP_LEFT) {
        item->goal_anim_state = LS_STEP_LEFT;
    } else if (g_Input & IN_STEP_RIGHT) {
        item->goal_anim_state = LS_STEP_RIGHT;
    } else if (g_Input & IN_LEFT) {
        item->goal_anim_state = LS_TURN_LEFT;
    } else if (g_Input & IN_RIGHT) {
        item->goal_anim_state = LS_TURN_RIGHT;
    }

    if (g_Lara.water_status == LWS_WADE) {
        if (g_Input & IN_JUMP) {
            item->goal_anim_state = LS_COMPRESS;
        }

        if (g_Input & IN_FORWARD) {
            if (g_Input & IN_SLOW) {
                Lara_State_Wade(item, coll);
            } else {
                Lara_State_Walk(item, coll);
            }
        } else if (g_Input & IN_BACK) {
            Lara_State_Back(item, coll);
        }
    } else if (g_Input & IN_JUMP) {
        item->goal_anim_state = LS_COMPRESS;
    } else if (g_Input & IN_FORWARD) {
        if (g_Input & IN_SLOW) {
            Lara_State_Walk(item, coll);
        } else {
            Lara_State_Run(item, coll);
        }
    } else if (g_Input & IN_BACK) {
        if (g_Input & IN_SLOW) {
            Lara_State_Back(item, coll);
        } else {
            item->goal_anim_state = LS_FAST_BACK;
        }
    }
}

void __cdecl Lara_State_ForwardJump(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (item->goal_anim_state == LS_SWAN_DIVE
        || item->goal_anim_state == LS_REACH) {
        item->goal_anim_state = LS_FORWARD_JUMP;
    }

    if (item->goal_anim_state != LS_DEATH && item->goal_anim_state != LS_STOP
        && item->goal_anim_state != LS_RUN) {
        if ((g_Input & IN_ACTION) && g_Lara.gun_status == LGS_ARMLESS) {
            item->goal_anim_state = LS_REACH;
        }
        if ((g_Input & IN_ROLL) || (g_Input & IN_BACK)) {
            item->goal_anim_state = LS_TWIST;
        }
        if ((g_Input & IN_SLOW) && g_Lara.gun_status == LGS_ARMLESS) {
            item->goal_anim_state = LS_SWAN_DIVE;
        }
        if (item->fall_speed > LARA_FAST_FALL_SPEED) {
            item->goal_anim_state = LS_FAST_FALL;
        }
    }

    if (g_Input & IN_LEFT) {
        g_Lara.turn_rate -= LARA_TURN_RATE;
        CLAMPL(g_Lara.turn_rate, -LARA_JUMP_TURN);
    } else if (g_Input & IN_RIGHT) {
        g_Lara.turn_rate += LARA_TURN_RATE;
        CLAMPG(g_Lara.turn_rate, +LARA_JUMP_TURN);
    }
}

void __cdecl Lara_State_FastBack(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    item->goal_anim_state = LS_STOP;
    if (g_Input & IN_LEFT) {
        g_Lara.turn_rate -= LARA_TURN_RATE;
        CLAMPL(g_Lara.turn_rate, -LARA_MED_TURN);
    } else if (g_Input & IN_RIGHT) {
        g_Lara.turn_rate += LARA_TURN_RATE;
        CLAMPG(g_Lara.turn_rate, LARA_MED_TURN);
    }
}

void __cdecl Lara_State_TurnRight(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = LS_STOP;
        return;
    }

    g_Lara.turn_rate += LARA_TURN_RATE;

    if (g_Lara.gun_status == LWS_WADE) {
        item->goal_anim_state = LS_FAST_TURN;
    } else if (g_Lara.turn_rate > LARA_SLOW_TURN) {
        if (g_Input & IN_SLOW) {
            g_Lara.turn_rate = LARA_SLOW_TURN;
        } else {
            item->goal_anim_state = LS_FAST_TURN;
        }
    }

    if (g_Input & IN_FORWARD) {
        if (g_Lara.water_status == LWS_WADE) {
            item->goal_anim_state = LS_WADE;
        } else if (g_Input & IN_SLOW) {
            item->goal_anim_state = LS_WALK;
        } else {
            item->goal_anim_state = LS_RUN;
        }
    } else if (!(g_Input & IN_RIGHT)) {
        item->goal_anim_state = LS_STOP;
    }
}

void __cdecl Lara_State_TurnLeft(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = LS_STOP;
        return;
    }

    g_Lara.turn_rate -= LARA_TURN_RATE;

    if (g_Lara.gun_status == IN_LEFT) {
        item->goal_anim_state = LS_FAST_TURN;
    } else if (g_Lara.turn_rate < -LARA_SLOW_TURN) {
        if (g_Input & IN_SLOW) {
            g_Lara.turn_rate = -LARA_SLOW_TURN;
        } else {
            item->goal_anim_state = LS_FAST_TURN;
        }
    }

    if (g_Input & IN_FORWARD) {
        if (g_Lara.water_status == IN_LEFT) {
            item->goal_anim_state = LS_WADE;
        } else if (g_Input & IN_SLOW) {
            item->goal_anim_state = LS_WALK;
        } else {
            item->goal_anim_state = LS_RUN;
        }
    } else if (!(g_Input & IN_LEFT)) {
        item->goal_anim_state = LS_STOP;
    }
}

void __cdecl Lara_State_Death(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.look = 0;
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
}

void __cdecl Lara_State_FastFall(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    item->speed = item->speed * 95 / 100;
    if (item->fall_speed == DAMAGE_START + DAMAGE_LENGTH) {
        Sound_Effect(SFX_LARA_FALL, &item->pos, 0);
    }
}

void __cdecl Lara_State_Hang(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (g_Input & IN_LOOK) {
        Lara_LookUpDown();
    }

    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    g_Camera.target_angle = CAM_HANG_ANGLE;
    g_Camera.target_elevation = CAM_HANG_ELEVATION;
    if ((g_Input & IN_LEFT) || (g_Input & IN_STEP_LEFT)) {
        item->goal_anim_state = LS_HANG_LEFT;
    } else if ((g_Input & IN_RIGHT) || (g_Input & IN_STEP_RIGHT)) {
        item->goal_anim_state = LS_HANG_RIGHT;
    }
}

void __cdecl Lara_State_Reach(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Camera.target_angle = CAM_REACH_ANGLE;
    if (item->fall_speed > LARA_FAST_FALL_SPEED) {
        item->goal_anim_state = LS_FAST_FALL;
    }
}

void __cdecl Lara_State_Splat(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.look = 0;
}

void __cdecl Lara_State_Compress(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (g_Lara.water_status != LWS_WADE) {
        if ((g_Input & IN_FORWARD)
            && Lara_FloorFront(item, item->pos.y_rot, STEP_L)
                >= -STEPUP_HEIGHT) {
            item->goal_anim_state = LS_FORWARD_JUMP;
            g_Lara.move_angle = item->pos.y_rot;
        } else if (
            (g_Input & IN_LEFT)
            && Lara_FloorFront(item, item->pos.y_rot - PHD_90, STEP_L)
                >= -STEPUP_HEIGHT) {
            item->goal_anim_state = LS_LEFT_JUMP;
            g_Lara.move_angle = item->pos.y_rot - PHD_90;
        } else if (
            (g_Input & IN_RIGHT)
            && Lara_FloorFront(item, item->pos.y_rot + PHD_90, STEP_L)
                >= -STEPUP_HEIGHT) {
            item->goal_anim_state = LS_RIGHT_JUMP;
            g_Lara.move_angle = item->pos.y_rot + PHD_90;
        } else if (
            (g_Input & IN_BACK)
            && Lara_FloorFront(item, item->pos.y_rot + PHD_180, STEP_L)
                >= -STEPUP_HEIGHT) {
            item->goal_anim_state = LS_BACK_JUMP;
            g_Lara.move_angle = item->pos.y_rot + PHD_180;
        }
    }

    if (item->fall_speed > LARA_FAST_FALL_SPEED) {
        item->goal_anim_state = LS_FAST_FALL;
    }
}

void __cdecl Lara_State_Back(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = LS_STOP;
        return;
    }

    if ((g_Input & IN_BACK)
        && ((g_Input & IN_SLOW) || g_Lara.water_status == LWS_WADE)) {
        item->goal_anim_state = LS_BACK;
    } else {
        item->goal_anim_state = LS_STOP;
    }

    if (g_Input & IN_LEFT) {
        g_Lara.turn_rate -= LARA_TURN_RATE;
        CLAMPL(g_Lara.turn_rate, -LARA_SLOW_TURN);
    } else if (g_Input & IN_RIGHT) {
        g_Lara.turn_rate += LARA_TURN_RATE;
        CLAMPG(g_Lara.turn_rate, LARA_SLOW_TURN);
    }
}

void __cdecl Lara_State_Null(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
}

void __cdecl Lara_State_FastTurn(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = LS_STOP;
        return;
    }

    if (g_Lara.turn_rate >= 0) {
        g_Lara.turn_rate = LARA_FAST_TURN;
        if (!(g_Input & IN_RIGHT)) {
            item->goal_anim_state = LS_STOP;
        }
    } else {
        g_Lara.turn_rate = -LARA_FAST_TURN;
        if (!(g_Input & IN_LEFT)) {
            item->goal_anim_state = LS_STOP;
        }
    }
}

void __cdecl Lara_State_StepRight(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.look = 0;
    if (item->hit_points <= 0) {
        item->goal_anim_state = LS_STOP;
        return;
    }

    if (!(g_Input & IN_STEP_RIGHT)) {
        item->goal_anim_state = LS_STOP;
    }

    if (g_Input & IN_LEFT) {
        g_Lara.turn_rate -= LARA_TURN_RATE;
        CLAMPL(g_Lara.turn_rate, -LARA_SLOW_TURN);
    } else if (g_Input & IN_RIGHT) {
        g_Lara.turn_rate += LARA_TURN_RATE;
        CLAMPG(g_Lara.turn_rate, LARA_SLOW_TURN);
    }
}

void __cdecl Lara_State_StepLeft(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.look = 0;
    if (item->hit_points <= 0) {
        item->goal_anim_state = LS_STOP;
        return;
    }

    if (!(g_Input & IN_STEP_LEFT)) {
        item->goal_anim_state = LS_STOP;
    }

    if (g_Input & IN_LEFT) {
        g_Lara.turn_rate -= LARA_TURN_RATE;
        CLAMPL(g_Lara.turn_rate, -LARA_SLOW_TURN);
    } else if (g_Input & IN_RIGHT) {
        g_Lara.turn_rate += LARA_TURN_RATE;
        CLAMPG(g_Lara.turn_rate, LARA_SLOW_TURN);
    }
}

void __cdecl Lara_State_Slide(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Camera.flags = CF_NO_CHUNKY;
    g_Camera.target_elevation = CAM_SLIDE_ELEVATION;
    if ((g_Input & IN_JUMP) && !(g_Input & IN_BACK)) {
        item->goal_anim_state = LS_FORWARD_JUMP;
    }
}

void __cdecl Lara_State_BackJump(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Camera.target_angle = CAM_BACK_JUMP_ANGLE;
    if (item->fall_speed > LARA_FAST_FALL_SPEED) {
        item->goal_anim_state = LS_FAST_FALL;
        return;
    }

    if (item->goal_anim_state == LS_RUN) {
        item->goal_anim_state = LS_STOP;
    } else if (
        ((g_Input & IN_FORWARD) || (g_Input & IN_ROLL))
        && item->goal_anim_state != LS_STOP) {
        item->goal_anim_state = LS_TWIST;
    }
}

void __cdecl Lara_State_RightJump(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.look = 0;
    if (item->fall_speed > LARA_FAST_FALL_SPEED) {
        item->goal_anim_state = LS_FAST_FALL;
        return;
    }

    if ((g_Input & IN_LEFT) && item->goal_anim_state != LS_STOP) {
        item->goal_anim_state = LS_TWIST;
    }
}

void __cdecl Lara_State_LeftJump(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.look = 0;
    if (item->fall_speed > LARA_FAST_FALL_SPEED) {
        item->goal_anim_state = LS_FAST_FALL;
        return;
    }

    if ((g_Input & IN_RIGHT) && item->goal_anim_state != LS_STOP) {
        item->goal_anim_state = LS_TWIST;
    }
}

void __cdecl Lara_State_UpJump(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (item->fall_speed > LARA_FAST_FALL_SPEED) {
        item->goal_anim_state = LS_FAST_FALL;
    }
}

void __cdecl Lara_State_Fallback(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (item->fall_speed > LARA_FAST_FALL_SPEED) {
        item->goal_anim_state = LS_FAST_FALL;
        return;
    }

    if ((g_Input & IN_ACTION) && g_Lara.gun_status == LGS_ARMLESS) {
        item->goal_anim_state = LS_REACH;
    }
}

void __cdecl Lara_State_HangLeft(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    g_Camera.target_angle = CAM_HANG_ANGLE;
    g_Camera.target_elevation = CAM_HANG_ELEVATION;
    if (!(g_Input & IN_LEFT) && !(g_Input & IN_STEP_LEFT)) {
        item->goal_anim_state = LS_HANG;
    }
}

void __cdecl Lara_State_HangRight(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    g_Camera.target_angle = CAM_HANG_ANGLE;
    g_Camera.target_elevation = CAM_HANG_ELEVATION;
    if (!(g_Input & IN_RIGHT) && !(g_Input & IN_STEP_RIGHT)) {
        item->goal_anim_state = LS_HANG;
    }
}

void __cdecl Lara_State_SlideBack(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if ((g_Input & IN_JUMP) && !(g_Input & IN_FORWARD)) {
        item->goal_anim_state = LS_BACK_JUMP;
    }
}

void __cdecl Lara_State_PushBlock(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.look = 0;
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    g_Camera.flags = CF_FOLLOW_CENTRE;
    g_Camera.target_angle = CAM_PUSH_BLOCK_ANGLE;
    g_Camera.target_elevation = CAM_PUSH_BLOCK_ELEVATION;
}

void __cdecl Lara_State_PPReady(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    g_Camera.target_angle = CAM_PP_READY_ANGLE;
    if (!(g_Input & IN_ACTION)) {
        item->goal_anim_state = LS_STOP;
    }
}

void __cdecl Lara_State_Pickup(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.look = 0;
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    g_Camera.target_angle = CAM_PICKUP_ANGLE;
    g_Camera.target_elevation = CAM_PICKUP_ELEVATION;
    g_Camera.target_distance = CAM_PICKUP_DISTANCE;
}

void __cdecl Lara_State_PickupFlare(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    Lara_State_Pickup(item, coll);
    if (item->frame_num == g_Anims[item->anim_num].frame_end - 1) {
        g_Lara.gun_status = LGS_ARMLESS;
    }
}

void __cdecl Lara_State_SwitchOn(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.look = 0;
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    g_Camera.target_angle = CAM_SWITCH_ON_ANGLE;
    g_Camera.target_elevation = CAM_SWITCH_ON_ELEVATION;
    g_Camera.target_distance = CAM_SWITCH_ON_DISTANCE;
    g_Camera.speed = CAM_SWITCH_ON_SPEED;
}

void __cdecl Lara_State_UseKey(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Lara.look = 0;
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    g_Camera.target_angle = CAM_USE_KEY_ANGLE;
    g_Camera.target_elevation = CAM_USE_KEY_ELEVATION;
    g_Camera.target_distance = CAM_USE_KEY_DISTANCE;
}

void __cdecl Lara_State_Special(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Camera.flags = CF_FOLLOW_CENTRE;
    g_Camera.target_angle = CAM_SPECIAL_ANGLE;
    g_Camera.target_elevation = CAM_SPECIAL_ELEVATION;
}

void __cdecl Lara_State_SwanDive(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 1;
    if (item->fall_speed > LARA_FAST_FALL_SPEED
        && item->goal_anim_state != LS_DIVE) {
        item->goal_anim_state = LS_FAST_DIVE;
    }
}

void __cdecl Lara_State_FastDive(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if ((g_Input & IN_ROLL) && item->goal_anim_state == LS_FAST_DIVE) {
        item->goal_anim_state = LS_TWIST;
    }
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 1;
    item->speed = item->speed * 95 / 100;
}

void __cdecl Lara_State_WaterOut(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    coll->enable_spaz = 0;
    coll->enable_baddie_push = 0;
    g_Camera.flags = CF_FOLLOW_CENTRE;
}

void __cdecl Lara_State_Wade(struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    if (item->hit_points <= 0) {
        item->goal_anim_state = LS_STOP;
        return;
    }

    g_Camera.target_elevation = CAM_WADE_ELEVATION;
    if (g_Input & IN_LEFT) {
        g_Lara.turn_rate -= LARA_TURN_RATE;
        CLAMPL(g_Lara.turn_rate, -LARA_FAST_TURN);
        item->pos.z_rot -= LARA_LEAN_RATE;
        CLAMPL(item->pos.z_rot, -LARA_LEAN_MAX);
    } else if (g_Input & IN_RIGHT) {
        g_Lara.turn_rate += LARA_TURN_RATE;
        CLAMPG(g_Lara.turn_rate, LARA_FAST_TURN);
        item->pos.z_rot += LARA_LEAN_RATE;
        CLAMPG(item->pos.z_rot, LARA_LEAN_MAX);
    }

    if (g_Input & IN_FORWARD) {
        if (g_Lara.water_status != LWS_ABOVE_WATER) {
            item->goal_anim_state = LS_WADE;
        } else {
            item->goal_anim_state = IN_FORWARD;
        }
    } else {
        item->goal_anim_state = LS_STOP;
    }
}

void __cdecl Lara_State_DeathSlide(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Camera.target_angle = CAM_DEATH_SLIDE_ANGLE;

    int16_t room_num = item->room_num;
    struct FLOOR_INFO *floor =
        Room_GetFloor(item->pos.x, item->pos.y, item->pos.z, &room_num);
    Room_GetHeight(floor, item->pos.x, item->pos.y, item->pos.z);

    coll->trigger = g_TriggerIndex;

    if (!(g_Input & IN_ACTION)) {
        item->goal_anim_state = LS_FORWARD_JUMP;
        Lara_Animate(item);
        g_LaraItem->gravity = 1;
        g_LaraItem->speed = 100;
        g_LaraItem->fall_speed = 40;
        g_Lara.move_angle = item->pos.y_rot;
    }
}

void __cdecl Lara_State_Extra_Breath(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    item->anim_num = LA_BREATH;
    item->frame_num = g_Anims[LA_BREATH].frame_base;
    item->goal_anim_state = LS_STOP;
    item->current_anim_state = LS_STOP;
    g_Lara.extra_anim = 0;
    g_Lara.gun_status = LGS_ARMLESS;
    g_Camera.type = CAM_CHASE;
    Viewport_AlterFOV(GAME_FOV * PHD_DEGREE);
}

void __cdecl Lara_State_Extra_YetiKill(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Camera.target_angle = CAM_YETI_KILL_ANGLE;
    g_Camera.target_distance = CAM_YETI_KILL_DISTANCE;
    g_Lara.hit_direction = -1;
    if (item->frame_num < g_Anims[item->anim_num].frame_end - 30) {
        g_Lara.death_count = 1;
    }
}

void __cdecl Lara_State_Extra_SharkKill(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Camera.target_angle = CAM_SHARK_KILL_ANGLE;
    g_Camera.target_distance = CAM_SHARK_KILL_DISTANCE;
    g_Lara.hit_direction = -1;

    if (item->frame_num == g_Anims[item->anim_num].frame_end) {
        int32_t water_height = Room_GetWaterHeight(
            item->pos.x, item->pos.y, item->pos.z, item->room_num);
        if (water_height != NO_HEIGHT && water_height < item->pos.y - 100) {
            item->pos.y -= 5;
        }
    }

    if (item->frame_num < g_Anims[item->anim_num].frame_end - 30) {
        g_Lara.death_count = 1;
    }
}

void __cdecl Lara_State_Extra_Airlock(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Camera.target_angle = CAM_AIRLOCK_ANGLE;
    g_Camera.target_elevation = CAM_AIRLOCK_ELEVATION;
}

void __cdecl Lara_State_Extra_GongBong(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Camera.target_angle = CAM_GONG_BONG_ANGLE;
    g_Camera.target_elevation = CAM_GONG_BONG_ELEVATION;
    g_Camera.target_distance = CAM_GONG_BONG_DISTANCE;
}

void __cdecl Lara_State_Extra_DinoKill(
    struct ITEM_INFO *item, struct COLL_INFO *coll)
{
    g_Camera.flags = CF_FOLLOW_CENTRE;
    g_Camera.target_angle = CAM_DINO_KILL_ANGLE;
    g_Camera.target_elevation = CAM_DINO_KILL_ELEVATION;
    g_Lara.hit_direction = -1;
    if (item->frame_num < g_Anims[item->anim_num].frame_end - 30) {
        g_Lara.death_count = 1;
    }
}
