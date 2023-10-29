#pragma once

#include <stdint.h>

void __cdecl Output_InsertPolygons(const int16_t *obj_ptr, int32_t clip);
void __cdecl Output_InsertRoom(const int16_t *obj_ptr, int32_t is_outside);
const int16_t *__cdecl Output_CalcSkyboxLight(const int16_t *obj_ptr);
