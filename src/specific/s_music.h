#pragma once

#include "global/types.h"

void __cdecl S_Music_Play(int16_t track_id, bool is_looped);
void __cdecl S_Music_Stop(void);
bool __cdecl S_Music_PlaySynced(int32_t track_id);
uint32_t __cdecl S_Music_GetFrames(void);
void __cdecl S_Music_SetVolume(int32_t volume);
