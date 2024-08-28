#pragma once

#include "global/types.h"

void __cdecl Option_DoInventory(INVENTORY_ITEM *item);
void __cdecl Option_ShutdownInventory(INVENTORY_ITEM *item);

void __cdecl Option_Passport(INVENTORY_ITEM *item);
void __cdecl Option_Passport_Shutdown(INVENTORY_ITEM *item);

void __cdecl Option_Detail_Shutdown(INVENTORY_ITEM *item);

void __cdecl Option_Sound_Shutdown(INVENTORY_ITEM *item);

void __cdecl Option_Controls_Shutdown(INVENTORY_ITEM *item);

void __cdecl Option_Compass_Shutdown(INVENTORY_ITEM *item);
