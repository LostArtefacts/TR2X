#pragma once

#include "global/types.h"

void __cdecl Lara_Col_Walk(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_Run(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_Stop(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_ForwardJump(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_FastBack(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_TurnRight(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_TurnLeft(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_Death(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_FastFall(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_Hang(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_Reach(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_Splat(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_Land(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_Compress(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Back(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_StepRight(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_StepLeft(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_Slide(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_BackJump(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_RightJump(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_LeftJump(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_UpJump(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_Fallback(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_HangLeft(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_HangRight(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_SlideBack(struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_Col_Null(struct ITEM_INFO *item, struct COLL_INFO *coll);
