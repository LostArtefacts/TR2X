#pragma once

#include "global/types.h"

void __cdecl Camera_Initialise(void);
void __cdecl Camera_Move(struct GAME_VECTOR *target, int32_t speed);
void __cdecl Camera_Clip(
    int32_t *x, int32_t *y, int32_t *h, int32_t target_x, int32_t target_y,
    int32_t target_h, int32_t left, int32_t top, int32_t right, int32_t bottom);
