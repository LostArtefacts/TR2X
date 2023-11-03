#pragma once

#include "global/types.h"

void __cdecl Text_Init(void);
struct TEXTSTRING *__cdecl Text_Create(
    int32_t x, int32_t y, int32_t z, const char *string);
void __cdecl Text_ChangeText(struct TEXTSTRING *string, const char *text);
void __cdecl Text_SetScale(
    struct TEXTSTRING *string, int32_t scale_h, int32_t scale_v);
void __cdecl Text_Flash(
    struct TEXTSTRING *string, int16_t enable, int16_t rate);
void __cdecl Text_AddBackground(
    struct TEXTSTRING *string, int16_t x_size, int16_t y_size, int16_t x_off,
    int16_t y_off, int16_t z_off, int16_t colour, const uint16_t *gour_ptr,
    uint16_t flags);
