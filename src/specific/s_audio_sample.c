#include "global/vars.h"
#include "specific/s_audio_sample.h"

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
