#pragma once

#include "global/types.h"

void __cdecl Item_InitialiseArray(int32_t num_items);
int16_t __cdecl Item_Create(void);
void __cdecl Item_Kill(int16_t item_num);
void __cdecl Item_Initialise(int16_t item_num);
void __cdecl Item_RemoveActive(int16_t item_num);
void __cdecl Item_RemoveDrawn(int16_t item_num);
void __cdecl Item_AddActive(int16_t item_num);
void __cdecl Item_NewRoom(int16_t item_num, int16_t room_num);

bool Item_IsSmashable(const struct ITEM_INFO *item);
