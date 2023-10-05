#include "game/music.h"

#include "specific/s_music.h"

bool __cdecl Music_Init(void)
{
    return S_Music_Init();
}

void __cdecl Music_Shutdown(void)
{
    S_Music_Shutdown();
}

void __cdecl Music_Play(int16_t track_id, bool is_looped)
{
    S_Music_Play(track_id, is_looped);
}

void __cdecl Music_Stop(void)
{
    S_Music_Stop();
}

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
