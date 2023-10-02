#pragma once

#include "global/types.h"

void __cdecl Camera_Initialise(void);
void __cdecl Camera_Move(struct GAME_VECTOR *target, int32_t speed);
