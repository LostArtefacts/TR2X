#include "specific/s_music.h"

#include "lib/winmm.h"

void __cdecl S_Music_SetVolume(int32_t volume)
{
    int32_t aux_dev_count = auxGetNumDevs();
    if (aux_dev_count == 0) {
        return;
    }

    volume *= 0x100;

    int32_t device_id = -1;
    bool is_volume_set = false;
    for (int32_t i = 0; i < aux_dev_count; ++i) {
        AUXCAPS caps;
        auxGetDevCaps(i, &caps, sizeof(AUXCAPS));

        switch (caps.wTechnology) {
        case AUXCAPS_CDAUDIO:
            auxSetVolume(i, MAKELONG(volume, volume));
            is_volume_set = true;
            break;

        case AUXCAPS_AUXIN:
            device_id = i;
            break;
        }
    }

    if (!is_volume_set && device_id != -1) {
        auxSetVolume(device_id, MAKELONG(volume, volume));
    }
}
