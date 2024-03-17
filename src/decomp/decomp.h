#pragma once

#include <stdint.h>
#include <windows.h>

int32_t __cdecl GameInit(void);
int32_t __stdcall WinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,
    int32_t nShowCmd);
const char *__cdecl DecodeErrorMessage(int32_t error_code);
int32_t __cdecl RenderErrorBox(int32_t error_code);
