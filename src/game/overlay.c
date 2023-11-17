#include "game/overlay.h"

#include "game/output.h"
#include "global/const.h"
#include "global/vars.h"

#include <stdio.h>

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

void __cdecl Overlay_DrawAssaultTimer(void)
{
    if (g_CurrentLevel != 0 || !g_IsAssaultTimerDisplay) {
        return;
    }

    static char buffer[8];
    const int32_t total_sec = g_SaveGame.statistics.timer / FRAMES_PER_SECOND;
    const int32_t frame = g_SaveGame.statistics.timer % FRAMES_PER_SECOND;
    sprintf(
        buffer, "%d:%02d.%d", total_sec / 60, total_sec % 60,
        frame * 10 / FRAMES_PER_SECOND);

    const int32_t scale_h = PHD_ONE;
    const int32_t scale_v = PHD_ONE;

    const int32_t dot_offset = -6;
    const int32_t dot_width = 14;
    const int32_t colon_offset = -6;
    const int32_t colon_width = 14;
    const int32_t digit_offset = 0;
    const int32_t digit_width = 20;

    const int32_t y = 36;
    int32_t x = (g_PhdWinMaxX / 2) - 50;

    for (char *c = buffer; *c != '\0'; c++) {
        if (*c == ':') {
            x += colon_offset;
            Output_DrawScreenSprite2D(
                x, y, 0, scale_h, scale_v,
                g_Objects[O_ASSAULT_DIGITS].mesh_idx + 10, 0x1000, 0);
            x += colon_width;
        } else if (*c == '.') {
            x += dot_offset;
            Output_DrawScreenSprite2D(
                x, y, 0, scale_h, scale_v,
                g_Objects[O_ASSAULT_DIGITS].mesh_idx + 11, 0x1000, 0);
            x += dot_width;
        } else {
            x += digit_offset;
            Output_DrawScreenSprite2D(
                x, y, 0, scale_h, scale_v,
                *c + g_Objects[O_ASSAULT_DIGITS].mesh_idx - '0', 0x1000, 0);
            x += digit_width;
        }
    }
}
