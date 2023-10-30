#include "specific/s_music_pauld.h"

#include "filesystem.h"
#include "global/funcs.h"
#include "global/types.h"
#include "global/vars.h"
#include "lib/winmm.h"
#include "log.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

#define CD_ALIAS "TR2X"
#define MAX_CD_TRACKS 60

typedef struct CDAUDIO_TRACK_INFO {
    uint64_t from;
    uint64_t to;
    bool active;
} CDAUDIO_TRACK_INFO;

typedef struct CDAUDIO_SPEC {
    char *path;
    char *audio_type;
} CDAUDIO_SPEC;

const int32_t m_CDAudioSpecCount = 2;
static struct CDAUDIO_SPEC m_CDAudioSpecs[] = {
    {
        // NOTE: volume control doesn't seem to work with WAV
        .path = "audio/cdaudio.wav",
        .audio_type = "waveaudio",
    },
    {
        .path = "audio/cdaudio.mp3",
        .audio_type = "mpegvideo",
    },
};

static CDAUDIO_TRACK_INFO m_Tracks[MAX_CD_TRACKS];

static bool S_Music_PaulD_Command(
    LPSTR param, UINT param2, const char *cmd_fmt, ...);
static const struct CDAUDIO_SPEC *S_Music_PaulD_FindSpec(void);
static bool S_Music_PaulD_ParseCDAudio(void);

static bool S_Music_PaulD_Init(void);
static void S_Music_PaulD_Shutdown(void);
static void S_Music_PaulD_Play(int32_t track_id, bool is_looped);
static void S_Music_PaulD_Stop(void);
static bool S_Music_PaulD_PlaySynced(int32_t track_id);
static uint32_t S_Music_PaulD_GetFrames(void);
static void S_Music_PaulD_SetVolume(int32_t volume);

static bool S_Music_PaulD_Command(
    LPSTR param, UINT param2, const char *cmd_fmt, ...)
{
    const size_t size = 256;
    char cmd_string[size];
    va_list args;
    va_start(args, cmd_fmt);
    vsnprintf(cmd_string, size, cmd_fmt, args);
    va_end(args);

    MCIERROR result = mciSendStringA(cmd_string, param, param2, 0);
    LOG_DEBUG("sending command: %s (result: 0x%X)", cmd_string, result);
    return result == 0;
}

static const struct CDAUDIO_SPEC *S_Music_PaulD_FindSpec(void)
{
    const struct CDAUDIO_SPEC *spec = NULL;

    for (int32_t i = 0; i < m_CDAudioSpecCount; i++) {
        MYFILE *fp = File_Open(m_CDAudioSpecs[i].path, FILE_OPEN_READ);
        if (fp) {
            spec = &m_CDAudioSpecs[i];
            File_Close(fp);
            break;
        }
    }

    if (!spec) {
        LOG_WARNING("Cannot find any CDAudio data files");
        return NULL;
    }
    return spec;
}

static bool S_Music_PaulD_ParseCDAudio(void)
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

static bool S_Music_PaulD_Init(void)
{
    const struct CDAUDIO_SPEC *spec = S_Music_PaulD_FindSpec();
    if (!spec) {
        return false;
    }

    if (!S_Music_PaulD_ParseCDAudio()) {
        return false;
    }

    if (!S_Music_PaulD_Command(
            NULL, 0, "open %s type %s alias " CD_ALIAS, spec->path,
            spec->audio_type)) {
        return false;
    }

    S_Music_PaulD_Command(NULL, 0, "set " CD_ALIAS " time format ms");
    return true;
}

static void S_Music_PaulD_Shutdown(void)
{
    S_Music_PaulD_Stop();
    S_Music_PaulD_Command(NULL, 0, "close " CD_ALIAS);
}

static void S_Music_PaulD_Play(int32_t track_id, bool is_looped)
{
    if (g_OptionMusicVolume == 0) {
        return;
    }

    int32_t real_track_id = Music_GetRealTrack(track_id);
    if (real_track_id < 1 || !m_Tracks[real_track_id - 1].active) {
        return;
    }

    g_CD_TrackID = track_id;

    S_Music_PaulD_Command(
        NULL, 0, "play " CD_ALIAS " from %" PRIu64 " to %" PRIu64,
        m_Tracks[real_track_id - 1].from, m_Tracks[real_track_id - 1].to);

    if (is_looped) {
        g_CD_LoopTrack = real_track_id;
    }
}

static void S_Music_PaulD_Stop(void)
{
    if (g_CD_TrackID <= 0) {
        return;
    }

    S_Music_PaulD_Command(NULL, 0, "stop " CD_ALIAS);
    g_CD_TrackID = 0;
    g_CD_LoopTrack = 0;
}

static bool S_Music_PaulD_PlaySynced(int32_t track_id)
{
    int32_t real_track_id = Music_GetRealTrack(track_id);
    if (real_track_id < 1 || !m_Tracks[real_track_id - 1].active) {
        return false;
    }

    g_CD_TrackID = track_id;

    if (!S_Music_PaulD_Command(NULL, 0, "set " CD_ALIAS " time format ms")) {
        return false;
    }

    return S_Music_PaulD_Command(
        NULL, 0, "play " CD_ALIAS " from %" PRIu64 " to %" PRIu64,
        m_Tracks[real_track_id - 1].from, m_Tracks[real_track_id - 1].to);
}

static uint32_t S_Music_PaulD_GetFrames(void)
{
    char status_string[32];
    if (!S_Music_PaulD_Command(
            status_string, sizeof(status_string),
            "status " CD_ALIAS " position")) {
        return 0;
    }

    int32_t real_track_id = Music_GetRealTrack(g_CD_TrackID);
    return (atol(status_string) - m_Tracks[real_track_id - 1].from) * 75 / 1000;
}

static void S_Music_PaulD_SetVolume(int32_t volume)
{
    if (volume > 0) {
        volume = (volume - 5) * 4;
    }

    S_Music_PaulD_Command(
        NULL, 0, "setaudio " CD_ALIAS " volume to %lu", volume);

    char status_string[32];
    if (!S_Music_PaulD_Command(
            status_string, sizeof(status_string), "status " CD_ALIAS " mode")) {
        return;
    }

    if (volume > 0) {
        if (!strncmp(status_string, "paused", sizeof(status_string))) {
            S_Music_PaulD_Command(NULL, 0, "resume " CD_ALIAS);
        }
    } else {
        if (!strncmp(status_string, "playing", sizeof(status_string))) {
            S_Music_PaulD_Command(NULL, 0, "pause " CD_ALIAS);
        }
    }
}

static struct S_MUSIC_BACKEND m_Backend = {
    .name = "PaulD",
    .Init = S_Music_PaulD_Init,
    .Shutdown = S_Music_PaulD_Shutdown,
    .Play = S_Music_PaulD_Play,
    .Stop = S_Music_PaulD_Stop,
    .PlaySynced = S_Music_PaulD_PlaySynced,
    .GetFrames = S_Music_PaulD_GetFrames,
    .SetVolume = S_Music_PaulD_SetVolume,
};

struct S_MUSIC_BACKEND *S_Music_PaulD_Factory(void)
{
    return &m_Backend;
}
