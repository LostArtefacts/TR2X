#include "log.h"

#include <stdio.h>
#include <windows.h>

BOOL APIENTRY
DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        Log_Init();
        LOG_DEBUG("Injected DLL: DLL_PROCESS_ATTACH\n");
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        LOG_DEBUG("Injected DLL: DLL_PROCESS_DETACH\n");
        break;
    }
    return TRUE;
}
