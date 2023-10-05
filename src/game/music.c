#include "game/music.h"

#include "specific/s_music.h"

void __cdecl Music_SetVolume(int32_t volume)
{
    S_Music_SetVolume(volume);
}
