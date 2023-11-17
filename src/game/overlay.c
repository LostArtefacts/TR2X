#include "game/overlay.h"

#define FLASH_FRAMES 5

static bool m_FlashState = false;
static int32_t m_FlashCounter = 0;

bool __cdecl Overlay_FlashCounter(void)
{
    if (m_FlashCounter > 0) {
        m_FlashCounter--;
        return m_FlashState;
    } else {
        m_FlashCounter = FLASH_FRAMES;
        m_FlashState = !m_FlashState;
    }
    return m_FlashState;
}
