#include "decomp/decomp.h"

#include "game/music.h"
#include "game/shell.h"
#include "game/text.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "lib/dinput.h"
#include "specific/s_audio_sample.h"

#include <dinput.h>
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

    bool is_setup_requested = UT_FindArg("setup") != 0;
    if (!GameInit()) {
        UT_ErrorBox(IDS_DX5_REQUIRED, NULL);
        goto cleanup;
    }

    int32_t app_settings_status = SE_ReadAppSettings(&g_SavedAppSettings);
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

void __cdecl ScreenshotPCX(void)
{
    LPDDS screen = g_SavedAppSettings.render_mode == RM_SOFTWARE
        ? g_RenderBufferSurface
        : g_PrimaryBufferSurface;

    DDSURFACEDESC desc = { .dwSize = sizeof(DDSURFACEDESC), 0 };

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

void __cdecl ScreenshotTGA(IDirectDrawSurface3 *screen, int32_t bpp)
{
    DDSURFACEDESC desc = {
        .dwSize = sizeof(DDSURFACEDESC),
        0,
    };

    if (FAILED(WinVidBufferLock(screen, &desc, 0x21u))) {
        return;
    }

    const int32_t width = desc.dwWidth;
    const int32_t height = desc.dwHeight;

    g_ScreenshotCounter++;

    char file_name[20];
    sprintf(file_name, "tomb%04d.tga", g_ScreenshotCounter);

    FILE *handle = fopen(file_name, "wb");
    if (!handle) {
        goto cleanup;
    }

    const TGA_HEADER header = {
        .id_length = 0,
        .color_map_type = 0,
        .data_type_code = 2, // Uncompressed, RGB images
        .color_map_origin = 0,
        .color_map_length = 0,
        .color_map_depth = 0,
        .x_origin = 0,
        .y_origin = 0,
        .width = width,
        .height = height,
        .bpp = 16,
        .image_descriptor = 0,
    };

    fwrite(&header, sizeof(TGA_HEADER), 1, handle);

    uint8_t *tga_pic =
        (uint8_t *)GlobalAlloc(GMEM_FIXED, width * height * (bpp / 8));
    uint8_t *src = desc.lpSurface + desc.lPitch * (height - 1);

    uint8_t *dst = tga_pic;
    for (int32_t y = 0; y < height; y++) {
        if (desc.ddpfPixelFormat.dwRBitMask == 0xF800) {
            // R5G6B5 - transform
            for (int32_t x = 0; x < width; x++) {
                const uint16_t sample = ((uint16_t *)src)[x];
                ((uint16_t *)dst)[x] =
                    ((sample & 0xFFC0) >> 1) | (sample & 0x001F);
            }
        } else {
            // X1R5G5B5 - good
            memcpy(dst, src, sizeof(uint16_t) * width);
        }
        src -= desc.lPitch;
        dst += sizeof(uint16_t) * width;
    }
    fwrite(tga_pic, 2 * height * width, 1, handle);

cleanup:
    if (tga_pic) {
        GlobalFree(tga_pic);
    }

    if (handle) {
        fclose(handle);
    }
    WinVidBufferUnlock(screen, &desc);
}

void __cdecl Screenshot(LPDDS screen)
{
    DDSURFACEDESC desc = { 0 };
    desc.dwSize = sizeof(DDSURFACEDESC);

    if (SUCCEEDED(IDirectDrawSurface_GetSurfaceDesc(screen, &desc))) {
        if (desc.ddpfPixelFormat.dwRGBBitCount == 8) {
            ScreenshotPCX();
        } else if (desc.ddpfPixelFormat.dwRGBBitCount == 16) {
            ScreenshotTGA(screen, 16);
        }
    }
}

bool __cdecl DInputCreate(void)
{
    return SUCCEEDED(DirectInputCreate(g_GameModule, 1280, &g_DInput, NULL));
}

void __cdecl DInputRelease(void)
{
    if (g_DInput) {
        IDirectInput_Release(g_DInput);
        g_DInput = NULL;
    }
}

void __cdecl WinInReadKeyboard(uint8_t *input_data)
{
    if (SUCCEEDED(IDirectInputDevice_GetDeviceState(
            IDID_SysKeyboard, 256, input_data))) {
        return;
    }

    if (SUCCEEDED(IDirectInputDevice_Acquire(IDID_SysKeyboard))
        && SUCCEEDED(IDirectInputDevice_GetDeviceState(
            IDID_SysKeyboard, 256, input_data))) {
        return;
    }

    memset(input_data, 0, 256);
}

int32_t __cdecl WinGameStart(void)
{
    // try {
    WinVidStart();
    RenderStart(1);
    S_Audio_Sample_Init2(0);
    WinInStart();
    // } catch (int error) {
    //     return error;
    // }
    return 0;
}

void __cdecl Shell_Shutdown(void)
{
    WinInFinish();
    S_Audio_Sample_Shutdown();
    RenderFinish(1);
    WinVidFinish();
    WinVidHideGameWindow();
    if (g_ErrorMessage[0]) {
        MessageBoxA(NULL, g_ErrorMessage, NULL, MB_ICONWARNING);
    }
}

int16_t __cdecl TitleSequence(void)
{
    Text_Init();
    TempVideoAdjust(1, 1.0);

    g_NoInputCounter = 0;

    if (!g_IsTitleLoaded) {
        if (!Level_Initialise(0, 0)) {
            return GFD_EXIT_GAME;
        }
        g_IsTitleLoaded = TRUE;
    }

    S_DisplayPicture("data/title.pcx", 1);
    if (g_GameFlow.title_track) {
        Music_Play(g_GameFlow.title_track, 1);
    }

    Display_Inventory(INV_TITLE_MODE);

    S_FadeToBlack();
    S_DontDisplayPicture();

    Music_Stop();
    if (g_IsResetFlag) {
        g_IsResetFlag = 0;
        return GFD_START_DEMO;
    }

    if (g_InventoryChosen == O_PHOTO_OPTION) {
        return GFD_START_GAME | LV_GYM;
    }

    if (g_InventoryChosen == O_PASSPORT_OPTION) {
        const int32_t slot_num = g_InventoryExtraData[1];

        if (g_InventoryExtraData[0] == 0) {
            Inv_RemoveAllItems();
            S_LoadGame(&g_SaveGame, sizeof(SAVEGAME_INFO), slot_num);
            return GFD_START_SAVED_GAME | slot_num;
        }

        if (g_InventoryExtraData[0] == 1) {
            InitialiseStartInfo();
            int32_t level_id = LV_FIRST;
            if ((g_GameFlow.flags & GFF_SELECT_ANY_LEVEL) != 0) {
                level_id = LV_FIRST + slot_num;
            }
            return GFD_START_GAME | level_id;
        }
        return GFD_EXIT_GAME;
    }

    return GFD_EXIT_GAME;
}

void __cdecl WinVidSetMinWindowSize(int32_t width, int32_t height)
{
    g_MinWindowClientWidth = width;
    g_MinWindowClientHeight = height;
    GameWindowCalculateSizeFromClient(&width, &height);
    g_MinWindowWidth = width;
    g_MinWindowHeight = height;
    g_IsMinWindowSizeSet = true;
}

void __cdecl WinVidSetMaxWindowSize(int32_t width, int32_t height)
{
    g_MaxWindowClientWidth = width;
    g_MaxWindowClientHeight = height;
    GameWindowCalculateSizeFromClient(&width, &height);
    g_MaxWindowWidth = width;
    g_MaxWindowHeight = height;
    g_IsMaxWindowSizeSet = true;
}

void __cdecl WinVidClearMinWindowSize(void)
{
    g_IsMinWindowSizeSet = false;
}

void __cdecl WinVidClearMaxWindowSize(void)
{
    g_IsMaxWindowSizeSet = false;
}

int32_t __cdecl CalculateWindowWidth(const int32_t width, const int32_t height)
{
    if (g_SavedAppSettings.aspect_mode == AM_4_3) {
        return 4 * height / 3;
    }
    if (g_SavedAppSettings.aspect_mode == AM_16_9) {
        return 16 * height / 9;
    }
    return width;
}

int32_t __cdecl CalculateWindowHeight(const int32_t width, const int32_t height)
{
    if (g_SavedAppSettings.aspect_mode == AM_4_3) {
        return (3 * width) / 4;
    }
    if (g_SavedAppSettings.aspect_mode == AM_16_9) {
        return (9 * width) / 16;
    }
    return height;
}

bool __cdecl WinVidGetMinMaxInfo(LPMINMAXINFO info)
{
    if (!g_IsGameWindowCreated) {
        return false;
    }

    if (g_IsGameFullScreen) {
        info->ptMaxTrackSize.x = g_FullScreenWidth;
        info->ptMaxTrackSize.y = g_FullScreenHeight;
        info->ptMinTrackSize.x = g_FullScreenWidth;
        info->ptMinTrackSize.y = g_FullScreenHeight;
        info->ptMaxSize.x = g_FullScreenWidth;
        info->ptMaxSize.y = g_FullScreenHeight;
        return true;
    }

    if (g_IsMinWindowSizeSet) {
        info->ptMinTrackSize.x = g_MinWindowWidth;
        info->ptMinTrackSize.y = g_MinWindowHeight;
    }

    if (g_IsMinMaxInfoSpecial) {
        int32_t width = g_GameWindowWidth;
        int32_t height = g_GameWindowHeight;
        GameWindowCalculateSizeFromClient(&width, &height);
        info->ptMaxSize.x = width;
        info->ptMaxTrackSize.x = width;
        info->ptMaxSize.y = height;
        info->ptMaxTrackSize.y = height;
    } else if (g_IsMaxWindowSizeSet) {
        info->ptMaxTrackSize.x = g_MaxWindowWidth;
        info->ptMaxTrackSize.y = g_MaxWindowHeight;
        info->ptMaxSize.x = g_MaxWindowWidth;
        info->ptMaxSize.y = g_MaxWindowHeight;
    }

    return g_IsMinWindowSizeSet || g_IsMaxWindowSizeSet;
}

HWND __cdecl WinVidFindGameWindow(void)
{
    return FindWindowA(CLASS_NAME, WINDOW_NAME);
}

bool __cdecl WinVidSpinMessageLoop(bool need_wait)
{
    if (g_IsMessageLoopClosed) {
        return 0;
    }

    g_MessageLoopCounter++;

    do {
        if (need_wait) {
            WaitMessage();
        } else {
            need_wait = true;
        }

        MSG msg;
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
            if (msg.message == WM_QUIT) {
                g_AppResultCode = msg.wParam;
                g_IsMessageLoopClosed = true;
                g_IsGameToExit = true;
                g_StopInventory = true;
                g_MessageLoopCounter--;
                return 0;
            }
        }
    } while (!g_IsGameWindowActive || g_IsGameWindowMinimized);

    g_MessageLoopCounter--;
    return true;
}

void __cdecl WinVidShowGameWindow(const int32_t cmd_show)
{
    if (cmd_show != SW_SHOW || !g_IsGameWindowShow) {
        g_IsGameWindowUpdating = true;
        ShowWindow(g_GameWindowHandle, cmd_show);
        UpdateWindow(g_GameWindowHandle);
        g_IsGameWindowUpdating = false;
        g_IsGameWindowShow = true;
    }
}

void __cdecl WinVidHideGameWindow(void)
{
    if (g_IsGameWindowShow) {
        g_IsGameWindowUpdating = true;
        ShowWindow(g_GameWindowHandle, SW_HIDE);
        UpdateWindow(g_GameWindowHandle);
        g_IsGameWindowUpdating = false;
        g_IsGameWindowShow = false;
    }
}

void __cdecl WinVidSetGameWindowSize(int32_t width, int32_t height)
{
    GameWindowCalculateSizeFromClient(&width, &height);
    SetWindowPos(
        g_GameWindowHandle, NULL, 0, 0, width, height,
        SWP_NOCOPYBITS | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}

bool __cdecl ShowDDrawGameWindow(bool active)
{
    if (!g_GameWindowHandle || !g_DDraw) {
        return false;
    }
    if (g_IsDDrawGameWindowShow) {
        return true;
    }

    RECT rect;
    GetWindowRect(g_GameWindowHandle, &rect);
    g_GameWindowX = rect.left;
    g_GameWindowY = rect.top;

    if (active) {
        WinVidShowGameWindow(SW_SHOW);
    }

    g_IsGameWindowUpdating = true;
    uint32_t flags = DDSCL_ALLOWMODEX | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT
        | DDSCL_FULLSCREEN;
    if (!active)
        flags |= DDSCL_NOWINDOWCHANGES;
    const HRESULT result =
        IDirectDraw_SetCooperativeLevel(g_DDraw, g_GameWindowHandle, flags);
    g_IsGameWindowUpdating = false;
    if (FAILED(result)) {
        return false;
    }

    g_IsDDrawGameWindowShow = true;
    return true;
}

bool __cdecl HideDDrawGameWindow(void)
{
    if (!g_GameWindowHandle || !g_DDraw) {
        return false;
    }
    if (!g_IsDDrawGameWindowShow) {
        return true;
    }

    WinVidHideGameWindow();
    g_IsGameWindowUpdating = true;
    const HRESULT result = IDirectDraw_SetCooperativeLevel(
        g_DDraw, g_GameWindowHandle, DDSCL_NORMAL);
    if (SUCCEEDED(result)) {
        g_IsDDrawGameWindowShow = false;
        SetWindowPos(
            g_GameWindowHandle, NULL, g_GameWindowX, g_GameWindowY, 0, 0,
            SWP_NOCOPYBITS | SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
    }

    g_IsGameWindowUpdating = false;
    return SUCCEEDED(result);
}

HRESULT __cdecl DDrawSurfaceCreate(LPDDSDESC dsp, LPDDS *surface)
{
    LPDIRECTDRAWSURFACE sub_surface;
    HRESULT rc = IDirectDraw_CreateSurface(g_DDraw, dsp, &sub_surface, NULL);

    if SUCCEEDED (rc) {
        rc = IDirectDrawSurface_QueryInterface(
            sub_surface, &g_IID_IDirectDrawSurface3, (LPVOID *)surface);
        IDirectDrawSurface_Release(sub_surface);
    }

    return rc;
}

HRESULT __cdecl DDrawSurfaceRestoreLost(
    LPDDS surface1, LPDDS surface2, bool blank)
{
    HRESULT rc = IDirectDrawSurface_IsLost(surface1);
    if (rc != DDERR_SURFACELOST) {
        return rc;
    }
    rc = IDirectDrawSurface_Restore(surface2 != NULL ? surface2 : surface1);
    if (blank && SUCCEEDED(rc)) {
        WinVidClearBuffer(surface1, 0, 0);
    }
    return rc;
}

bool __cdecl WinVidClearBuffer(LPDDS surface, LPRECT rect, DWORD fill_color)
{
    DDBLTFX blt = { .dwFillColor = fill_color, .dwSize = sizeof(DDBLTFX), 0 };
    HRESULT rc = IDirectDrawSurface_Blt(
        surface, rect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &blt);
    return SUCCEEDED(rc);
}

HRESULT __cdecl WinVidBufferLock(LPDDS surface, LPDDSDESC desc, DWORD flags)
{
    memset(desc, 0, sizeof(DDSURFACEDESC));
    desc->dwSize = sizeof(DDSURFACEDESC);
    HRESULT rc = IDirectDrawSurface_Lock(surface, NULL, desc, flags, NULL);
    if (SUCCEEDED(rc)) {
        g_LockedBufferCount++;
    }
    return rc;
}

HRESULT __cdecl WinVidBufferUnlock(LPDDS surface, LPDDSDESC desc)
{
    HRESULT rc = surface->lpVtbl->Unlock(surface, desc->lpSurface);
    if (SUCCEEDED(rc)) {
        g_LockedBufferCount--;
    }
    return rc;
}

bool __cdecl WinVidCopyBitmapToBuffer(LPDDS surface, const BYTE *bitmap)
{
    DDSURFACEDESC desc;
    if (FAILED(
            WinVidBufferLock(surface, &desc, DDLOCK_WRITEONLY | DDLOCK_WAIT))) {
        return false;
    }

    const uint8_t *src = (const uint8_t *)bitmap;
    uint8_t *dst = (uint8_t *)desc.lpSurface;
    for (int i = 0; i < (int)desc.dwHeight; i++) {
        memcpy(dst, src, desc.dwWidth);
        src += desc.dwWidth;
        dst += desc.lPitch;
    }
    WinVidBufferUnlock(surface, &desc);
    return true;
}

DWORD __cdecl GetRenderBitDepth(const uint32_t rgb_bit_count)
{
    switch (rgb_bit_count) {
        // clang-format off
        case 1:    return DDBD_1;
        case 2:    return DDBD_2;
        case 4:    return DDBD_4;
        case 8:    return DDBD_8;
        case 0x10: return DDBD_16;
        case 0x18: return DDBD_24;
        case 0x20: return DDBD_32;
        // clang-format on
    }
    return 0;
}

void __thiscall WinVidGetColorBitMasks(
    COLOR_BIT_MASKS *bm, LPDDPIXELFORMAT pixel_format)
{
    bm->dwRBitMask = pixel_format->dwRBitMask;
    bm->dwGBitMask = pixel_format->dwGBitMask;
    bm->dwBBitMask = pixel_format->dwBBitMask;
    bm->dwRGBAlphaBitMask = pixel_format->dwRGBAlphaBitMask;
    BitMaskGetNumberOfBits(bm->dwRBitMask, &bm->dwRBitDepth, &bm->dwRBitOffset);
    BitMaskGetNumberOfBits(bm->dwGBitMask, &bm->dwGBitDepth, &bm->dwGBitOffset);
    BitMaskGetNumberOfBits(bm->dwBBitMask, &bm->dwBBitDepth, &bm->dwBBitOffset);
    BitMaskGetNumberOfBits(
        bm->dwRGBAlphaBitMask, &bm->dwRGBAlphaBitDepth,
        &bm->dwRGBAlphaBitOffset);
}

void __cdecl BitMaskGetNumberOfBits(
    DWORD bit_mask, DWORD *bit_depth, DWORD *bit_offset)
{
    if (!bit_mask) {
        *bit_offset = 0;
        *bit_depth = 0;
        return;
    }

    int32_t i;

    for (i = 0; (bit_mask & 1) == 0; i++) {
        bit_mask >>= 1;
    }
    *bit_offset = i;

    for (i = 0; bit_mask != 0; i++) {
        bit_mask >>= 1;
    }
    *bit_depth = i;
}

DWORD __cdecl CalculateCompatibleColor(
    const COLOR_BIT_MASKS *const mask, const int32_t red, const int32_t green,
    const int32_t blue, const int32_t alpha)
{
    // clang-format off
    return (
        (red   >> (8 - mask->dwRBitDepth) << mask->dwRBitOffset) |
        (green >> (8 - mask->dwGBitDepth) << mask->dwGBitOffset) |
        (blue  >> (8 - mask->dwBBitDepth) << mask->dwBBitOffset) |
        (alpha >> (8 - mask->dwRGBAlphaBitDepth) << mask->dwRGBAlphaBitOffset)
    );
    // clang-format on
}

bool __cdecl WinVidGetDisplayMode(DISPLAY_MODE *disp_mode)
{
    DDSDESC dsp = { .dwSize = sizeof(DDSDESC), 0 };

    if (FAILED(IDirectDraw_GetDisplayMode(g_DDraw, &dsp))) {
        return false;
    }

    // clang-format off
    if (!(dsp.dwFlags & DDSD_WIDTH)
        || !(dsp.dwFlags & DDSD_HEIGHT)
        || !(dsp.dwFlags & DDSD_PIXELFORMAT)
        || !(dsp.ddpfPixelFormat.dwFlags & DDPF_RGB)
    ) {
        return false;
    }
    // clang-format on

    disp_mode->width = dsp.dwWidth;
    disp_mode->height = dsp.dwHeight;
    disp_mode->bpp = dsp.ddpfPixelFormat.dwRGBBitCount;
    disp_mode->vga = (dsp.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) != 0
        ? VGA_256_COLOR
        : VGA_NO_VGA;
    return true;
}

bool __cdecl WinVidGoFullScreen(DISPLAY_MODE *disp_mode)
{
    g_FullScreenWidth = disp_mode->width;
    g_FullScreenHeight = disp_mode->height;
    g_FullScreenBPP = disp_mode->bpp;
    g_FullScreenVGA = disp_mode->vga;

    if (!ShowDDrawGameWindow(true)) {
        return false;
    }

    g_IsGameWindowUpdating = true;
    const HRESULT rc = IDirectDraw4_SetDisplayMode(
        g_DDraw, disp_mode->width, disp_mode->height, disp_mode->bpp, 0,
        disp_mode->vga == VGA_STANDARD ? DDSDM_STANDARDVGAMODE : 0);
    g_IsGameWindowUpdating = false;

    if (FAILED(rc)) {
        return false;
    }

    g_IsGameFullScreen = true;
    return true;
}
