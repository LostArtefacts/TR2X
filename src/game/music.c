#include "game/music.h"

#include "specific/s_music.h"

uint32_t __cdecl Music_GetFrames(void)
{
    return S_Music_GetFrames();
}

void __cdecl Music_SetVolume(int32_t volume)
{
    S_Music_SetVolume(volume);
}
