#pragma once

// Lara state routines.

#include "global/types.h"

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
