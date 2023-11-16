#include "game/shell.h"

#include "game/input.h"
#include "global/funcs.h"
#include "global/vars.h"

#include <stdio.h>

// TODO: refactor the hell out of me
BOOL __cdecl Shell_Main(void)
{
    g_HiRes = 0;
    g_ScreenSizer = 0;
    g_GameSizer = 1.0;
    g_GameSizerCopy = 1.0;

    if (!S_InitialiseSystem()) {
        return false;
    }

    if (!GF_LoadScriptFile("data\\tombPC.dat")) {
        Shell_ExitSystem("GameMain: could not load script file");
        return false;
    }

    Sound_Init();
    InitialiseStartInfo();
    S_FrontEndCheck();
    S_LoadSettings();

    g_HiRes = -1;
    g_GameMemoryPtr = GlobalAlloc(0, 0x380000u);

    if (!g_GameMemoryPtr) {
        strcpy(g_ErrorMessage, "GameMain: could not allocate malloc_buffer");
        return false;
    }

    g_HiRes = 0;
    TempVideoAdjust(1, 1.0);
    Input_Update();

    g_IsVidModeLock = 1;
    S_DisplayPicture("data\\legal.pcx", 0);
    S_InitialisePolyList(0);
    S_CopyBufferToScreen();
    S_OutputPolyList();
    S_DumpScreen();
    FadeToPal(30, g_GamePalette8);
    S_Wait(180, 1);
    S_FadeToBlack();
    S_DontDisplayPicture();
    g_IsVidModeLock = 0;

    bool is_frontend_fail = GF_DoFrontEndSequence();
    if (g_IsGameToExit) {
        return true;
    }

    if (is_frontend_fail == 1) {
        strcpy(g_ErrorMessage, "GameMain: failed in GF_DoFrontEndSequence()");
        return false;
    }

    S_FadeToBlack();
    int16_t gf_option = g_GameFlow.first_option;
    g_NoInputCounter = 0;

    bool is_loop_continued = true;
    while (is_loop_continued) {
        const int16_t gf_dir = gf_option & 0xFF00;
        const int16_t gf_param = gf_option & 0x00FF;

        switch (gf_dir) {
        case GFD_START_GAME:
            if (g_GameFlow.single_level >= 0) {
                gf_option = GF_DoLevelSequence(g_GameFlow.single_level, 1);
            } else {
                if (gf_param > g_GameFlow.num_levels) {
                    sprintf(
                        g_ErrorMessage,
                        "GameMain: STARTGAME with invalid level number (%d)",
                        gf_param);
                    return false;
                }
                gf_option = GF_DoLevelSequence(gf_param, 1);
            }
            break;

        case GFD_START_SAVED_GAME:
            S_LoadGame(&g_SaveGame, sizeof(SAVEGAME_INFO), gf_param);
            if (g_SaveGame.current_level > g_GameFlow.num_levels) {
                sprintf(
                    g_ErrorMessage,
                    "GameMain: STARTSAVEDGAME with invalid level number (%d)",
                    g_SaveGame.current_level);
                return false;
            }
            gf_option = GF_DoLevelSequence(g_SaveGame.current_level, 2);
            break;

        case GFD_START_CINE:
            Game_Cutscene_Start(gf_param);
            gf_option = GFD_EXIT_TO_TITLE;
            break;

        case GFD_START_DEMO:
            gf_option = Demo_Control(-1);
            break;

        case GFD_LEVEL_COMPLETE:
            gf_option = LevelCompleteSequence();
            break;

        case GFD_EXIT_TO_TITLE:
        case GFD_EXIT_TO_OPTION:
            if (g_GameFlow.flags & GFF_TITLE_DISABLED) {
                if (g_GameFlow.title_replace < 0
                    || g_GameFlow.title_replace == GFD_EXIT_TO_TITLE) {
                    strcpy(
                        g_ErrorMessage,
                        "GameMain Failed: Title disabled & no replacement");
                    return false;
                }
                gf_option = g_GameFlow.title_replace;
            } else {
                gf_option = TitleSequence();
                g_GF_StartGame = 1;
            }
            break;

        default:
            is_loop_continued = false;
            break;
        }
    }

    S_SaveSettings();
    GameBuf_Shutdown();
    return true;
}

void __cdecl Shell_ExitSystem(const char *message)
{
    GameBuf_Shutdown();
    strcpy(g_ErrorMessage, message);
    Shell_Shutdown();
    Shell_Cleanup();
    exit(1);
}
