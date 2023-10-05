#include "game/music.h"

#include "specific/s_music.h"

bool __cdecl Music_PlaySynced(int32_t track_id)
{
    return S_Music_PlaySynced(track_id);
}

uint32_t __cdecl Music_GetFrames(void)
{
    return S_Music_GetFrames();
}

void __cdecl Music_SetVolume(int32_t volume)
{
    S_Music_SetVolume(volume);
}
