#pragma once

#include "global/types.h"

void __cdecl Item_InitialiseArray(int32_t num_items);

bool Item_IsSmashable(const struct ITEM_INFO *item);
