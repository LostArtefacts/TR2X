#include "inject_exec.h"

#include "game/camera.h"
#include "game/lara/lara_col.h"
#include "game/lara/lara_misc.h"
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
static void Inject_Lara_Misc(void);
static void Inject_Lara_State(void);
static void Inject_Lara_Col(void);
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
    INJECT(1, 0x0041B710, Matrix_InitInterpolate);
    INJECT(1, 0x0041B750, Matrix_Pop_I);
    INJECT(1, 0x0041B780, Matrix_Push_I);
    INJECT(1, 0x0041B7B0, Matrix_RotY_I);
    INJECT(1, 0x0041B7F0, Matrix_RotX_I);
    INJECT(1, 0x0041B830, Matrix_RotZ_I);
    INJECT(1, 0x0041B870, Matrix_TranslateRel_I);
    INJECT(1, 0x0041B8C0, Matrix_TranslateRel_ID);
    INJECT(1, 0x0041B910, Matrix_RotYXZ_I);
    INJECT(1, 0x0041B960, Matrix_RotYXZsuperpack_I);
    INJECT(1, 0x0041B9A0, Matrix_RotYXZsuperpack);
    INJECT(1, 0x0041BA80, Matrix_Interpolate);
    INJECT(1, 0x0041BC30, Matrix_InterpolateArm);
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

static void Inject_Lara_Misc(void)
{
    INJECT(1, 0x0042A0A0, Lara_GetCollisionInfo);
    INJECT(1, 0x0042A0E0, Lara_SlideSlope);
    INJECT(1, 0x0042A1D0, Lara_HitCeiling);
    INJECT(1, 0x0042A240, Lara_DeflectEdge);
    INJECT(1, 0x0042A2C0, Lara_DeflectEdgeJump);
    INJECT(1, 0x0042A440, Lara_SlideEdgeJump);
    INJECT(1, 0x0042A530, Lara_TestWall);
    INJECT(1, 0x0042A640, Lara_TestHangOnClimbWall);
    INJECT(1, 0x0042A750, Lara_TestClimbStance);
    INJECT(1, 0x0042A810, Lara_HangTest);
    INJECT(1, 0x0042AB70, Lara_TestEdgeCatch);
    INJECT(1, 0x0042AC20, Lara_TestHangJumpUp);
    INJECT(1, 0x0042AD90, Lara_TestHangJump);
    INJECT(1, 0x0042AF30, Lara_TestHangSwingIn);
    INJECT(1, 0x0042AFF0, Lara_TestVault);
    INJECT(1, 0x0042B2E0, Lara_TestSlide);
    INJECT(1, 0x0042B410, Lara_FloorFront);
    INJECT(1, 0x0042B490, Lara_LandedBad);
    INJECT(1, 0x0042DF70, Lara_CheckForLetGo);
    INJECT(1, 0x0042B550, Lara_GetJointAbsPosition);
    INJECT(1, 0x0042B8E0, Lara_GetJointAbsPosition_I);
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
    INJECT(1, 0x00427E90, Lara_State_Hang);
    INJECT(1, 0x00427EF0, Lara_State_Reach);
    INJECT(1, 0x00427F10, Lara_State_Splat);
    INJECT(1, 0x00427F20, Lara_State_Compress);
    INJECT(1, 0x00428030, Lara_State_Back);
    INJECT(1, 0x004280C0, Lara_State_Null);
    INJECT(1, 0x004280D0, Lara_State_FastTurn);
    INJECT(1, 0x00428120, Lara_State_StepRight);
    INJECT(1, 0x004281A0, Lara_State_StepLeft);
    INJECT(1, 0x00428220, Lara_State_Slide);
    INJECT(1, 0x00428250, Lara_State_BackJump);
    INJECT(1, 0x004282A0, Lara_State_RightJump);
    INJECT(1, 0x004282E0, Lara_State_LeftJump);
    INJECT(1, 0x00428320, Lara_State_UpJump);
    INJECT(1, 0x00428340, Lara_State_Fallback);
    INJECT(1, 0x00428370, Lara_State_HangLeft);
    INJECT(1, 0x004283B0, Lara_State_HangRight);
    INJECT(1, 0x004283F0, Lara_State_SlideBack);
    INJECT(1, 0x00428410, Lara_State_PushBlock);
    INJECT(1, 0x00428440, Lara_State_PPReady);
    INJECT(1, 0x00428470, Lara_State_Pickup);
    INJECT(1, 0x004284A0, Lara_State_PickupFlare);
    INJECT(1, 0x00428500, Lara_State_SwitchOn);
    INJECT(1, 0x00428540, Lara_State_UseKey);
    INJECT(1, 0x00428570, Lara_State_Special);
    INJECT(1, 0x00428590, Lara_State_SwanDive);
    INJECT(1, 0x004285C0, Lara_State_FastDive);
    INJECT(1, 0x00428620, Lara_State_WaterOut);
    INJECT(1, 0x00428640, Lara_State_Wade);
    INJECT(1, 0x00428710, Lara_State_DeathSlide);
    INJECT(1, 0x004287B0, Lara_State_Extra_Breath);
    INJECT(1, 0x00428800, Lara_State_Extra_YetiKill);
    INJECT(1, 0x00428850, Lara_State_Extra_SharkKill);
    INJECT(1, 0x004288F0, Lara_State_Extra_Airlock);
    INJECT(1, 0x00428910, Lara_State_Extra_GongBong);
    INJECT(1, 0x00428930, Lara_State_Extra_DinoKill);
    INJECT(1, 0x00428990, Lara_State_Extra_PullDagger);
    INJECT(1, 0x00428A50, Lara_State_Extra_StartAnim);
    INJECT(1, 0x00428AA0, Lara_State_Extra_StartHouse);
    INJECT(1, 0x00428B50, Lara_State_Extra_FinalAnim);
    INJECT(1, 0x0042D850, Lara_State_ClimbLeft);
    INJECT(1, 0x0042D890, Lara_State_ClimbRight);
    INJECT(1, 0x0042D8D0, Lara_State_ClimbStance);
    INJECT(1, 0x0042D950, Lara_State_Climbing);
    INJECT(1, 0x0042D970, Lara_State_ClimbEnd);
    INJECT(1, 0x0042D990, Lara_State_ClimbDown);
    INJECT(1, 0x004317D0, Lara_State_SurfSwim);
    INJECT(1, 0x00431840, Lara_State_SurfBack);
    INJECT(1, 0x004318A0, Lara_State_SurfLeft);
    INJECT(1, 0x00431900, Lara_State_SurfRight);
    INJECT(1, 0x00431960, Lara_State_SurfTread);
    INJECT(1, 0x00432210, Lara_State_Swim);
    INJECT(1, 0x00432280, Lara_State_Glide);
    INJECT(1, 0x00432300, Lara_State_Tread);
    INJECT(1, 0x00432390, Lara_State_Dive);
    INJECT(1, 0x004323B0, Lara_State_UWDeath);
    INJECT(1, 0x00432410, Lara_State_UWTwist);
}

static void Inject_Lara_Col(void)
{
    INJECT(1, 0x00428D20, Lara_Col_Walk);
    INJECT(1, 0x00428EC0, Lara_Col_Run);
    INJECT(1, 0x00429040, Lara_Col_Stop);
    INJECT(1, 0x004290D0, Lara_Col_ForwardJump);
    INJECT(1, 0x004291B0, Lara_Col_FastBack);
    INJECT(1, 0x00429270, Lara_Col_TurnRight);
    INJECT(1, 0x00429310, Lara_Col_TurnLeft);
    INJECT(1, 0x00429330, Lara_Col_Death);
    INJECT(1, 0x004293A0, Lara_Col_FastFall);
    INJECT(1, 0x00429440, Lara_Col_Hang);
    INJECT(1, 0x00429570, Lara_Col_Reach);
    INJECT(1, 0x00429600, Lara_Col_Splat);
    INJECT(1, 0x00429660, Lara_Col_Land);
    INJECT(1, 0x00429680, Lara_Col_Compress);
    INJECT(1, 0x00429720, Lara_Col_Back);
    INJECT(1, 0x00429820, Lara_Col_StepRight);
    INJECT(1, 0x004298E0, Lara_Col_StepLeft);
    INJECT(1, 0x00429900, Lara_Col_Slide);
    INJECT(1, 0x00429920, Lara_Col_BackJump);
    INJECT(1, 0x00429950, Lara_Col_RightJump);
    INJECT(1, 0x00429980, Lara_Col_LeftJump);
    INJECT(1, 0x004299B0, Lara_Col_UpJump);
    INJECT(1, 0x00429AD0, Lara_Col_Fallback);
    INJECT(1, 0x00429B60, Lara_Col_HangLeft);
    INJECT(1, 0x00429BA0, Lara_Col_HangRight);
    INJECT(1, 0x00429BE0, Lara_Col_SlideBack);
    INJECT(1, 0x00429C10, Lara_Col_Null);
    INJECT(1, 0x00429C30, Lara_Col_Roll);
    INJECT(1, 0x00429CC0, Lara_Col_Roll2);
    INJECT(1, 0x00429D80, Lara_Col_SwanDive);
    INJECT(1, 0x00429DF0, Lara_Col_FastDive);
    INJECT(1, 0x00429E70, Lara_Col_Wade);
    INJECT(1, 0x00429FE0, Lara_Col_Default);
    INJECT(1, 0x0042A020, Lara_Col_Jumper);
    INJECT(1, 0x0042D9B0, Lara_Col_ClimbLeft);
    INJECT(1, 0x0042DA10, Lara_Col_ClimbRight);
    INJECT(1, 0x0042DA70, Lara_Col_ClimbStance);
    INJECT(1, 0x0042DC80, Lara_Col_Climbing);
    INJECT(1, 0x0042DDD0, Lara_Col_ClimbDown);
    INJECT(1, 0x00431A50, Lara_Col_SurfSwim);
    INJECT(1, 0x00431A90, Lara_Col_SurfBack);
    INJECT(1, 0x00431AC0, Lara_Col_SurfLeft);
    INJECT(1, 0x00431AF0, Lara_Col_SurfRight);
    INJECT(1, 0x00431B20, Lara_Col_SurfTread);
    INJECT(1, 0x00432420, Lara_Col_Swim);
    INJECT(1, 0x00432440, Lara_Col_UWDeath);
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
    Inject_Lara_Misc();
    Inject_Lara_State();
    Inject_Lara_Col();

    Inject_S_Audio_Sample();
    Inject_S_FlaggedString();
}
