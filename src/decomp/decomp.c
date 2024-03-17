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

void __cdecl ScreenShotPCX(void)
{
    LPDDS screen = g_SavedAppSettings.render_mode == RM_SOFTWARE
        ? g_RenderBufferSurface
        : g_PrimaryBufferSurface;

    DDSURFACEDESC desc;
    desc.dwSize = sizeof(desc);

    int32_t result;
    while (true) {
        result = IDirectDrawSurface_Lock(screen, 0, &desc, 0, 0);
        if (result != DDERR_WASSTILLDRAWING) {
            break;
        }
    }

    if (result == DDERR_SURFACELOST) {
        IDirectDrawSurface_Restore(screen);
    }

    if (FAILED(result)) {
        return;
    }

    uint8_t *pcx_data;
    int32_t pcx_size = CompPCX(
        desc.lpSurface, desc.dwWidth, desc.dwHeight, g_GamePalette8, &pcx_data);

    IDirectDrawSurface_Unlock(screen, &desc);
    if (!pcx_size) {
        return;
    }

    g_ScreenshotCounter++;
    if (g_ScreenshotCounter > 9999) {
        g_ScreenshotCounter = 1;
    }

    char file_name[20];
    sprintf(file_name, "tomb%04d.pcx", g_ScreenshotCounter);

    HANDLE handle = CreateFileA(
        file_name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
        NULL);
    DWORD bytes_written;
    WriteFile(handle, pcx_data, pcx_size, &bytes_written, 0);
    CloseHandle(handle);
    GlobalFree(pcx_data);
}

size_t __cdecl CompPCX(
    uint8_t *bitmap, int32_t width, int32_t height, RGB888 *palette,
    uint8_t **pcx_data)
{
    *pcx_data = (uint8_t *)GlobalAlloc(
        GMEM_FIXED,
        sizeof(PCX_HEADER) + sizeof(RGB888) * 256 + width * height * 2);
    if (*pcx_data == NULL) {
        return 0;
    }

    PCX_HEADER *pcx_header = (PCX_HEADER *)*pcx_data;
    pcx_header->manufacturer = 10;
    pcx_header->version = 5;
    pcx_header->rle = 1;
    pcx_header->bpp = 8;
    pcx_header->planes = 1;
    pcx_header->x_min = 0;
    pcx_header->y_min = 0;
    pcx_header->x_max = width - 1;
    pcx_header->y_max = height - 1;
    pcx_header->h_dpi = width;
    pcx_header->v_dpi = height;
    pcx_header->bytes_per_line = width;

    uint8_t *pic_data = *pcx_data + sizeof(PCX_HEADER);
    for (int y = 0; y < height; y++) {
        pic_data += EncodeLinePCX(bitmap, width, pic_data);
        bitmap += width;
    }

    *pic_data++ = 0x0C;
    for (int i = 0; i < 256; i++) {
        *pic_data++ = palette[i].red;
        *pic_data++ = palette[i].green;
        *pic_data++ = palette[i].blue;
    }

    return pic_data - *pcx_data + sizeof(RGB888) * 256;
}

size_t __cdecl EncodeLinePCX(
    const uint8_t *src, const int32_t width, uint8_t *dst)
{
    const uint8_t *const dst_start = dst;
    int32_t run_count = 1;
    uint8_t last = *src;

    for (int32_t i = 1; i < width; i++) {
        uint8_t current = *src++;
        if (*src == last) {
            run_count++;
            if (run_count == 63) {
                const size_t add = EncodePutPCX(last, 0x3Fu, dst);
                if (add == 0) {
                    return 0;
                }
                dst += add;
                run_count = 0;
            }
        } else {
            if (run_count != 0) {
                const size_t add = EncodePutPCX(last, run_count, dst);
                if (add == 0) {
                    return 0;
                }
                dst += add;
            }
            last = current;
            run_count = 1;
        }
    }

    if (run_count != 0) {
        const size_t add = EncodePutPCX(last, run_count, dst);
        if (add == 0) {
            return 0;
        }
        dst += add;
    }

    const size_t total = dst - dst_start;
    return total;
}

size_t __cdecl EncodePutPCX(uint8_t value, uint8_t num, uint8_t *buffer)
{
    if (num == 0 || num > 63) {
        return 0;
    }

    if (num == 1 && (value & 0xC0) != 0xC0) {
        buffer[0] = value;
        return 1;
    }

    buffer[0] = num | 0xC0;
    buffer[1] = value;
    return 2;
}
