#pragma once

#include "global/types.h"

void __cdecl Music_Shutdown(void);
void __cdecl Music_Play(int16_t track_id, bool is_looped);
void __cdecl Music_Stop(void);
bool __cdecl Music_PlaySynced(int32_t track_id);
uint32_t __cdecl Music_GetFrames(void);
void __cdecl Music_SetVolume(int32_t volume);
