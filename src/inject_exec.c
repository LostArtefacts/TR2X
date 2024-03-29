#include "inject_exec.h"

#include "decomp/decomp.h"
#include "game/camera.h"
#include "game/effects.h"
#include "game/input.h"
#include "game/items.h"
#include "game/lara/lara_col.h"
#include "game/lara/lara_control.h"
#include "game/lara/lara_look.h"
#include "game/lara/lara_misc.h"
#include "game/lara/lara_state.h"
#include "game/los.h"
#include "game/math.h"
#include "game/math_misc.h"
#include "game/matrix.h"
#include "game/music.h"
#include "game/output.h"
#include "game/overlay.h"
#include "game/random.h"
#include "game/shell.h"
#include "game/sound.h"
#include "game/text.h"
#include "inject_util.h"
#include "specific/s_audio_sample.h"
#include "specific/s_flagged_string.h"
#include "specific/s_input.h"

static void Inject_Decomp(void);

static void Inject_Camera(void);
static void Inject_Math(void);
static void Inject_Matrix(void);
static void Inject_Shell(void);
static void Inject_Text(void);
static void Inject_Input(void);
static void Inject_Output(void);
static void Inject_Music(void);
static void Inject_Sound(void);

static void Inject_Overlay(void);
static void Inject_Random(void);
static void Inject_Items(void);
static void Inject_Effects(void);
static void Inject_LOS(void);
static void Inject_Lara_Look(void);
static void Inject_Lara_Misc(void);
static void Inject_Lara_State(void);
static void Inject_Lara_Col(void);
static void Inject_S_Audio_Sample(void);
static void Inject_S_Input(void);
static void Inject_S_FlaggedString(void);

static void Inject_Decomp(void)
{
    INJECT(1, 0x00444D60, WinVidSetMinWindowSize);
    INJECT(1, 0x00444DB0, WinVidClearMinWindowSize);
    INJECT(1, 0x00444DC0, WinVidSetMaxWindowSize);
    INJECT(1, 0x00444E10, WinVidClearMaxWindowSize);
    INJECT(1, 0x00444E20, CalculateWindowWidth);
    INJECT(1, 0x00444E70, CalculateWindowHeight);
    INJECT(1, 0x00444EA0, WinVidGetMinMaxInfo);
    INJECT(1, 0x00444FB0, WinVidFindGameWindow);
    INJECT(1, 0x00444FD0, WinVidSpinMessageLoop);
    INJECT(1, 0x004450C0, WinVidShowGameWindow);
    INJECT(1, 0x00445110, WinVidHideGameWindow);
    INJECT(1, 0x00445150, WinVidSetGameWindowSize);
    INJECT(1, 0x00445190, ShowDDrawGameWindow);
    INJECT(1, 0x00445240, HideDDrawGameWindow);
    INJECT(1, 0x004452D0, DDrawSurfaceCreate);
    INJECT(1, 0x00445320, DDrawSurfaceRestoreLost);
    INJECT(1, 0x00445370, WinVidClearBuffer);
    INJECT(1, 0x004453C0, WinVidBufferLock);
    INJECT(1, 0x00445400, WinVidBufferUnlock);
    INJECT(1, 0x00445430, WinVidCopyBitmapToBuffer);
    INJECT(1, 0x004454C0, GetRenderBitDepth);
    INJECT(1, 0x00445550, WinVidGetColorBitMasks);
    INJECT(1, 0x004455D0, BitMaskGetNumberOfBits);
    INJECT(1, 0x00445620, CalculateCompatibleColor);
    INJECT(1, 0x00445690, WinVidGetDisplayMode);
    INJECT(1, 0x00445720, WinVidGoFullScreen);
    INJECT(1, 0x004457B0, WinVidGoWindowed);
    INJECT(1, 0x004458C0, WinVidSetDisplayAdapter);
    INJECT(1, 0x004471F0, DInputCreate);
    INJECT(1, 0x00447220, DInputRelease);
    INJECT(1, 0x00447240, WinInReadKeyboard);
    INJECT(1, 0x004498C0, DecodeErrorMessage);
    INJECT(1, 0x0044E4E0, RenderErrorBox);
    INJECT(1, 0x0044E520, WinMain);
    INJECT(1, 0x0044E700, GameInit);
    INJECT(1, 0x0044E7A0, WinGameStart);
    INJECT(1, 0x0044E820, Shell_Shutdown);
    INJECT(1, 0x0044E8E0, ScreenshotPCX);
    INJECT(1, 0x0044E9F0, CompPCX);
    INJECT(1, 0x0044EAA0, EncodeLinePCX);
    INJECT(1, 0x0044EB80, EncodePutPCX);
    INJECT(1, 0x0044EBC0, Screenshot);
    INJECT(1, 0x00454C50, TitleSequence);
}

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
    INJECT(1, 0x00401960, Matrix_TranslateAbs);
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
    INJECT(1, 0x00457280, Matrix_Push);
    INJECT(1, 0x0045729E, Matrix_PushUnit);
}

static void Inject_Math(void)
{
    INJECT(1, 0x00457C10, Math_Atan);
    INJECT(1, 0x00457C58, Math_Cos);
    INJECT(1, 0x00457C5E, Math_Sin);
    INJECT(1, 0x00457C93, Math_Sqrt);
    INJECT(1, 0x00401250, Math_GetVectorAngles);
}

static void Inject_Shell(void)
{
    INJECT(1, 0x0044E770, Shell_Cleanup);
    INJECT(1, 0x0044E890, Shell_ExitSystem);
    INJECT(1, 0x00454980, Shell_Main);
}

static void Inject_Text(void)
{
    INJECT(1, 0x00440450, Text_Init);
    INJECT(1, 0x00440480, Text_Create);
    INJECT(1, 0x00440590, Text_ChangeText);
    INJECT(1, 0x004405D0, Text_SetScale);
    INJECT(1, 0x004405F0, Text_Flash);
    INJECT(1, 0x00440620, Text_AddBackground);
    INJECT(1, 0x004406B0, Text_RemoveBackground);
    INJECT(1, 0x004406C0, Text_AddOutline);
    INJECT(1, 0x004406F0, Text_RemoveOutline);
    INJECT(1, 0x00440700, Text_CentreH);
    INJECT(1, 0x00440720, Text_CentreV);
    INJECT(1, 0x00440740, Text_AlignRight);
    INJECT(1, 0x00440760, Text_AlignBottom);
    INJECT(1, 0x00440780, Text_GetWidth);
    INJECT(1, 0x00440890, Text_Remove);
    INJECT(1, 0x004408F0, Text_Draw);
    INJECT(1, 0x00440920, Text_DrawBorder);
    INJECT(1, 0x00440AB0, Text_DrawText);
    INJECT(1, 0x00440E90, Text_GetScaleH);
    INJECT(1, 0x00440ED0, Text_GetScaleV);
}

static void Inject_Input(void)
{
    INJECT(1, 0x0044DA10, Input_Update);
}

static void Inject_Output(void)
{
    INJECT(1, 0x004019E0, Output_InsertPolygons);
    INJECT(1, 0x00401AE0, Output_InsertRoom);
    INJECT(1, 0x00401BD0, Output_CalcSkyboxLight);
    INJECT(1, 0x00401C10, Output_InsertSkybox);
    INJECT(1, 0x00401D60, Output_CalcObjectVertices);
    INJECT(1, 0x00401F40, Output_CalcVerticeLight);
    INJECT(1, 0x004020B0, Output_CalcRoomVertices);
    INJECT(1, 0x00402330, Output_RotateLight);
    INJECT(1, 0x00402400, Output_InitPolyList);
    INJECT(1, 0x00402430, Output_SortPolyList);
    INJECT(1, 0x00402470, Output_QuickSort);
    INJECT(1, 0x00402540, Output_PrintPolyList);
    INJECT(1, 0x00402580, Output_AlterFOV);
    INJECT(1, 0x00402690, Output_SetNearZ);
    INJECT(1, 0x004026E0, Output_SetFarZ);
    INJECT(1, 0x00402700, Output_Init);
    INJECT(1, 0x00402970, Output_DrawPolyLine);
    INJECT(1, 0x00402B10, Output_DrawPolyFlat);
    INJECT(1, 0x00402B50, Output_DrawPolyTrans);
    INJECT(1, 0x00402B90, Output_DrawPolyGouraud);
    INJECT(1, 0x00402BD0, Output_DrawPolyGTMap);
    INJECT(1, 0x00402C10, Output_DrawPolyWGTMap);
    INJECT(1, 0x00402C50, Output_XGenX);
    INJECT(1, 0x00402D30, Output_XGenXG);
    INJECT(1, 0x00402E80, Output_XGenXGUV);
    INJECT(1, 0x004030A0, Output_XGenXGUVPerspFP);
    INJECT(1, 0x00403330, Output_GTMapPersp32FP);
    INJECT(1, 0x00404300, Output_WGTMapPersp32FP);
    INJECT(1, 0x004057D0, Output_DrawPolyGTMapPersp);
    INJECT(1, 0x00405810, Output_DrawPolyWGTMapPersp);
    INJECT(1, 0x00405850, Output_VisibleZClip);
    INJECT(1, 0x004058C0, Output_ZedClipper);
    INJECT(1, 0x00405A00, Output_XYGUVClipper);
    INJECT(1, 0x00405F20, Output_InsertObjectGT4);
    INJECT(1, 0x00406980, Output_InsertObjectGT3);
    INJECT(1, 0x00407200, Output_XYGClipper);
    INJECT(1, 0x00407630, Output_InsertObjectG4);
    INJECT(1, 0x00407A10, Output_InsertObjectG3);
    INJECT(1, 0x00407D30, Output_XYClipper);
    INJECT(1, 0x00408000, Output_InsertTrans8);
    INJECT(1, 0x004084B0, Output_InsertTransQuad);
    INJECT(1, 0x00408590, Output_InsertFlatRect);
    INJECT(1, 0x00408660, Output_InsertLine);
    INJECT(1, 0x00408720, Output_InsertGT3_ZBuffered);
    INJECT(1, 0x00408D70, Output_DrawClippedPoly_Textured);
    INJECT(1, 0x00408EB0, Output_InsertGT4_ZBuffered);
    INJECT(1, 0x00409300, Output_InsertObjectGT4_ZBuffered);
    INJECT(1, 0x004093A0, Output_InsertObjectGT3_ZBuffered);
    INJECT(1, 0x00409450, Output_InsertObjectG4_ZBuffered);
    INJECT(1, 0x004097F0, Output_DrawPoly_Gouraud);
    INJECT(1, 0x004098F0, Output_InsertObjectG3_ZBuffered);
    INJECT(1, 0x00409BD0, Output_InsertFlatRect_ZBuffered);
    INJECT(1, 0x00409DA0, Output_InsertLine_ZBuffered);
    INJECT(1, 0x00409EE0, Output_InsertGT3_Sorted);
    INJECT(1, 0x0040A5F0, Output_InsertClippedPoly_Textured);
    INJECT(1, 0x0040A7A0, Output_InsertGT4_Sorted);
    INJECT(1, 0x0040AC80, Output_InsertObjectGT4_Sorted);
    INJECT(1, 0x0040AD10, Output_InsertObjectGT3_Sorted);
    INJECT(1, 0x0040ADB0, Output_InsertObjectG4_Sorted);
    INJECT(1, 0x0040B1F0, Output_InsertPoly_Gouraud);
    INJECT(1, 0x0040B370, Output_InsertObjectG3_Sorted);
    INJECT(1, 0x0040B6C0, Output_InsertSprite_Sorted);
    INJECT(1, 0x0040BA10, Output_InsertFlatRect_Sorted);
    INJECT(1, 0x0040BB90, Output_InsertLine_Sorted);
    INJECT(1, 0x0040BCC0, Output_InsertTrans8_Sorted);
    INJECT(1, 0x0040BE60, Output_InsertTransQuad_Sorted);
    INJECT(1, 0x0040BFA0, Output_InsertSprite);
    INJECT(1, 0x0040C050, Output_DrawSprite);
    INJECT(1, 0x0040C320, Output_DrawPickup);
    INJECT(1, 0x0040C3B0, Output_InsertRoomSprite);
    INJECT(1, 0x0040C510, Output_DrawScreenSprite2D);
    INJECT(1, 0x0040C5B0, Output_DrawScreenSprite);
    INJECT(1, 0x0040C650, Output_DrawScaledSpriteC);
    INJECT(1, 0x0041BA50, Output_InsertPolygons_I);
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

static void Inject_Overlay(void)
{
    INJECT(1, 0x004219A0, Overlay_FlashCounter);
    INJECT(1, 0x004219D0, Overlay_DrawAssaultTimer);
    INJECT(1, 0x00421B20, Overlay_DrawGameInfo);
    INJECT(1, 0x00421B70, Overlay_DrawHealthBar);
    INJECT(1, 0x00421C20, Overlay_DrawAirBar);
    INJECT(1, 0x00421CC0, Overlay_MakeAmmoString);
    INJECT(1, 0x00421CF0, Overlay_DrawAmmoInfo);
    INJECT(1, 0x00421E40, Overlay_InitialisePickUpDisplay);
    INJECT(1, 0x00421E60, Overlay_DrawPickups);
    INJECT(1, 0x00421F60, Overlay_AddDisplayPickup);
    INJECT(1, 0x00421FD0, Overlay_DisplayModeInfo);
    INJECT(1, 0x00422050, Overlay_DrawModeInfo);
}

static void Inject_Random(void)
{
    INJECT(1, 0x0044C970, Random_GetControl);
    INJECT(1, 0x0044C990, Random_SeedControl);
    INJECT(1, 0x0044C9A0, Random_GetDraw);
    INJECT(1, 0x0044C9C0, Random_SeedDraw);
    INJECT(1, 0x0044D870, Random_Seed);
}

static void Inject_Items(void)
{
    INJECT(1, 0x00426CF0, Item_InitialiseArray);
    INJECT(1, 0x00426D50, Item_Kill);
    INJECT(1, 0x00426E70, Item_Create);
    INJECT(1, 0x00426EB0, Item_Initialise);
    INJECT(1, 0x00427070, Item_RemoveActive);
    INJECT(1, 0x00427100, Item_RemoveDrawn);
    INJECT(1, 0x00427170, Item_AddActive);
    INJECT(1, 0x004271D0, Item_NewRoom);
    INJECT(1, 0x00427270, Item_GlobalReplace);
    INJECT(1, 0x00427520, Item_ClearKilled);
}

static void Inject_Effects(void)
{
    INJECT(1, 0x004272F0, Effect_InitialiseArray);
    INJECT(1, 0x00427320, Effect_Create);
    INJECT(1, 0x00427390, Effect_Kill);
    INJECT(1, 0x00427480, Effect_NewRoom);
}

static void Inject_LOS(void)
{
    INJECT(1, 0x00415BE0, LOS_Check);
    INJECT(1, 0x00415C80, LOS_CheckZ);
    INJECT(1, 0x00415F70, LOS_CheckX);
    INJECT(1, 0x00416260, LOS_ClipTarget);
    INJECT(1, 0x00416340, LOS_CheckSmashable);
}

static void Inject_Lara_Control(void)
{
    INJECT(1, 0x00427580, Lara_HandleAboveWater);
    INJECT(1, 0x00431670, Lara_HandleSurface);
    INJECT(1, 0x00431F50, Lara_HandleUnderwater);
}

static void Inject_Lara_Look(void)
{
    INJECT(1, 0x00427720, Lara_LookUpDown);
    INJECT(1, 0x00427790, Lara_LookLeftRight);
    INJECT(1, 0x00427810, Lara_ResetLook);
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
    INJECT(1, 0x00432180, Lara_SwimTurn);
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
    INJECT(1, 0x00428C00, Lara_Fallen);
    INJECT(1, 0x00428C60, Lara_CollideStop);
    INJECT(1, 0x00431B40, Lara_SurfaceCollision);
    INJECT(1, 0x00431C40, Lara_TestWaterStepOut);
    INJECT(1, 0x00431D30, Lara_TestWaterClimbOut);

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

static void Inject_S_Input(void)
{
    INJECT(1, 0x0044D8F0, S_Input_Key);
}

static void Inject_S_FlaggedString(void)
{
    INJECT(1, 0x00445F00, S_FlaggedString_Delete);
    INJECT(1, 0x00446100, S_FlaggedString_InitAdapter);
    INJECT(1, 0x00447550, S_FlaggedString_Create);
}

void Inject_Exec(void)
{
    Inject_Decomp();

    Inject_Camera();
    Inject_Math();
    Inject_Matrix();
    Inject_Shell();
    Inject_Text();
    Inject_Input();
    Inject_Output();
    Inject_Music();
    Inject_Sound();

    Inject_Overlay();
    Inject_Random();
    Inject_Items();
    Inject_Effects();
    Inject_LOS();
    Inject_Lara_Control();
    Inject_Lara_Look();
    Inject_Lara_Misc();
    Inject_Lara_State();
    Inject_Lara_Col();

    Inject_S_Audio_Sample();
    Inject_S_Input();
    Inject_S_FlaggedString();
}
