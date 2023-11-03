#include "game/text.h"

#include "global/funcs.h"
#include "global/vars.h"

#define TEXT_MAX_STRINGS 64

void __cdecl Text_Init(void)
{
    DisplayModeInfo(0);
    for (int i = 0; i < TEXT_MAX_STRINGS; i++) {
        g_TextstringTable[i].flags.all = 0;
    }
    g_TextstringCount = 0;
}
