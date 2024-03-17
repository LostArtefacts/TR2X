#include "decomp/decomp.h"

#include "game/music.h"
#include "global/funcs.h"
#include "specific/s_audio_sample.h"

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
