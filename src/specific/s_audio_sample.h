#pragma once

#include "global/types.h"

const struct SOUND_ADAPTER_NODE *__cdecl S_Audio_Sample_GetAdapter(GUID *guid);
void __cdecl S_Audio_Sample_CloseAllTracks(void);
bool __cdecl S_Audio_Sample_Load(
    int32_t sample_idx, LPWAVEFORMATEX format, const void *data,
    uint32_t data_size);
bool __cdecl S_Audio_Sample_IsTrackPlaying(int32_t track_id);
int32_t __cdecl S_Audio_Sample_Play(
    int32_t sample_idx, int32_t volume, int32_t pitch, int32_t pan,
    uint32_t flags);
int32_t __cdecl S_Audio_Sample_GetFreeTrackIndex(void);
void __cdecl S_Audio_Sample_AdjustTrackVolumeAndPan(
    int32_t track_id, int32_t volume, int32_t pan);
