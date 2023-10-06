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
