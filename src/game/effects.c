#include "game/effects.h"

#include "global/const.h"
#include "global/vars.h"

void __cdecl Effect_InitialiseArray(void)
{
    g_NextEffectFree = 0;
    g_NextEffectActive = NO_ITEM;

    for (int i = 0; i < MAX_EFFECTS - 1; i++) {
        struct FX_INFO *const fx = &g_Effects[i];
        fx->next_fx = i + 1;
    }
    g_Effects[MAX_EFFECTS - 1].next_fx = NO_ITEM;
}
