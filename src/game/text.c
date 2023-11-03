#include "game/text.h"

#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

#include <assert.h>
#include <string.h>

#define TEXT_MAX_STRINGS 64
#define TEXT_MAX_STRING_SIZE 64
#define CHAR_SECRET_1 0x7Fu
#define CHAR_SECRET_2 0x80u
#define CHAR_SECRET_3 0x81u
#define IS_CHAR_LEGAL(c) ((c) <= CHAR_SECRET_3 && ((c) <= 10u || (c) >= 32u))
#define IS_CHAR_SECRET(c) ((c) >= CHAR_SECRET_1 && (c) <= CHAR_SECRET_3)
#define IS_CHAR_DIACRITIC(c)                                                   \
    ((c) == '(' || (c) == ')' || (c) == '$' || (c) == '~')
#define IS_CHAR_SPACE(c) ((c) == 32)
#define IS_CHAR_DIGIT(c) ((c) <= 0xAu)

// TODO: replace textstring == NULL checks with assertions

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

    if (result == NULL) {
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
    if (string == NULL) {
        return;
    }
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
    if (string == NULL) {
        return;
    }
    string->scale.h = scale_h;
    string->scale.v = scale_v;
}

void __cdecl Text_Flash(
    struct TEXTSTRING *const string, const int16_t enable, const int16_t rate)
{
    if (string == NULL) {
        return;
    }
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
    if (string == NULL) {
        return;
    }
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
    if (string == NULL) {
        return;
    }
    string->flags.background = 0;
}

void __cdecl Text_AddOutline(
    struct TEXTSTRING *const string, const int16_t enable, const int16_t colour,
    const uint16_t *const gour_ptr, const uint16_t flags)
{
    if (string == NULL) {
        return;
    }
    string->flags.outline = 1;
    string->outl_gour = gour_ptr;
    string->outl_colour = colour;
    string->outl_flags = flags;
}

void __cdecl Text_RemoveOutline(struct TEXTSTRING *const string)
{
    if (string == NULL) {
        return;
    }
    string->flags.outline = 0;
}

void __cdecl Text_CentreH(struct TEXTSTRING *const string, const int16_t enable)
{
    if (string == NULL) {
        return;
    }
    string->flags.centre_h = enable;
}

void __cdecl Text_CentreV(struct TEXTSTRING *const string, const int16_t enable)
{
    if (string == NULL) {
        return;
    }
    string->flags.centre_v = enable;
}

void __cdecl Text_AlignRight(
    struct TEXTSTRING *const string, const int16_t enable)
{
    if (string == NULL) {
        return;
    }
    string->flags.right = enable;
}

void __cdecl Text_AlignBottom(
    struct TEXTSTRING *const string, const int16_t enable)
{
    if (string == NULL) {
        return;
    }
    string->flags.bottom = enable;
}

int32_t __cdecl Text_Remove(struct TEXTSTRING *const string)
{
    if (string == NULL) {
        return 0;
    }
    if (!string->flags.active) {
        return 0;
    }
    string->flags.active = 0;
    g_TextstringCount--;
    return 1;
}

int32_t __cdecl Text_GetWidth(struct TEXTSTRING *const string)
{
    if (string == NULL) {
        return 0;
    }

    const uint32_t scale_h = Text_GetScaleH(string->scale.h);
    const char *str = string->text;
    int32_t width = 0;

    while (1) {
        uint8_t c = *str++;
        if (!c) {
            break;
        }

        if (!IS_CHAR_LEGAL(c) || IS_CHAR_DIACRITIC(c)) {
            continue;
        }

        int32_t spacing;
        if (IS_CHAR_SPACE(c)) {
            spacing = string->word_spacing;
        } else if (IS_CHAR_SECRET(c)) {
            spacing = 16;
        } else {
            int16_t sprite_num;
            if (IS_CHAR_DIGIT(c)) {
                sprite_num = c + 81;
            } else {
                sprite_num = g_TextASCIIMap[c];
            }

            // TODO: OG bug - this should check c, not sprite_num
            if (sprite_num >= '0' && sprite_num <= '9') {
                spacing = 12;
            } else {
                // TODO: OG bug - wrong letter spacing calculation
                spacing = g_TextSpacing[sprite_num] + string->letter_spacing;
            }
        }

        width += spacing * scale_h / PHD_ONE;
    }

    // TODO: OG bug - wrong letter spacing calculation; pointless ~1
    return ((int16_t)width - string->letter_spacing) & ~1;
}
