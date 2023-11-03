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
