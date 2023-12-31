#pragma once

// Lara state routines.

#include "global/types.h"

// TODO: make static
void __cdecl Lara_SwimTurn(struct ITEM_INFO *const item);

void __cdecl Lara_State_Walk(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Run(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Stop(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_ForwardJump(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_FastBack(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_TurnRight(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_TurnLeft(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Death(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_FastFall(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Hang(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Reach(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Splat(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Compress(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Back(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Null(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_FastTurn(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_StepRight(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_StepLeft(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Slide(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_BackJump(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_RightJump(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_LeftJump(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_UpJump(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Fallback(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_HangLeft(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_HangRight(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_SlideBack(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_PushBlock(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_PPReady(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Pickup(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_PickupFlare(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_SwitchOn(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_UseKey(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Special(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_SwanDive(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_FastDive(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_WaterOut(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Wade(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_DeathSlide(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Extra_Breath(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Extra_YetiKill(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Extra_SharkKill(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Extra_Airlock(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Extra_GongBong(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Extra_DinoKill(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Extra_PullDagger(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Extra_StartAnim(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Extra_StartHouse(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Extra_FinalAnim(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_ClimbLeft(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_ClimbRight(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_ClimbStance(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Climbing(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_ClimbEnd(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_ClimbDown(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_SurfSwim(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_SurfBack(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_SurfLeft(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_SurfRight(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_SurfTread(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Swim(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Glide(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Tread(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_Dive(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_UWDeath(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_State_UWTwist(struct ITEM_INFO *item, struct COLL_INFO *coll);
