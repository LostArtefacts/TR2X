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
