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

int16_t __cdecl Effect_Create(const int16_t room_num)
{
    int16_t fx_num = g_NextEffectFree;
    if (fx_num == NO_ITEM) {
        return NO_ITEM;
    }

    struct FX_INFO *const fx = &g_Effects[fx_num];
    struct ROOM_INFO *const room = &g_Rooms[room_num];
    fx->room_num = room_num;
    fx->next_active = g_NextEffectActive;
    fx->next_fx = room->fx_num;
    room->fx_num = fx_num;

    g_NextEffectActive = fx_num;
    g_NextEffectFree = fx->next_fx;

    fx->shade = 0x1000;

    return fx_num;
}
