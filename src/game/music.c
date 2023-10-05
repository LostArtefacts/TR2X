#include "game/music.h"

#include "log.h"
#include "specific/s_music.h"
#include "specific/s_music_mm.h"
#include "specific/s_music_pauld.h"

struct S_MUSIC_BACKEND *m_CurrentBackend = NULL;
struct S_MUSIC_BACKEND **m_Backends = NULL;
struct S_MUSIC_BACKEND *(*m_BackendFactories[])(void) = {
    S_Music_PaulD_Factory,
    S_Music_MM_Factory,
    NULL,
};

bool __cdecl Music_Init(void)
{
    if (m_CurrentBackend) {
        return true;
    }

    int32_t num_backends = 0;
    for (int32_t i = 0;; i++) {
        struct S_MUSIC_BACKEND *(*factory)(void) = m_BackendFactories[i];
        if (!factory) {
            break;
        }
        num_backends++;
    }

    m_Backends = malloc(sizeof(S_MUSIC_BACKEND *) * num_backends);

    for (int32_t i = 0; i < num_backends; i++) {
        struct S_MUSIC_BACKEND *(*factory)(void) = m_BackendFactories[i];
        m_Backends[i] = factory();
    }

    for (int32_t i = 0; i < num_backends; i++) {
        struct S_MUSIC_BACKEND *backend = m_Backends[i];
        LOG_DEBUG("querying backend: %s", backend->name);
        if (m_Backends[i]->Init()) {
            LOG_DEBUG("chosen backend: %s", backend->name);
            m_CurrentBackend = backend;
            return true;
        }
    }

    return false;
}

void __cdecl Music_Shutdown(void)
{
    if (!m_CurrentBackend) {
        return;
    }
    m_CurrentBackend->Shutdown();
}

void __cdecl Music_Play(int16_t track_id, bool is_looped)
{
    Music_Init(); // TODO: this shouldn't be called here
    if (!m_CurrentBackend) {
        return;
    }
    m_CurrentBackend->Play(track_id, is_looped);
}

void __cdecl Music_Stop(void)
{
    if (!m_CurrentBackend) {
        return;
    }
    m_CurrentBackend->Stop();
}

bool __cdecl Music_PlaySynced(int16_t track_id)
{
    if (!m_CurrentBackend) {
        return false;
    }
    return m_CurrentBackend->PlaySynced(track_id);
}

uint32_t __cdecl Music_GetFrames(void)
{
    if (!m_CurrentBackend) {
        return 0;
    }
    return m_CurrentBackend->GetFrames();
}

void __cdecl Music_SetVolume(int32_t volume)
{
    if (!m_CurrentBackend) {
        return;
    }
    return m_CurrentBackend->SetVolume(volume);
}
