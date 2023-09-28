#pragma once

#include "global/types.h"

#include <stdint.h>

struct PHD_3DPOS;

void __cdecl Matrix_GenerateW2V(struct PHD_3DPOS *viewpos);
void __cdecl Matrix_LookAt(
    int32_t xsrc, int32_t ysrc, int32_t zsrc, int32_t xtar, int32_t ytar,
    int32_t ztar, int16_t roll);
void __cdecl Matrix_RotX(PHD_ANGLE rx);
void __cdecl Matrix_RotY(PHD_ANGLE ry);
void __cdecl Matrix_RotZ(PHD_ANGLE rz);
void __cdecl Matrix_RotYXZ(int16_t ry, int16_t rx, int16_t rz);
