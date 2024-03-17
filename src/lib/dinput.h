#pragma once

#include <windows.h>

#define DirectInputCreate                                                      \
    ((HRESULT(__stdcall *)(                                                    \
        HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUT * lplpDirectInput,     \
        LPUNKNOWN punkOuter))0x00457CC0)
