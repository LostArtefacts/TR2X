#pragma once

#include "global/types.h"

#include <stdint.h>

int32_t __fastcall Math_Cos(int32_t angle);
int32_t __fastcall Math_Sin(int32_t angle);
int32_t __fastcall Math_Atan(int32_t x, int32_t y);
uint32_t __fastcall Math_Sqrt(uint32_t n);

enum DIRECTION Math_GetDirection(int16_t angle);
enum DIRECTION Math_GetDirectionCone(int16_t angle, int16_t cone);
int16_t Math_DirectionToAngle(enum DIRECTION dir);
