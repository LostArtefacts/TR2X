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
