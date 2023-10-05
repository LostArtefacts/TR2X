#include "specific/s_music_mm.h"

#include "global/funcs.h"
#include "global/vars.h"
#include "lib/winmm.h"
#include "log.h"

static bool __cdecl S_Music_MM_Init(void);
static void __cdecl S_Music_MM_Shutdown(void);
static void __cdecl S_Music_MM_Play(int32_t track_id, bool is_looped);
static void __cdecl S_Music_MM_Stop(void);
static bool __cdecl S_Music_MM_PlaySynced(int32_t track_id);
static uint32_t __cdecl S_Music_MM_GetFrames(void);
static void __cdecl S_Music_MM_SetVolume(int32_t volume);

static bool __cdecl S_Music_MM_Init(void)
{
    MCI_OPEN_PARMS open_params;
    open_params.lpstrDeviceType = "cdaudio";

    if (mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE, (DWORD_PTR)&open_params)) {
        return false;
    }

    g_MciDeviceID = open_params.wDeviceID;

    MCI_SET_PARMS set_params;
    set_params.dwTimeFormat = MCI_FORMAT_TMSF;
    mciSendCommand(
        g_MciDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)&set_params);

    return true;
}

static void __cdecl S_Music_MM_Shutdown(void)
{
    MCI_GENERIC_PARMS params;
    mciSendCommand(g_MciDeviceID, MCI_STOP, 0, (DWORD_PTR)&params);
    mciSendCommand(g_MciDeviceID, MCI_CLOSE, 0, (DWORD_PTR)&params);
}

static void __cdecl S_Music_MM_Play(int32_t track_id, bool is_looped)
{
    if (g_OptionMusicVolume == 0) {
        return;
    }

    g_CD_TrackID = track_id;

    track_id = Music_GetRealTrack(track_id);

    MCI_PLAY_PARMS play_params;
    play_params.dwFrom = track_id;
    play_params.dwTo = track_id + 1;
    mciSendCommand(
        g_MciDeviceID, MCI_PLAY, MCI_NOTIFY_FAILURE | MCI_NOTIFY_ABORTED,
        (DWORD_PTR)&play_params);

    if (is_looped) {
        g_CD_LoopTrack = track_id;
    }
}

static void __cdecl S_Music_MM_Stop(void)
{
    if (g_CD_TrackID <= 0) {
        return;
    }

    MCI_GENERIC_PARMS params;
    mciSendCommand(g_MciDeviceID, MCI_STOP, 0, (DWORD_PTR)&params);
    g_CD_TrackID = 0;
    g_CD_LoopTrack = 0;
}

static bool __cdecl S_Music_MM_PlaySynced(int32_t track_id)
{
    g_CD_TrackID = track_id;

    track_id = Music_GetRealTrack(track_id);

    MCI_SET_PARMS set_params;
    set_params.dwTimeFormat = MCI_FORMAT_TMSF;
    if (mciSendCommand(
            g_MciDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)&set_params)
        != 0) {
        return FALSE;
    }

    MCI_PLAY_PARMS play_params;
    play_params.dwFrom = track_id;
    play_params.dwTo = track_id + 1;
    return (
        mciSendCommand(
            g_MciDeviceID, MCI_PLAY, MCI_NOTIFY_FAILURE | MCI_NOTIFY_ABORTED,
            (DWORD_PTR)&play_params)
        == 0);
}

static uint32_t __cdecl S_Music_MM_GetFrames(void)
{
    MCI_STATUS_PARMS status_params;
    status_params.dwItem = MCI_STATUS_POSITION;
    if (mciSendCommand(
            g_MciDeviceID, MCI_STATUS, MCI_STATUS_ITEM,
            (DWORD_PTR)&status_params)
        != 0) {
        return 0;
    }

    uint32_t pos = status_params.dwReturn;
    int32_t min = MCI_TMSF_MINUTE(pos);
    int32_t sec = MCI_TMSF_SECOND(pos);
    int32_t frame = MCI_TMSF_FRAME(pos);
    return (min * 60 + sec) * 75 + frame;
}

static void __cdecl S_Music_MM_SetVolume(int32_t volume)
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

static struct S_MUSIC_BACKEND m_Backend = {
    .name = "MM",
    .Init = S_Music_MM_Init,
    .Shutdown = S_Music_MM_Shutdown,
    .Play = S_Music_MM_Play,
    .Stop = S_Music_MM_Stop,
    .PlaySynced = S_Music_MM_PlaySynced,
    .GetFrames = S_Music_MM_GetFrames,
    .SetVolume = S_Music_MM_SetVolume,
};

struct S_MUSIC_BACKEND *S_Music_MM_Factory(void)
{
    return &m_Backend;
}
