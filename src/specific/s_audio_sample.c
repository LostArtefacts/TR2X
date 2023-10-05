#include "specific/s_audio_sample.h"

#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "lib/dsound.h"
#include "log.h"
#include "specific/s_flagged_string.h"

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

bool __cdecl S_Audio_Sample_Load(
    int32_t sample_idx, LPWAVEFORMATEX format, const void *data,
    uint32_t data_size)
{
    if (!g_DSound || !g_IsSoundEnabled
        || sample_idx >= MAX_AUDIO_SAMPLE_BUFFERS) {
        return false;
    }

    if (g_SampleBuffers[sample_idx] != NULL) {
        IDirectSound_Release(g_SampleBuffers[sample_idx]);
        g_SampleBuffers[sample_idx] = NULL;
    }

    DSBUFFERDESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY
        | DSBCAPS_LOCSOFTWARE;
    desc.dwBufferBytes = data_size;
    desc.dwReserved = 0;
    desc.lpwfxFormat = format;

    if (IDirectSound_CreateSoundBuffer(
            g_DSound, &desc, &g_SampleBuffers[sample_idx], NULL)
        < 0) {
        return false;
    }

    LPVOID audio_ptr;
    DWORD audio_bytes;
    if (IDirectSoundBuffer_Lock(
            g_SampleBuffers[sample_idx], 0, data_size, &audio_ptr, &audio_bytes,
            NULL, NULL, 0)
        < 0) {
        return false;
    }

    memcpy(audio_ptr, data, audio_bytes);

    if (IDirectSoundBuffer_Unlock(
            g_SampleBuffers[sample_idx], audio_ptr, audio_bytes, NULL, 0)
        < 0) {
        return false;
    }

    g_SampleFreqs[sample_idx] = format->nSamplesPerSec;
    return true;
}

bool __cdecl S_Audio_Sample_IsTrackPlaying(int32_t track_id)
{
    if (track_id < 0 || !g_ChannelBuffers[track_id]) {
        return false;
    }

    DWORD status;
    if (IDirectSoundBuffer_GetStatus(g_ChannelBuffers[track_id], &status) < 0) {
        return false;
    }

    if (!(status & DSBSTATUS_PLAYING)) {
        IDirectSoundBuffer_Release(g_ChannelBuffers[track_id]);
        g_ChannelBuffers[track_id] = NULL;
        return false;
    }

    return true;
}

int32_t __cdecl S_Audio_Sample_Play(
    int32_t sample_idx, int32_t volume, int32_t pitch, int32_t pan,
    uint32_t flags)
{
    int32_t track_id = S_Audio_Sample_GetFreeTrackIndex();
    if (track_id < 0) {
        return -1;
    }

    LPDIRECTSOUNDBUFFER buffer = NULL;
    if ((IDirectSound_DuplicateSoundBuffer(
             g_DSound, g_SampleBuffers[sample_idx], &buffer)
         < 0)
        || (IDirectSoundBuffer_SetVolume(buffer, volume) < 0)
        || (IDirectSoundBuffer_SetFrequency(
                buffer, g_SampleFreqs[sample_idx] * pitch / PHD_ONE)
            < 0)
        || (IDirectSoundBuffer_SetPan(buffer, pan) < 0)
        || (IDirectSoundBuffer_SetCurrentPosition(buffer, 0) < 0)
        || (IDirectSoundBuffer_Play(buffer, 0, 0, flags) < 0)) {
        return -2;
    }

    g_ChannelSamples[track_id] = sample_idx;
    g_ChannelBuffers[track_id] = buffer;

    return track_id;
}

int32_t __cdecl S_Audio_Sample_GetFreeTrackIndex(void)
{
    for (int32_t i = 0; i < MAX_AUDIO_SAMPLE_TRACKS; ++i) {
        if (!g_ChannelBuffers[i]) {
            return i;
        }
    }

    for (int32_t i = 0; i < MAX_AUDIO_SAMPLE_TRACKS; ++i) {
        if (!S_Audio_Sample_IsTrackPlaying(i)) {
            return i;
        }
    }

    return -1;
}

void __cdecl S_Audio_Sample_AdjustTrackVolumeAndPan(
    int32_t track_id, int32_t volume, int32_t pan)
{
    if (track_id < 0 || !g_ChannelBuffers[track_id]) {
        return;
    }

    IDirectSoundBuffer_SetVolume(g_ChannelBuffers[track_id], volume);
    IDirectSoundBuffer_SetPan(g_ChannelBuffers[track_id], pan);
}

void __cdecl S_Audio_Sample_AdjustTrackPitch(int32_t track_id, int32_t pitch)
{
    if (track_id < 0 || !g_ChannelBuffers[track_id]) {
        return;
    }

    IDirectSoundBuffer_SetFrequency(
        g_ChannelBuffers[track_id],
        g_SampleFreqs[g_ChannelSamples[track_id]] * pitch / PHD_ONE);
}

void __cdecl S_Audio_Sample_CloseTrack(int32_t track_id)
{
    if (track_id < 0 || !g_ChannelBuffers[track_id]) {
        return;
    }

    IDirectSoundBuffer_Stop(g_ChannelBuffers[track_id]);
    IDirectSoundBuffer_Release(g_ChannelBuffers[track_id]);
    g_ChannelBuffers[track_id] = NULL;
}

bool __cdecl S_Audio_Sample_Init(void)
{
    struct SOUND_ADAPTER_NODE *node = g_SoundAdapterList.head;
    while (node) {
        struct SOUND_ADAPTER_NODE *next_node = node->next;
        S_FlaggedString_Delete(&node->body.module);
        S_FlaggedString_Delete(&node->body.description);
        free(node);
        node = next_node;
    }

    g_SoundAdapterList.head = NULL;
    g_SoundAdapterList.tail = NULL;
    g_SoundAdapterList.count = 0;
    g_PrimarySoundAdapter = NULL;

    if (!S_Audio_Sample_DSoundEnumerate(&g_SoundAdapterList)) {
        LOG_ERROR("Failed to enumerate sound devices");
        return false;
    }

    for (node = g_SoundAdapterList.head; node; node = node->next) {
        if (!node->body.adapter_guid_ptr) {
            g_PrimarySoundAdapter = node;
            break;
        }
    }

    return true;
}

bool __cdecl S_Audio_Sample_DSoundEnumerate(
    struct SOUND_ADAPTER_LIST *adapter_list)
{
    return DirectSoundEnumerateA(
               S_Audio_Sample_DSoundEnumCallback, (LPVOID)adapter_list)
        >= 0;
}

BOOL CALLBACK S_Audio_Sample_DSoundEnumCallback(
    LPGUID guid, LPCTSTR description, LPCTSTR module, LPVOID context)
{
    struct SOUND_ADAPTER_LIST *adapter_list =
        (struct SOUND_ADAPTER_LIST *)context;
    struct SOUND_ADAPTER_NODE *adapter_node =
        malloc(sizeof(struct SOUND_ADAPTER_NODE));

    if (!adapter_node) {
        return TRUE;
    }

    adapter_node->next = NULL;
    adapter_node->previous = adapter_list->tail;

    if (!adapter_list->head) {
        adapter_list->head = adapter_node;
    }

    if (adapter_list->tail) {
        adapter_list->tail->next = adapter_node;
    }

    adapter_list->tail = adapter_node;
    adapter_list->count++;

    if (guid) {
        adapter_node->body.adapter_guid = *guid;
        adapter_node->body.adapter_guid_ptr = &adapter_node->body.adapter_guid;
    } else {
        memset(&adapter_node->body.adapter_guid, 0, sizeof(GUID));
        adapter_node->body.adapter_guid_ptr = NULL;
    }

    S_FlaggedString_Create(&adapter_node->body.description, 256);
    S_FlaggedString_Create(&adapter_node->body.module, 256);
    strcpy(adapter_node->body.description.content, description);
    strcpy(adapter_node->body.module.content, module);

    return TRUE;
}

void __cdecl S_Audio_Sample_Init2(HWND hwnd)
{
    memset(g_SampleBuffers, 0, sizeof(g_SampleBuffers));
    memset(g_ChannelBuffers, 0, sizeof(g_ChannelBuffers));

    g_Camera.is_lara_mic = g_SavedAppSettings.LaraMic;
    g_IsSoundEnabled = false;

    if (!g_SavedAppSettings.SoundEnabled
        || !g_SavedAppSettings.PreferredSoundAdapter) {
        return;
    }

    struct SOUND_ADAPTER *preferred =
        &g_SavedAppSettings.PreferredSoundAdapter->body;
    g_CurrentSoundAdapter = *preferred;

    S_FlaggedString_Copy(
        &g_CurrentSoundAdapter.description, &preferred->description);
    S_FlaggedString_Copy(&g_CurrentSoundAdapter.module, &preferred->module);

    if (!S_Audio_Sample_DSoundCreate(g_CurrentSoundAdapter.adapter_guid_ptr)) {
        return;
    }

    if (!hwnd) {
        hwnd = g_GameWindowHandle;
    }

    if (IDirectSound_SetCooperativeLevel(g_DSound, hwnd, DSSCL_EXCLUSIVE) < 0) {
        LOG_ERROR("Error: Can't set DSound cooperative level");
        return;
    }

    g_IsSoundEnabled = S_Audio_Sample_DSoundBufferTest();
}

bool __cdecl S_Audio_Sample_DSoundCreate(GUID *guid)
{
    return DirectSoundCreate(guid, &g_DSound, NULL) >= 0;
}

bool __cdecl S_Audio_Sample_DSoundBufferTest(void)
{
    DSBUFFERDESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_PRIMARYBUFFER;
    desc.dwBufferBytes = 0;
    desc.dwReserved = 0;
    desc.lpwfxFormat = NULL;

    LPDIRECTSOUNDBUFFER dsBuffer;
    if (IDirectSound_CreateSoundBuffer(g_DSound, &desc, &dsBuffer, NULL) < 0) {
        return false;
    }

    WAVEFORMATEX format;
    memset(&format, 0, sizeof(format));
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 2;
    format.nSamplesPerSec = 11025;
    format.nAvgBytesPerSec = 44100;
    format.nBlockAlign = 4;
    format.wBitsPerSample = 16;
    format.cbSize = 0;

    bool result = IDirectSoundBuffer_SetFormat(dsBuffer, &format) >= 0;
    IDirectSoundBuffer_Release(dsBuffer);
    return result;
}

void __cdecl S_Audio_Sample_Shutdown(void)
{
    S_Audio_Sample_CloseAllTracks();
    if (g_DSound != NULL) {
        IDirectSound_Release(g_DSound);
        g_DSound = NULL;
    }
}

bool __cdecl S_Audio_Sample_IsEnabled(void)
{
    return g_IsSoundEnabled;
}

int32_t __cdecl S_Audio_Sample_OutPlay(
    int32_t sample_id, int32_t volume, int32_t pitch, int32_t pan)
{
    if (!g_SoundIsActive) {
        return -3;
    }

    int32_t calc_pan = S_Audio_Sample_CalculateSamplePan(pan);
    int32_t calc_volume = S_Audio_Sample_CalculateSampleVolume(volume);
    return S_Audio_Sample_Play(sample_id, calc_volume, pitch, calc_pan, 0);
}

int32_t __cdecl S_Audio_Sample_CalculateSampleVolume(int32_t volume)
{
    double adjusted_volume = g_MasterVolume * volume;
    double scaler = 0x1.p-21; // 2.0e-21
    return (adjusted_volume * scaler - 1.0) * 5000.0;
}

int32_t __cdecl S_Audio_Sample_CalculateSamplePan(int16_t pan)
{
    return pan / 16;
}

int32_t __cdecl S_Audio_Sample_OutPlayLooped(
    int32_t sample_id, int32_t volume, int32_t pitch, int32_t pan)
{
    if (!g_SoundIsActive) {
        return -3;
    }

    int32_t calc_pan = S_Audio_Sample_CalculateSamplePan(pan);
    int32_t calc_volume = S_Audio_Sample_CalculateSampleVolume(volume);
    return S_Audio_Sample_Play(
        sample_id, calc_volume, pitch, calc_pan, DSBPLAY_LOOPING);
}

void __cdecl S_Audio_Sample_OutSetPanAndVolume(
    int32_t track_id, int32_t pan, int32_t volume)
{
    if (!g_SoundIsActive) {
        return;
    }
    int32_t calc_pan = S_Audio_Sample_CalculateSamplePan(pan);
    int32_t calc_volume = S_Audio_Sample_CalculateSampleVolume(volume);
    S_Audio_Sample_AdjustTrackVolumeAndPan(track_id, calc_volume, calc_pan);
}

void __cdecl S_Audio_Sample_OutSetPitch(int32_t track_id, int32_t pitch)
{
    if (!g_SoundIsActive) {
        return;
    }
    S_Audio_Sample_AdjustTrackPitch(track_id, pitch);
}

void __cdecl S_Audio_Sample_OutCloseTrack(int32_t track_id)
{
    if (!g_SoundIsActive) {
        return;
    }
    S_Audio_Sample_CloseTrack(track_id);
}

void __cdecl S_Audio_Sample_OutCloseAllTracks(void)
{
    if (!g_SoundIsActive) {
        return;
    }
    for (int32_t i = 0; i < MAX_AUDIO_SAMPLE_TRACKS; ++i) {
        S_Audio_Sample_CloseTrack(i);
    }
}

bool __cdecl S_Audio_Sample_OutIsTrackPlaying(int32_t track_id)
{
    if (!g_SoundIsActive) {
        return FALSE;
    }
    return S_Audio_Sample_IsTrackPlaying(track_id);
}
