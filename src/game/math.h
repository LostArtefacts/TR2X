#pragma once

#include <stdint.h>

#define Math_Cos ((int32_t __fastcall(*)(int16_t angle))0x00457C58)
#define Math_Sin ((int32_t __fastcall(*)(int16_t angle))0x00457C5E)

int32_t __fastcall Math_Atan(int32_t x, int32_t y);
