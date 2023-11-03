#pragma once

#include "global/types.h"

void __cdecl Text_Init(void);
struct TEXTSTRING *__cdecl Text_Create(
    int32_t x, int32_t y, int32_t z, const char *string);
void __cdecl Text_ChangeText(struct TEXTSTRING *string, const char *text);
