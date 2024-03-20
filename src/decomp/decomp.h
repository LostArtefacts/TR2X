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
bool __cdecl WinVidSpinMessageLoop(bool need_wait);
