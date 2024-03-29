#pragma once

#include "global/types.h"

#include <stdint.h>
#include <windows.h>

int32_t __cdecl GameInit(void);
int32_t __stdcall WinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,
    int32_t nShowCmd);
const char *__cdecl DecodeErrorMessage(int32_t error_code);
int32_t __cdecl RenderErrorBox(int32_t error_code);
void __cdecl ScreenshotPCX(void);
size_t __cdecl CompPCX(
    uint8_t *bitmap, int32_t width, int32_t height, RGB888 *palette,
    uint8_t **pcx_data);
size_t __cdecl EncodeLinePCX(const uint8_t *src, int32_t width, uint8_t *dst);
size_t __cdecl EncodePutPCX(uint8_t value, uint8_t num, uint8_t *buffer);
void __cdecl Screenshot(LPDDS screen);
bool __cdecl DInputCreate(void);
void __cdecl DInputRelease(void);
void __cdecl WinInReadKeyboard(uint8_t *input_data);
int32_t __cdecl WinGameStart(void);
void __cdecl Shell_Shutdown(void);
int16_t __cdecl TitleSequence(void);
void __cdecl WinVidSetMinWindowSize(int32_t width, int32_t height);
void __cdecl WinVidSetMaxWindowSize(int32_t width, int32_t height);
void __cdecl WinVidClearMinWindowSize(void);
void __cdecl WinVidClearMaxWindowSize(void);
int32_t __cdecl CalculateWindowWidth(int32_t width, int32_t height);
int32_t __cdecl CalculateWindowHeight(int32_t width, int32_t height);
bool __cdecl WinVidGetMinMaxInfo(LPMINMAXINFO info);
HWND __cdecl WinVidFindGameWindow(void);
bool __cdecl WinVidSpinMessageLoop(bool need_wait);
void __cdecl WinVidShowGameWindow(int32_t cmd_show);
void __cdecl WinVidHideGameWindow(void);
void __cdecl WinVidSetGameWindowSize(int32_t width, int32_t height);
bool __cdecl ShowDDrawGameWindow(bool active);
bool __cdecl HideDDrawGameWindow(void);
HRESULT __cdecl DDrawSurfaceCreate(LPDDSDESC dsp, LPDDS *surface);
HRESULT __cdecl DDrawSurfaceRestoreLost(
    LPDDS surface1, LPDDS surface2, bool blank);
bool __cdecl WinVidClearBuffer(LPDDS surface, LPRECT rect, DWORD fill_color);
HRESULT __cdecl WinVidBufferLock(LPDDS surface, LPDDSDESC desc, DWORD flags);
HRESULT __cdecl WinVidBufferUnlock(LPDDS surface, LPDDSDESC desc);
bool __cdecl WinVidCopyBitmapToBuffer(LPDDS surface, const BYTE *bitmap);
DWORD __cdecl GetRenderBitDepth(uint32_t rgb_bit_count);
void __thiscall WinVidGetColorBitMasks(
    COLOR_BIT_MASKS *bm, LPDDPIXELFORMAT pixel_format);
void __cdecl BitMaskGetNumberOfBits(
    DWORD bit_mask, DWORD *bit_depth, DWORD *bit_offset);
DWORD __cdecl CalculateCompatibleColor(
    const COLOR_BIT_MASKS *mask, int32_t red, int32_t green, int32_t blue,
    int32_t alpha);
bool __cdecl WinVidGetDisplayMode(DISPLAY_MODE *disp_mode);
bool __cdecl WinVidGoFullScreen(DISPLAY_MODE *disp_mode);
bool __cdecl WinVidGoWindowed(
    int32_t width, int32_t height, DISPLAY_MODE *disp_mode);
void __cdecl WinVidSetDisplayAdapter(DISPLAY_ADAPTER *disp_adapter);
