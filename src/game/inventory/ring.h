#pragma once

#include "global/types.h"

void __cdecl Inv_Ring_Init(
    RING_INFO *ring, RING_TYPE type, INVENTORY_ITEM **list, int16_t qty,
    int16_t current, IMOTION_INFO *imo);
void __cdecl Inv_Ring_GetView(const RING_INFO *ring, PHD_3DPOS *view);
void __cdecl Inv_Ring_Light(const RING_INFO *ring);
void __cdecl Inv_Ring_CalcAdders(RING_INFO *ring, int16_t rotation_duration);
void __cdecl Inv_Ring_DoMotions(RING_INFO *ring);
void __cdecl Inv_Ring_RotateLeft(RING_INFO *ring);
