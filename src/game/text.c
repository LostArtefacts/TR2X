#include "game/text.h"

#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

#include <assert.h>
#include <string.h>

#define TEXT_MAX_STRINGS 64
#define TEXT_MAX_STRING_SIZE 64

void __cdecl Text_Init(void)
{
    DisplayModeInfo(0);
    for (int i = 0; i < TEXT_MAX_STRINGS; i++) {
        g_TextstringTable[i].flags.all = 0;
    }
    g_TextstringCount = 0;
}

struct TEXTSTRING *__cdecl Text_Create(
    const int32_t x, const int32_t y, const int32_t z, const char *const text)
{
    if (text == NULL) {
        return NULL;
    }

    struct TEXTSTRING *result = NULL;
    for (int i = 0; i < TEXT_MAX_STRINGS; i++) {
        struct TEXTSTRING *const string = &g_TextstringTable[i];
        if (!string->flags.active) {
            result = string;
            break;
        }
    }

    if (!result) {
        return NULL;
    }

    const int32_t i = result - g_TextstringTable;
    result->text = g_TextstringBuffers[i];
    result->scale.h = PHD_ONE;
    result->scale.v = PHD_ONE;
    result->pos.x = (x * Text_GetScaleH(PHD_ONE)) / PHD_ONE;
    result->pos.y = (y * Text_GetScaleV(PHD_ONE)) / PHD_ONE;
    result->pos.z = z;

    result->letter_spacing = 1;
    result->word_spacing = 6;

    result->text_flags = 0;
    result->outl_flags = 0;
    result->bgnd_flags = 0;
    result->bgnd_size.x = 0;
    result->bgnd_size.y = 0;
    result->bgnd_off.x = 0;
    result->bgnd_off.y = 0;
    result->bgnd_off.z = 0;
    result->flags.active = 1;
    g_TextstringCount++;

    strncpy(result->text, text, TEXT_MAX_STRING_SIZE);
    result->text[TEXT_MAX_STRING_SIZE - 1] = '\0';

    return result;
}

void __cdecl Text_ChangeText(
    struct TEXTSTRING *const string, const char *const text)
{
    assert(string);
    assert(text);
    if (string->flags.active) {
        strncpy(string->text, text, TEXT_MAX_STRING_SIZE);
        string->text[TEXT_MAX_STRING_SIZE - 1] = '\0';
    }
}

void __cdecl Text_SetScale(
    struct TEXTSTRING *const string, const int32_t scale_h,
    const int32_t scale_v)
{
    assert(string);
    string->scale.h = scale_h;
    string->scale.v = scale_v;
}

void __cdecl Text_Flash(
    struct TEXTSTRING *const string, const int16_t enable, const int16_t rate)
{
    assert(string);
    if (enable) {
        string->flags.flash = 1;
        string->flash.rate = rate;
        string->flash.count = rate;
    } else {
        string->flags.flash = 0;
    }
}

void __cdecl Text_AddBackground(
    struct TEXTSTRING *const string, const int16_t x_size, const int16_t y_size,
    const int16_t x_off, const int16_t y_off, const int16_t z_off,
    const int16_t colour, const uint16_t *const gour_ptr, const uint16_t flags)
{
    assert(string);

    uint32_t scale_h = Text_GetScaleH(string->scale.h);
    uint32_t scale_v = Text_GetScaleV(string->scale.v);
    string->flags.background = 1;
    string->bgnd_size.x = (scale_h * x_size) / PHD_ONE;
    string->bgnd_size.y = (scale_v * y_size) / PHD_ONE;
    string->bgnd_off.x = (scale_h * x_off) / PHD_ONE;
    string->bgnd_off.y = (scale_v * y_off) / PHD_ONE;
    string->bgnd_off.z = z_off;
    string->bgnd_colour = colour;
    string->bgnd_gour = gour_ptr;
    string->bgnd_flags = flags;
}

void __cdecl Text_RemoveBackground(struct TEXTSTRING *const string)
{
    assert(string);
    string->flags.background = 0;
}

void __cdecl Text_AddOutline(
    struct TEXTSTRING *const string, const int16_t enable, const int16_t colour,
    const uint16_t *const gour_ptr, const uint16_t flags)
{
    assert(string);
    string->flags.outline = 1;
    string->outl_gour = gour_ptr;
    string->outl_colour = colour;
    string->outl_flags = flags;
}

void __cdecl Text_RemoveOutline(struct TEXTSTRING *const string)
{
    assert(string);
    string->flags.outline = 0;
}
