#pragma once

#include "global/types.h"

void __cdecl Inv_Ring_Init(
    RING_INFO *ring, RING_TYPE type, INVENTORY_ITEM **list, int16_t qty,
    int16_t current, IMOTION_INFO *imo);
