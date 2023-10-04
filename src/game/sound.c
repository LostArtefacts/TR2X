#include "game/sound.h"

#include "global/vars.h"

void __cdecl Sound_SetMasterVolume(int32_t volume)
{
    g_MasterVolume = volume;
}
