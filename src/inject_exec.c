#include "inject_exec.h"

#include "game/camera.h"
#include "game/lara/lara_state.h"
#include "game/math.h"
#include "game/matrix.h"
#include "game/music.h"
#include "game/shell.h"
#include "game/sound.h"
#include "inject_util.h"
#include "specific/s_audio_sample.h"
#include "specific/s_flagged_string.h"

static void Inject_Camera(void);
static void Inject_Math(void);
static void Inject_Matrix(void);
static void Inject_Shell(void);
static void Inject_Music(void);
static void Inject_Sound(void);
static void Inject_Lara_State(void);
static void Inject_S_Audio_Sample(void);
static void Inject_S_FlaggedString(void);

static void Inject_Camera(void)
{
    INJECT(1, 0x004105A0, Camera_Initialise);
    INJECT(1, 0x00410650, Camera_Move);
    INJECT(1, 0x004109D0, Camera_Clip);
    INJECT(1, 0x00410AB0, Camera_Shift);
    INJECT(1, 0x00410C10, Camera_GoodPosition);
    INJECT(1, 0x00410C60, Camera_SmartShift);
    INJECT(1, 0x004113F0, Camera_Chase);
    INJECT(1, 0x004114E0, Camera_ShiftClamp);
    INJECT(1, 0x00411680, Camera_Combat);
    INJECT(1, 0x00411810, Camera_Look);
    INJECT(1, 0x00411A00, Camera_Fixed);
    INJECT(1, 0x00411AA0, Camera_Update);
    INJECT(1, 0x004126A0, Camera_LoadCutsceneFrame);
    INJECT(1, 0x00412290, Camera_UpdateCutscene);
}

static void Inject_Matrix(void)
{
    INJECT(1, 0x00401000, Matrix_GenerateW2V);
    INJECT(1, 0x004011D0, Matrix_LookAt);
    INJECT(1, 0x004012D0, Matrix_RotX);
    INJECT(1, 0x00401380, Matrix_RotY);
    INJECT(1, 0x00401430, Matrix_RotZ);
    INJECT(1, 0x004014E0, Matrix_RotYXZ);
    INJECT(1, 0x004016C0, Matrix_RotYXZpack);
    INJECT(1, 0x004018B0, Matrix_TranslateRel);
}

static void Inject_Math(void)
{
    INJECT(1, 0x00457C10, Math_Atan);
    INJECT(1, 0x00457C58, Math_Cos);
    INJECT(1, 0x00457C5E, Math_Sin);
    INJECT(1, 0x00457C93, Math_Sqrt);
}

static void Inject_Shell(void)
{
    INJECT(1, 0x0044E890, Shell_ExitSystem);
}

static void Inject_Music(void)
{
    INJECT(1, 0x004553E0, Music_Init);
    INJECT(1, 0x00455460, Music_Shutdown);
    INJECT(1, 0x00455500, Music_Play);
    INJECT(1, 0x00455570, Music_Stop);
    INJECT(1, 0x004555B0, Music_PlaySynced);
    INJECT(1, 0x00455640, Music_GetFrames);
    INJECT(1, 0x004556B0, Music_SetVolume);
}

static void Inject_Sound(void)
{
    INJECT(1, 0x00455380, Sound_SetMasterVolume);
}

static void Inject_Lara_State(void)
{
    INJECT(1, 0x004278A0, Lara_State_Walk);
    INJECT(1, 0x00427930, Lara_State_Run);
    INJECT(1, 0x00427A80, Lara_State_Stop);
    INJECT(1, 0x00427BD0, Lara_State_ForwardJump);
    INJECT(1, 0x00427CB0, Lara_State_FastBack);
    INJECT(1, 0x00427D10, Lara_State_TurnRight);
    INJECT(1, 0x00427DA0, Lara_State_TurnLeft);
    INJECT(1, 0x00427E30, Lara_State_Death);
    INJECT(1, 0x00427E50, Lara_State_FastFall);
}

static void Inject_S_Audio_Sample(void)
{
    INJECT(1, 0x00447BC0, S_Audio_Sample_GetAdapter);
    INJECT(1, 0x00447C10, S_Audio_Sample_CloseAllTracks);
    INJECT(1, 0x00447C40, S_Audio_Sample_Load);
    INJECT(1, 0x00447D50, S_Audio_Sample_IsTrackPlaying);
    INJECT(1, 0x00447DA0, S_Audio_Sample_Play);
    INJECT(1, 0x00447E90, S_Audio_Sample_GetFreeTrackIndex);
    INJECT(1, 0x00447ED0, S_Audio_Sample_AdjustTrackVolumeAndPan);
    INJECT(1, 0x00447F00, S_Audio_Sample_AdjustTrackPitch);
    INJECT(1, 0x00447F40, S_Audio_Sample_CloseTrack);
    INJECT(1, 0x00447FB0, S_Audio_Sample_Init);
    INJECT(1, 0x00448050, S_Audio_Sample_DSoundEnumerate);
    INJECT(1, 0x00448070, S_Audio_Sample_DSoundEnumCallback);
    INJECT(1, 0x00448160, S_Audio_Sample_Init2);
    INJECT(1, 0x004482E0, S_Audio_Sample_DSoundCreate);
    INJECT(1, 0x00448300, S_Audio_Sample_DSoundBufferTest);
    INJECT(1, 0x004483D0, S_Audio_Sample_Shutdown);
    INJECT(1, 0x00448400, S_Audio_Sample_IsEnabled);
    INJECT(1, 0x00455220, S_Audio_Sample_OutPlay);
    INJECT(1, 0x00455270, S_Audio_Sample_CalculateSampleVolume);
    INJECT(1, 0x004552A0, S_Audio_Sample_CalculateSamplePan);
    INJECT(1, 0x004552D0, S_Audio_Sample_OutPlayLooped);
    INJECT(1, 0x00455320, S_Audio_Sample_OutSetPanAndVolume);
    INJECT(1, 0x00455360, S_Audio_Sample_OutSetPitch);
    INJECT(1, 0x00455390, S_Audio_Sample_OutCloseTrack);
    INJECT(1, 0x004553B0, S_Audio_Sample_OutCloseAllTracks);
    INJECT(1, 0x004553C0, S_Audio_Sample_OutIsTrackPlaying);
}

static void Inject_S_FlaggedString(void)
{
    INJECT(1, 0x00445F00, S_FlaggedString_Delete);
    INJECT(1, 0x00446100, S_FlaggedString_InitAdapter);
    INJECT(1, 0x00447550, S_FlaggedString_Create);
}

void Inject_Exec(void)
{
    Inject_Camera();
    Inject_Math();
    Inject_Matrix();
    Inject_Shell();
    Inject_Music();
    Inject_Sound();
    Inject_Lara_State();

    Inject_S_Audio_Sample();
    Inject_S_FlaggedString();
}
