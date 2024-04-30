#include "game/music.h"

#include "global/const.h"
#include "global/funcs.h"
#include "global/types.h"
#include "global/vars.h"

#include <libtrx/engine/audio.h>
#include <libtrx/filesystem.h>
#include <libtrx/log.h>

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

#define MAX_CD_TRACKS 60

static MUSIC_TRACK_ID m_TrackCurrent = MX_INACTIVE;
static MUSIC_TRACK_ID m_TrackLooped = MX_INACTIVE;

typedef struct {
    uint64_t from;
    uint64_t to;
    bool active;
} CDAUDIO_TRACK;

typedef struct {
    char *path;
    char *audio_type;
} CDAUDIO_SPEC;

static const int32_t m_CDAudioSpecCount = 2;
static CDAUDIO_SPEC m_CDAudioSpecs[] = {
    {
        .path = "audio/cdaudio.wav",
        .audio_type = "waveaudio",
    },
    {
        .path = "audio/cdaudio.mp3",
        .audio_type = "mpegvideo",
    },
};

static const CDAUDIO_SPEC *m_ChosenSpec = NULL;
static CDAUDIO_TRACK m_Tracks[MAX_CD_TRACKS];

static bool m_Initialized = false;
static float m_MusicVolume = 0.0f;
static int m_AudioStreamID = -1;

static const CDAUDIO_SPEC *Music_FindSpec(void);
static bool Music_ParseCDAudio(void);
static void Music_StreamFinished(int stream_id, void *user_data);

static void Music_StreamFinished(const int stream_id, void *const user_data)
{
    // When a stream finishes, play the remembered background BGM.
    if (stream_id == m_AudioStreamID) {
        m_AudioStreamID = -1;
        if (m_TrackLooped >= 0) {
            Music_Play(m_TrackLooped, true);
        }
    }
}

static const CDAUDIO_SPEC *Music_FindSpec(void)
{
    const CDAUDIO_SPEC *spec = NULL;

    for (int32_t i = 0; i < m_CDAudioSpecCount; i++) {
        MYFILE *const fp = File_Open(m_CDAudioSpecs[i].path, FILE_OPEN_READ);
        if (fp != NULL) {
            spec = &m_CDAudioSpecs[i];
            File_Close(fp);
            break;
        }
    }

    if (spec == NULL) {
        LOG_WARNING("Cannot find any CDAudio data files");
        return NULL;
    }
    return spec;
}

static bool Music_ParseCDAudio(void)
{
    char *track_content = NULL;
    size_t track_content_size;
    if (!File_Load("audio/cdaudio.dat", &track_content, &track_content_size)) {
        LOG_WARNING("Cannot find CDAudio control file");
        return false;
    }

    memset(m_Tracks, 0, sizeof(m_Tracks));

    size_t offset = 0;
    while (offset < track_content_size) {
        while (track_content[offset] == '\n' || track_content[offset] == '\r') {
            if (++offset >= track_content_size) {
                goto parse_end;
            }
        }

        uint64_t track_num;
        uint64_t from;
        uint64_t to;
        int32_t result = sscanf(
            &track_content[offset], "%" PRIu64 " %" PRIu64 " %" PRIu64,
            &track_num, &from, &to);

        if (result == 3 && track_num > 0 && track_num <= MAX_CD_TRACKS) {
            int32_t track_idx = track_num - 1;
            m_Tracks[track_idx].active = true;
            m_Tracks[track_idx].from = from;
            m_Tracks[track_idx].to = to;
        }

        while (track_content[offset] != '\n' && track_content[offset] != '\r') {
            if (++offset >= track_content_size) {
                goto parse_end;
            }
        }
    }

parse_end:
    free(track_content);

    // reindex wrong track boundaries
    for (int32_t i = 0; i < MAX_CD_TRACKS; i++) {
        if (!m_Tracks[i].active) {
            continue;
        }

        if (i < MAX_CD_TRACKS - 1 && m_Tracks[i].from >= m_Tracks[i].to) {
            for (int32_t j = i + 1; j < MAX_CD_TRACKS; j++) {
                if (m_Tracks[j].active) {
                    m_Tracks[i].to = m_Tracks[j].from;
                    break;
                }
            }
        }

        if (m_Tracks[i].from >= m_Tracks[i].to && i > 0) {
            for (int32_t j = i - 1; j >= 0; j--) {
                if (m_Tracks[j].active) {
                    m_Tracks[i].from = m_Tracks[j].to;
                    break;
                }
            }
        }
    }

    return true;
}

bool __cdecl Music_Init(void)
{
    // TODO: remove this guard once Music_Init can be called in a proper place
    if (m_Initialized) {
        return true;
    }

    if (!Audio_Init()) {
        LOG_ERROR("Failed to initialize libtrx sound system");
        return false;
    }

    m_ChosenSpec = Music_FindSpec();
    if (m_ChosenSpec == NULL) {
        LOG_ERROR("Failed to find CDAudio data");
        return false;
    }

    if (!Music_ParseCDAudio()) {
        LOG_ERROR("Failed to parse CDAudio data");
        return false;
    }

    m_Initialized = true;
    m_TrackCurrent = MX_INACTIVE;
    m_TrackLooped = MX_INACTIVE;
    Music_SetVolume(25 * g_OptionMusicVolume + 5);
    return true;
}

void __cdecl Music_Shutdown(void)
{
    if (m_AudioStreamID < 0) {
        return;
    }

    // We are only interested in calling Music_StreamFinished if a stream
    // finished by itself. In cases where we end the streams early by hand,
    // we clear the finish callback in order to avoid resuming the BGM playback
    // just after we stop it.
    Audio_Stream_SetFinishCallback(m_AudioStreamID, NULL, NULL);
    Audio_Stream_Close(m_AudioStreamID);
}

void __cdecl Music_Play(int16_t track_id, bool is_looped)
{
    if (track_id == m_TrackCurrent) {
        return;
    }

    // TODO: this should be called in shell instead, once per game launch
    Music_Init();

    Audio_Stream_Close(m_AudioStreamID);
    if (g_OptionMusicVolume == 0) {
        LOG_DEBUG("Not playing track %d because the game is silent", track_id);
        return;
    }

    const int32_t track_idx = Music_GetRealTrack(track_id) - 1;
    if (track_idx < 0 || track_idx >= MAX_CD_TRACKS) {
        LOG_ERROR("Invalid track: %d", track_id);
        return;
    }

    const CDAUDIO_TRACK *track = &m_Tracks[track_idx];
    if (!track->active) {
        LOG_ERROR("Invalid track: %d", track_id);
        return;
    }

    LOG_DEBUG(
        "Playing track %d (real: %d), looped: %d", track_id, track_idx + 1,
        is_looped);

    m_AudioStreamID = Audio_Stream_CreateFromFile(m_ChosenSpec->path);
    if (m_AudioStreamID < 0) {
        LOG_ERROR("Failed to create music stream for track %d", track_id);
        return;
    }

    g_CD_TrackID = track_id;
    m_TrackCurrent = track_id;
    if (is_looped) {
        m_TrackLooped = track_id;
    }

    Audio_Stream_SetVolume(m_AudioStreamID, m_MusicVolume);
    Audio_Stream_SetFinishCallback(m_AudioStreamID, Music_StreamFinished, NULL);
    Audio_Stream_SetStartTimestamp(m_AudioStreamID, track->from / 1000.0);
    Audio_Stream_SetStopTimestamp(m_AudioStreamID, track->to / 1000.0);
    Audio_Stream_SeekTimestamp(m_AudioStreamID, track->from / 1000.0);
    Audio_Stream_SetIsLooped(m_AudioStreamID, is_looped);
}

void __cdecl Music_Stop(void)
{
    if (m_AudioStreamID < 0) {
        return;
    }
    m_TrackCurrent = MX_INACTIVE;
    m_TrackLooped = MX_INACTIVE;
    Audio_Stream_Close(m_AudioStreamID);
}

bool __cdecl Music_PlaySynced(int16_t track_id)
{
    Music_Play(track_id, false);
    return true;
}

uint32_t __cdecl Music_GetFrames(void)
{
    if (m_AudioStreamID < 0) {
        return 0;
    }
    return Audio_Stream_GetTimestamp(m_AudioStreamID) * FRAMES_PER_SECOND
        * TICKS_PER_FRAME / 1000.0;
}

void __cdecl Music_SetVolume(int32_t volume)
{
    m_MusicVolume = volume ? volume / 255.0f : 0.0f;
    if (m_AudioStreamID >= 0) {
        Audio_Stream_SetVolume(m_AudioStreamID, m_MusicVolume);
    }
}
