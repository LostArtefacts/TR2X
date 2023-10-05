#pragma once

#include "global/types.h"

const struct SOUND_ADAPTER_NODE *__cdecl S_Audio_Sample_GetAdapter(GUID *guid);
void __cdecl S_Audio_Sample_CloseAllTracks(void);
bool __cdecl S_Audio_Sample_Load(
    int32_t sample_idx, LPWAVEFORMATEX format, const void *data,
    uint32_t data_size);
