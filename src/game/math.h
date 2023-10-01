#pragma once

#include <stdint.h>

#define Math_Sin ((int32_t __fastcall(*)(int16_t angle))0x00457C5E)

int32_t __fastcall Math_Cos(int16_t angle);
int32_t __fastcall Math_Atan(int32_t x, int32_t y);
