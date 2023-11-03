#pragma once

#include "global/types.h"

int32_t __cdecl LOS_CheckX(
    const struct GAME_VECTOR *start, struct GAME_VECTOR *target);
int32_t __cdecl LOS_CheckZ(
    const struct GAME_VECTOR *start, struct GAME_VECTOR *target);
int32_t __cdecl LOS_ClipTarget(
    const struct GAME_VECTOR *start, struct GAME_VECTOR *target,
    const FLOOR_INFO *floor);
int32_t __cdecl LOS_Check(
    const struct GAME_VECTOR *start, struct GAME_VECTOR *target);
int32_t __cdecl LOS_CheckSmashable(
    const struct GAME_VECTOR *start, struct GAME_VECTOR *target);
