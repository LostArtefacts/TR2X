#include "specific/s_audio_sample.h"

#include "global/const.h"
#include "global/vars.h"

const struct SOUND_ADAPTER_NODE *__cdecl S_Audio_Sample_GetAdapter(GUID *guid)
{
    if (guid != NULL) {
        for (const struct SOUND_ADAPTER_NODE *adapter = g_SoundAdapterList.head;
             adapter; adapter = adapter->next) {
            if (!memcmp(&adapter->body.adapter_guid, guid, sizeof(GUID))) {
                return adapter;
            }
        }
    }

    return g_PrimarySoundAdapter;
}

void __cdecl S_Audio_Sample_CloseAllTracks(void)
{
    if (!g_IsSoundEnabled) {
        return;
    }

    for (int32_t i = 0; i < MAX_AUDIO_SAMPLE_BUFFERS; ++i) {
        if (g_SampleBuffers[i] != NULL) {
            IDirectSound_Release(g_SampleBuffers[i]);
            g_SampleBuffers[i] = NULL;
        }
    }
}
