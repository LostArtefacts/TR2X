#include "game/shell.h"

#include "global/funcs.h"
#include "global/vars.h"

void __cdecl Shell_ExitSystem(const char *message)
{
    GameBuf_Shutdown();
    strcpy(g_ErrorMessage, message);
    Shell_Shutdown();
    Shell_Cleanup();
    exit(1);
}
