#pragma once

#include "global/types.h"

bool __cdecl Overlay_FlashCounter(void);
void __cdecl Overlay_DrawAssaultTimer(void);
void __cdecl Overlay_DrawGameInfo(bool pickup_state);
void __cdecl Overlay_DrawHealthBar(bool flash_state);
void __cdecl Overlay_DrawAirBar(bool flash_state);
void __cdecl Overlay_MakeAmmoString(char *string);
void __cdecl Overlay_DrawAmmoInfo(void);
void __cdecl Overlay_InitialisePickUpDisplay(void);
