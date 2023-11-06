#pragma once

#include "global/types.h"

void __cdecl Item_InitialiseArray(int32_t num_items);
void __cdecl Item_Kill(int16_t item_num);

bool Item_IsSmashable(const struct ITEM_INFO *item);
