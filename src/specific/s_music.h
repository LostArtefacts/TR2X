#pragma once

#include "global/types.h"

typedef struct S_MUSIC_BACKEND {
    const char *name;
    bool (*Init)(void);
    void (*Shutdown)(void);
    void (*Play)(int32_t track_id, bool is_looped);
    void (*Stop)(void);
    bool (*PlaySynced)(int32_t track_id);
    uint32_t (*GetFrames)(void);
    void (*SetVolume)(int32_t volume);
} S_MUSIC_BACKEND;
