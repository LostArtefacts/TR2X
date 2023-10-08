#pragma once

#include "global/types.h"

void __cdecl Lara_GetCollisionInfo(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_SlideSlope(struct ITEM_INFO *item, struct COLL_INFO *coll);
int32_t __cdecl Lara_HitCeiling(struct ITEM_INFO *item, struct COLL_INFO *coll);
int32_t __cdecl Lara_DeflectEdge(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_DeflectEdgeJump(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_SlideEdgeJump(struct ITEM_INFO *item, struct COLL_INFO *coll);
int32_t __cdecl Lara_TestWall(
    struct ITEM_INFO *item, int32_t front, int32_t right, int32_t down);
int32_t __cdecl Lara_TestHangOnClimbWall(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
int32_t __cdecl Lara_TestClimbStance(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_HangTest(struct ITEM_INFO *item, struct COLL_INFO *coll);
int32_t __cdecl Lara_TestEdgeCatch(
    struct ITEM_INFO *item, struct COLL_INFO *coll, int32_t *edge);
int32_t __cdecl Lara_TestHangJumpUp(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
int32_t __cdecl Lara_TestHangJump(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
int32_t __cdecl Lara_TestHangSwingIn(struct ITEM_INFO *item, PHD_ANGLE angle);
int32_t __cdecl Lara_TestVault(struct ITEM_INFO *item, struct COLL_INFO *coll);
int32_t __cdecl Lara_TestSlide(struct ITEM_INFO *item, struct COLL_INFO *coll);
int16_t __cdecl Lara_FloorFront(
    struct ITEM_INFO *item, int16_t ang, int32_t dist);
int32_t __cdecl Lara_LandedBad(struct ITEM_INFO *item, struct COLL_INFO *coll);
