#include "specific/s_music.h"

#include "global/vars.h"
#include "lib/winmm.h"

uint32_t __cdecl S_Music_GetFrames(void)
{
    MCI_STATUS_PARMS status_params;
    status_params.dwItem = MCI_STATUS_POSITION;
    if (mciSendCommand(
            g_MciDeviceID, MCI_STATUS, MCI_STATUS_ITEM,
            (DWORD_PTR)&status_params)) {
        return 0;
    }

    uint32_t pos = status_params.dwReturn;
    int32_t min = MCI_TMSF_MINUTE(pos);
    int32_t sec = MCI_TMSF_SECOND(pos);
    int32_t frame = MCI_TMSF_FRAME(pos);
    return (min * 60 + sec) * 75 + frame;
}

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
