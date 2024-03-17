#include "decomp/decomp.h"

#include "game/music.h"
#include "game/shell.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "specific/s_audio_sample.h"

#include <stdio.h>

int32_t __cdecl GameInit(void)
{
    Music_Shutdown();
    UT_InitAccurateTimer();
    // clang-format off
    return WinVidInit()
        && Direct3DInit()
        && RenderInit()
        && InitTextures()
        && S_Audio_Sample_Init()
        && WinInputInit()
        && TIME_Init()
        && HWR_Init()
        && BGND_Init();
    // clang-format on
}

int32_t __stdcall WinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,
    int32_t nShowCmd)
{
    BOOL is_setup_requested; // ebx
    int32_t app_settings_status; // eax
    int32_t v9; // esi
    char finish; // [esp+2Ch] [ebp+8h]

    g_GameModule = hInstance;
    g_CmdLine = lpCmdLine;
    HWND game_window = WinVidFindGameWindow();
    if (game_window) {
        HWND setup_window = SE_FindSetupDialog();
        if (!setup_window) {
            setup_window = game_window;
        }
        SetForegroundWindow(setup_window);
        return 0;
    }

    // TODO: install exception handler

    is_setup_requested = UT_FindArg("setup") != 0;
    if (!GameInit()) {
        UT_ErrorBox(IDS_DX5_REQUIRED, NULL);
        goto cleanup;
    }

    app_settings_status = SE_ReadAppSettings(&g_SavedAppSettings);
    if (!app_settings_status) {
        goto cleanup;
    }

    if (app_settings_status == 2 || is_setup_requested) {
        if (SE_ShowSetupDialog(0, app_settings_status == 2)) {
            SE_WriteAppSettings(&g_SavedAppSettings);
            if (is_setup_requested) {
                goto cleanup;
            }
        } else {
            goto cleanup;
        }
    }

    int32_t result = WinGameStart();
    if (result) {
        Shell_Shutdown();
        RenderErrorBox(result);
        if (!SE_ShowSetupDialog(0, 0)) {
            goto cleanup;
        }
        SE_WriteAppSettings(&g_SavedAppSettings);
    } else {
        g_StopInventory = 0;
        g_IsGameToExit = 0;
        Shell_Main();
        Shell_Shutdown();
        SE_WriteAppSettings(&g_SavedAppSettings);
    }

cleanup:
    Shell_Cleanup();
    return g_AppResultCode;
}

const char *__cdecl DecodeErrorMessage(int32_t error_code)
{
    return g_ErrorMessages[error_code];
}

int32_t __cdecl RenderErrorBox(int32_t error_code)
{
    char buffer[128];
    const char *decoded = DecodeErrorMessage(error_code);
    sprintf(buffer, "Render init failed with \"%s\"", decoded);
    return UT_MessageBox(buffer, 0);
}
