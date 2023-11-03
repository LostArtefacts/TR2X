#pragma once

#include "global/types.h"

int32_t __cdecl LOS_CheckZ(
    struct GAME_VECTOR *const start, struct GAME_VECTOR *const target);
int32_t __cdecl LOS_Check(
    struct GAME_VECTOR *start, struct GAME_VECTOR *target);
