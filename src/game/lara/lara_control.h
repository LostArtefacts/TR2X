#pragma once

#include "global/types.h"

void __cdecl Lara_HandleAboveWater(
    struct ITEM_INFO *item, struct COLL_INFO *coll);
void __cdecl Lara_HandleSurface(struct ITEM_INFO *item, struct COLL_INFO *coll);
