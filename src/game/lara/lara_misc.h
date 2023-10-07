#pragma once

#include "global/types.h"

void __cdecl Lara_GetLaraCollisionInfo(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_SlideSlope(struct ITEM_INFO *item, struct COLL_INFO *coll);
