#pragma once

#include "global/types.h"

void __cdecl Lara_GetLaraCollisionInfo(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_SlideSlope(struct ITEM_INFO *item, struct COLL_INFO *coll);
int32_t __cdecl Lara_HitCeiling(struct ITEM_INFO *item, struct COLL_INFO *coll);
