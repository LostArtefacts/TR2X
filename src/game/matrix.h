#pragma once

#include "global/types.h"

#include <stdint.h>

struct PHD_3DPOS;

void __cdecl Matrix_GenerateW2V(struct PHD_3DPOS *viewpos);
void __cdecl Matrix_LookAt(
    int32_t xsrc, int32_t ysrc, int32_t zsrc, int32_t xtar, int32_t ytar,
    int32_t ztar, int16_t roll);
