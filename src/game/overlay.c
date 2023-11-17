#include "game/overlay.h"

#include "game/output.h"
#include "game/text.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

#include <stdio.h>

#define FLASH_FRAMES 5
#define AMMO_X (-10)
#define AMMO_Y 35

static int32_t m_OldGameTimer = 0;
static int32_t m_OldHitPoints = -1;
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

void __cdecl Overlay_DrawGameInfo(const bool pickup_state)
{
    Overlay_DrawAmmoInfo();
    Overlay_DrawModeInfo();
    if (g_OverlayStatus > 0) {
        bool flash = Overlay_FlashCounter();
        Overlay_DrawHealthBar(flash);
        Overlay_DrawAirBar(flash);
        Overlay_DrawPickups(pickup_state);
        Overlay_DrawAssaultTimer();
    }
    Text_Draw();
}

void __cdecl Overlay_DrawHealthBar(const bool flash_state)
{
    int32_t hit_points = g_LaraItem->hit_points;
    CLAMP(hit_points, 0, LARA_MAX_HITPOINTS);

    if (m_OldHitPoints != hit_points) {
        m_OldHitPoints = hit_points;
        g_HealthBarTimer = 40;
    }

    int32_t timer = g_HealthBarTimer;
    if (timer < 0) {
        timer = 0;
        g_HealthBarTimer = 0;
    }

    if (hit_points <= LARA_MAX_HITPOINTS / 4 && !flash_state) {
        S_DrawHealthBar(0);
        return;
    }

    if (timer <= 0 && g_Lara.gun_status != LGS_READY) {
        return;
    }
    S_DrawHealthBar(hit_points * 100 / LARA_MAX_HITPOINTS);
}

void __cdecl Overlay_DrawAirBar(const bool flash_state)
{
    if (g_Lara.water_status != LWS_UNDERWATER
        && g_Lara.water_status != LWS_SURFACE) {
        return;
    }

    int32_t air = g_Lara.air;
    CLAMP(air, 0, LARA_MAX_AIR);
    if (air <= 450 && !flash_state) {
        S_DrawAirBar(0);
    } else {
        S_DrawAirBar(air * 100 / LARA_MAX_AIR);
    }
}

void __cdecl Overlay_MakeAmmoString(char *const string)
{
    for (char *c = string; *c != '\0'; c++) {
        if (*c == ' ') {
            continue;
        }

        if (*c - 'A' >= 0) {
            // ammo sprites
            *c += 0xC - 'A';
        } else {
            // digits
            *c += 1 - '0';
        }
    }
}

void __cdecl Overlay_DrawAmmoInfo(void)
{
    if (g_Lara.gun_status != LGS_READY || g_OverlayStatus <= 0
        || g_SaveGame.bonus_flag) {
        if (g_AmmoTextInfo != NULL) {
            Text_Remove(g_AmmoTextInfo);
            g_AmmoTextInfo = NULL;
        }
        return;
    }

    char buffer[80] = "";
    switch (g_Lara.gun_type) {
    case LGT_MAGNUMS:
        sprintf(buffer, "%5d", g_Lara.magnum_ammo.ammo);
        break;

    case LGT_UZIS:
        sprintf(buffer, "%5d", g_Lara.uzi_ammo.ammo);
        break;

    case LGT_SHOTGUN:
        sprintf(buffer, "%5d", g_Lara.shotgun_ammo.ammo / 6);
        break;

    case LGT_M16:
        sprintf(buffer, "%5d", g_Lara.m16_ammo.ammo);
        break;

    case LGT_ROCKET:
        sprintf(buffer, "%5d", g_Lara.grenade_ammo.ammo);
        break;

    case LGT_HARPOON:
        sprintf(buffer, "%5d", g_Lara.harpoon_ammo.ammo);
        break;

    default:
        return;
    }

    Overlay_MakeAmmoString(buffer);
    if (g_AmmoTextInfo != NULL) {
        Text_ChangeText(g_AmmoTextInfo, buffer);
    } else {
        g_AmmoTextInfo = Text_Create(AMMO_X, AMMO_Y, 0, buffer);
        Text_AlignRight(g_AmmoTextInfo, true);
    }
}

void __cdecl Overlay_InitialisePickUpDisplay(void)
{
    for (int i = 0; i < MAX_PICKUPS; i++) {
        g_Pickups[i].timer = 0;
    }
}

void __cdecl Overlay_DrawPickups(const bool pickup_state)
{
    const int32_t time = g_SaveGame.statistics.timer - m_OldGameTimer;
    m_OldGameTimer = g_SaveGame.statistics.timer;

    if (time < +0 || time >= 60) {
        return;
    }

    static const int32_t max_columns = 4;
    const int32_t cell_h = g_PhdWinWidth / 10;
    const int32_t cell_v = cell_h * 2 / 3;
    int32_t x = g_PhdWinWidth - cell_h;
    int32_t y = g_PhdWinHeight - cell_h;

    int32_t column = 0;
    for (int i = 0; i < 12; i++) {
        struct PICKUP_INFO *const pickup = &g_Pickups[i];
        if (pickup_state) {
            pickup->timer -= time;
        }

        if (pickup->timer <= 0) {
            pickup->timer = 0;
        } else {
            if (column == max_columns) {
                y -= cell_v;
                x = g_PhdWinWidth - cell_h;
            }
            column++;
            Output_DrawPickup(x, y, 12 * WALL_L, pickup->sprite, 0x1000);
        }
        x -= cell_h;
    }
}
