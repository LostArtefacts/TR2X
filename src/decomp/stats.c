#include "decomp/stats.h"

#include "game/input.h"
#include "game/music.h"
#include "game/text.h"
#include "global/funcs.h"
#include "global/types.h"
#include "global/vars.h"

#include <libtrx/log.h>

#include <stdio.h>

static int32_t m_ShowStatsTextMode = 0;

void __cdecl ShowStatsText(const char *const time_str, const int32_t type)
{
    char buffer[32];

    if (m_ShowStatsTextMode == 1) {
        ChangeRequesterItem(
            &g_StatsRequester, 0, g_GF_GameStrings[GF_S_GAME_MISC_TIME_TAKEN],
            REQ_ALIGN_LEFT, time_str, REQ_ALIGN_RIGHT);
        if (Display_Requester(&g_StatsRequester, type, 1)) {
            m_ShowStatsTextMode = 0;
        } else {
            g_InputDB = 0;
            g_Input = 0;
        }
        return;
    }

    g_StatsRequester.no_selector = 1;
    SetPCRequesterSize(&g_StatsRequester, 7, -32);
    g_StatsRequester.line_height = 18;
    g_StatsRequester.items_count = 0;
    g_StatsRequester.selected = 0;
    g_StatsRequester.line_offset = 0;
    g_StatsRequester.line_old_offset = 0;
    g_StatsRequester.pix_width = 304;
    g_StatsRequester.x_pos = 0;
    g_StatsRequester.z_pos = 0;
    g_StatsRequester.pitem_strings1 = g_ValidLevelStrings1;
    g_StatsRequester.pitem_strings2 = g_ValidLevelStrings2;
    g_StatsRequester.item_string_len = MAX_LEVEL_NAME_SIZE;

    Init_Requester(&g_StatsRequester);
    SetRequesterHeading(
        &g_StatsRequester, g_GF_LevelNames[g_CurrentLevel], REQ_CENTER, NULL,
        0);
    AddRequesterItem(
        &g_StatsRequester, g_GF_GameStrings[GF_S_GAME_MISC_TIME_TAKEN],
        REQ_ALIGN_LEFT, time_str, REQ_ALIGN_RIGHT);

    if (g_GF_NumSecrets) {
        char *ptr = buffer;
        int32_t num_secrets = 0;
        for (int32_t i = 0; i < 3; i++) {
            if (g_SaveGame.statistics.secrets & (1 << i)) {
                *ptr++ = 127 + i;
                num_secrets++;
            } else {
                *ptr++ = ' ';
                *ptr++ = ' ';
                *ptr++ = ' ';
            }
        }
        *ptr++ = '\0';
        if (num_secrets == 0) {
            sprintf(buffer, g_GF_GameStrings[GF_S_GAME_MISC_NONE]);
        }
        AddRequesterItem(
            &g_StatsRequester, g_GF_GameStrings[GF_S_GAME_MISC_SECRETS_FOUND],
            REQ_ALIGN_LEFT, buffer, REQ_ALIGN_RIGHT);
    }

    sprintf(buffer, "%d", g_SaveGame.statistics.kills);
    AddRequesterItem(
        &g_StatsRequester, g_GF_GameStrings[GF_S_GAME_MISC_KILLS],
        REQ_ALIGN_LEFT, buffer, REQ_ALIGN_RIGHT);

    sprintf(buffer, "%d", g_SaveGame.statistics.shots);
    AddRequesterItem(
        &g_StatsRequester, g_GF_GameStrings[GF_S_GAME_MISC_AMMO_USED],
        REQ_ALIGN_LEFT, buffer, REQ_ALIGN_RIGHT);

    sprintf(buffer, "%d", g_SaveGame.statistics.hits);
    AddRequesterItem(
        &g_StatsRequester, g_GF_GameStrings[GF_S_GAME_MISC_HITS],
        REQ_ALIGN_LEFT, buffer, REQ_ALIGN_RIGHT);

    if ((g_SaveGame.statistics.medipacks & 1) != 0) {
        sprintf(buffer, "%d.5", g_SaveGame.statistics.medipacks >> 1);
    } else {
        sprintf(buffer, "%d.0", g_SaveGame.statistics.medipacks >> 1);
    }
    AddRequesterItem(
        &g_StatsRequester, g_GF_GameStrings[GF_S_GAME_MISC_HEALTH_PACKS_USED],
        REQ_ALIGN_LEFT, buffer, REQ_ALIGN_RIGHT);

    const int32_t distance = g_SaveGame.statistics.distance / 445;
    if (distance < 1000) {
        sprintf(buffer, "%dm", distance);
    } else {
        sprintf(buffer, "%d.%02dkm", distance / 1000, distance % 100);
    }
    AddRequesterItem(
        &g_StatsRequester, g_GF_GameStrings[GF_S_GAME_MISC_DISTANCE_TRAVELLED],
        REQ_ALIGN_LEFT, buffer, REQ_ALIGN_RIGHT);

    m_ShowStatsTextMode = 1;
}

int32_t __cdecl LevelStats(const int32_t level_num)
{
    START_INFO *const start = &g_SaveGame.start[level_num];
    start->statistics.timer = g_SaveGame.statistics.timer;
    start->statistics.shots = g_SaveGame.statistics.shots;
    start->statistics.hits = g_SaveGame.statistics.hits;
    start->statistics.distance = g_SaveGame.statistics.distance;
    start->statistics.kills = g_SaveGame.statistics.kills;
    start->statistics.secrets = g_SaveGame.statistics.secrets;
    start->statistics.medipacks = g_SaveGame.statistics.medipacks;

    const int32_t sec = g_SaveGame.statistics.timer / FRAMES_PER_SECOND;
    char buffer[100];
    sprintf(buffer, "%02d:%02d:%02d", sec / 3600, (sec / 60) % 60, sec % 60);

    Music_Play(g_GameFlow.level_complete_track, false);

    TempVideoAdjust(g_HiRes, 1.0);
    FadeToPal(30, g_GamePalette8);
    Text_Init();
    S_CopyScreenToBuffer();

    while (g_Input & IN_SELECT) {
        Input_Update();
    }

    while (true) {
        S_InitialisePolyList(0);
        S_CopyBufferToScreen();

        Input_Update();

        if (g_GF_OverrideDir != (GAME_FLOW_DIR)-1) {
            break;
        }

        ShowStatsText(buffer, 0);
        Text_Draw();
        S_OutputPolyList();
        S_DumpScreen();

        if (g_InputDB & IN_SELECT) {
            break;
        }
    }

    CreateStartInfo(level_num + 1);
    g_SaveGame.current_level = level_num + 1;
    start->available = 0;
    S_FadeToBlack();
    TempVideoRemove();
    return 0;
}

int32_t __cdecl GameStats(const int32_t level_num)
{
    START_INFO *const start = &g_SaveGame.start[level_num];
    start->statistics.timer = g_SaveGame.statistics.timer;
    start->statistics.shots = g_SaveGame.statistics.shots;
    start->statistics.hits = g_SaveGame.statistics.hits;
    start->statistics.distance = g_SaveGame.statistics.distance;
    start->statistics.kills = g_SaveGame.statistics.kills;
    start->statistics.secrets = g_SaveGame.statistics.secrets;
    start->statistics.medipacks = g_SaveGame.statistics.medipacks;

    Text_Init();

    while (g_Input & IN_SELECT) {
        Input_Update();
    }

    while (true) {
        S_InitialisePolyList(0);
        S_CopyBufferToScreen();

        Input_Update();

        if (g_GF_OverrideDir != (GAME_FLOW_DIR)-1) {
            break;
        }

        ShowEndStatsText();
        Text_Draw();
        S_OutputPolyList();
        S_DumpScreen();

        if (g_InputDB & IN_SELECT) {
            break;
        }
    }

    g_SaveGame.bonus_flag = 1;
    for (int32_t level = LV_FIRST; level <= g_GameFlow.num_levels; level++) {
        ModifyStartInfo(level);
    }
    g_SaveGame.current_level = LV_FIRST;

    S_DontDisplayPicture();
    return 0;
}
