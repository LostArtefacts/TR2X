#include "inject_exec.h"

#include "decomp/decomp.h"
#include "game/box.h"
#include "game/camera.h"
#include "game/creature.h"
#include "game/effects.h"
#include "game/input.h"
#include "game/inventory.h"
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
#include "game/objects/creatures/bird.h"
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

static void Inject_Decomp(const bool enable);

static void Inject_Camera(bool enable);
static void Inject_Math(bool enable);
static void Inject_Matrix(bool enable);
static void Inject_Shell(bool enable);
static void Inject_Text(bool enable);
static void Inject_Input(bool enable);
static void Inject_Output(bool enable);
static void Inject_Music(bool enable);
static void Inject_Sound(bool enable);

static void Inject_Overlay(bool enable);
static void Inject_Random(bool enable);
static void Inject_Items(bool enable);
static void Inject_Effects(bool enable);
static void Inject_LOS(bool enable);
static void Inject_Inventory(bool enable);
static void Inject_Lara_Look(bool enable);
static void Inject_Lara_Misc(bool enable);
static void Inject_Lara_State(bool enable);
static void Inject_Lara_Col(bool enable);

static void Inject_Creature(bool enable);
static void Inject_Box(bool enable);
static void Inject_Objects(bool enable);

static void Inject_S_Audio_Sample(bool enable);
static void Inject_S_Input(bool enable);
static void Inject_S_FlaggedString(bool enable);

static void Inject_Decomp(const bool enable)
{
    INJECT(enable, 0x00444D60, WinVidSetMinWindowSize);
    INJECT(enable, 0x00444DB0, WinVidClearMinWindowSize);
    INJECT(enable, 0x00444DC0, WinVidSetMaxWindowSize);
    INJECT(enable, 0x00444E10, WinVidClearMaxWindowSize);
    INJECT(enable, 0x00444E20, CalculateWindowWidth);
    INJECT(enable, 0x00444E70, CalculateWindowHeight);
    INJECT(enable, 0x00444EA0, WinVidGetMinMaxInfo);
    INJECT(enable, 0x00444FB0, WinVidFindGameWindow);
    INJECT(enable, 0x00444FD0, WinVidSpinMessageLoop);
    INJECT(enable, 0x004450C0, WinVidShowGameWindow);
    INJECT(enable, 0x00445110, WinVidHideGameWindow);
    INJECT(enable, 0x00445150, WinVidSetGameWindowSize);
    INJECT(enable, 0x00445190, ShowDDrawGameWindow);
    INJECT(enable, 0x00445240, HideDDrawGameWindow);
    INJECT(enable, 0x004452D0, DDrawSurfaceCreate);
    INJECT(enable, 0x00445320, DDrawSurfaceRestoreLost);
    INJECT(enable, 0x00445370, WinVidClearBuffer);
    INJECT(enable, 0x004453C0, WinVidBufferLock);
    INJECT(enable, 0x00445400, WinVidBufferUnlock);
    INJECT(enable, 0x00445430, WinVidCopyBitmapToBuffer);
    INJECT(enable, 0x004454C0, GetRenderBitDepth);
    INJECT(enable, 0x00445550, WinVidGetColorBitMasks);
    INJECT(enable, 0x004455D0, BitMaskGetNumberOfBits);
    INJECT(enable, 0x00445620, CalculateCompatibleColor);
    INJECT(enable, 0x00445690, WinVidGetDisplayMode);
    INJECT(enable, 0x00445720, WinVidGoFullScreen);
    INJECT(enable, 0x004457B0, WinVidGoWindowed);
    INJECT(enable, 0x004458C0, WinVidSetDisplayAdapter);
    INJECT(enable, 0x004471F0, DInputCreate);
    INJECT(enable, 0x00447220, DInputRelease);
    INJECT(enable, 0x00447240, WinInReadKeyboard);
    INJECT(enable, 0x004498C0, DecodeErrorMessage);
    INJECT(enable, 0x0044E4E0, RenderErrorBox);
    INJECT(enable, 0x0044E520, WinMain);
    INJECT(enable, 0x0044E700, GameInit);
    INJECT(enable, 0x0044E7A0, WinGameStart);
    INJECT(enable, 0x0044E820, Shell_Shutdown);
    INJECT(enable, 0x0044E8E0, ScreenshotPCX);
    INJECT(enable, 0x0044E9F0, CompPCX);
    INJECT(enable, 0x0044EAA0, EncodeLinePCX);
    INJECT(enable, 0x0044EB80, EncodePutPCX);
    INJECT(enable, 0x0044EBC0, Screenshot);
    INJECT(enable, 0x00454C50, TitleSequence);
    INJECT(enable, 0x00411F50, Game_SetCutsceneTrack);
    INJECT(enable, 0x00411F60, Game_Cutscene_Start);
    INJECT(enable, 0x00412080, Misc_InitCinematicRooms);
    INJECT(enable, 0x00412120, Game_Cutscene_Control);
    INJECT(enable, 0x004123D0, Room_FindByPos);
    INJECT(enable, 0x00412450, CutscenePlayer_Control);
    INJECT(enable, 0x00412530, Lara_Control_Cutscene);
    INJECT(enable, 0x004125D0, CutscenePlayer1_Initialise);
    INJECT(enable, 0x00412660, CutscenePlayerGen_Initialise);
}

static void Inject_Camera(const bool enable)
{
    INJECT(enable, 0x004105A0, Camera_Initialise);
    INJECT(enable, 0x00410650, Camera_Move);
    INJECT(enable, 0x004109D0, Camera_Clip);
    INJECT(enable, 0x00410AB0, Camera_Shift);
    INJECT(enable, 0x00410C10, Camera_GoodPosition);
    INJECT(enable, 0x00410C60, Camera_SmartShift);
    INJECT(enable, 0x004113F0, Camera_Chase);
    INJECT(enable, 0x004114E0, Camera_ShiftClamp);
    INJECT(enable, 0x00411680, Camera_Combat);
    INJECT(enable, 0x00411810, Camera_Look);
    INJECT(enable, 0x00411A00, Camera_Fixed);
    INJECT(enable, 0x00411AA0, Camera_Update);
    INJECT(enable, 0x004126A0, Camera_LoadCutsceneFrame);
    INJECT(enable, 0x00412290, Camera_UpdateCutscene);
}

static void Inject_Matrix(const bool enable)
{
    INJECT(enable, 0x00401000, Matrix_GenerateW2V);
    INJECT(enable, 0x004011D0, Matrix_LookAt);
    INJECT(enable, 0x004012D0, Matrix_RotX);
    INJECT(enable, 0x00401380, Matrix_RotY);
    INJECT(enable, 0x00401430, Matrix_RotZ);
    INJECT(enable, 0x004014E0, Matrix_RotYXZ);
    INJECT(enable, 0x004016C0, Matrix_RotYXZpack);
    INJECT(enable, 0x004018B0, Matrix_TranslateRel);
    INJECT(enable, 0x00401960, Matrix_TranslateAbs);
    INJECT(enable, 0x0041B710, Matrix_InitInterpolate);
    INJECT(enable, 0x0041B750, Matrix_Pop_I);
    INJECT(enable, 0x0041B780, Matrix_Push_I);
    INJECT(enable, 0x0041B7B0, Matrix_RotY_I);
    INJECT(enable, 0x0041B7F0, Matrix_RotX_I);
    INJECT(enable, 0x0041B830, Matrix_RotZ_I);
    INJECT(enable, 0x0041B870, Matrix_TranslateRel_I);
    INJECT(enable, 0x0041B8C0, Matrix_TranslateRel_ID);
    INJECT(enable, 0x0041B910, Matrix_RotYXZ_I);
    INJECT(enable, 0x0041B960, Matrix_RotYXZsuperpack_I);
    INJECT(enable, 0x0041B9A0, Matrix_RotYXZsuperpack);
    INJECT(enable, 0x0041BA80, Matrix_Interpolate);
    INJECT(enable, 0x0041BC30, Matrix_InterpolateArm);
    INJECT(enable, 0x00457280, Matrix_Push);
    INJECT(enable, 0x0045729E, Matrix_PushUnit);
}

static void Inject_Math(const bool enable)
{
    INJECT(enable, 0x00457C10, Math_Atan);
    INJECT(enable, 0x00457C58, Math_Cos);
    INJECT(enable, 0x00457C5E, Math_Sin);
    INJECT(enable, 0x00457C93, Math_Sqrt);
    INJECT(enable, 0x00401250, Math_GetVectorAngles);
}

static void Inject_Shell(const bool enable)
{
    INJECT(enable, 0x0044E770, Shell_Cleanup);
    INJECT(enable, 0x0044E890, Shell_ExitSystem);
    INJECT(enable, 0x00454980, Shell_Main);
}

static void Inject_Text(const bool enable)
{
    INJECT(enable, 0x00440450, Text_Init);
    INJECT(enable, 0x00440480, Text_Create);
    INJECT(enable, 0x00440590, Text_ChangeText);
    INJECT(enable, 0x004405D0, Text_SetScale);
    INJECT(enable, 0x004405F0, Text_Flash);
    INJECT(enable, 0x00440620, Text_AddBackground);
    INJECT(enable, 0x004406B0, Text_RemoveBackground);
    INJECT(enable, 0x004406C0, Text_AddOutline);
    INJECT(enable, 0x004406F0, Text_RemoveOutline);
    INJECT(enable, 0x00440700, Text_CentreH);
    INJECT(enable, 0x00440720, Text_CentreV);
    INJECT(enable, 0x00440740, Text_AlignRight);
    INJECT(enable, 0x00440760, Text_AlignBottom);
    INJECT(enable, 0x00440780, Text_GetWidth);
    INJECT(enable, 0x00440890, Text_Remove);
    INJECT(enable, 0x004408F0, Text_Draw);
    INJECT(enable, 0x00440920, Text_DrawBorder);
    INJECT(enable, 0x00440AB0, Text_DrawText);
    INJECT(enable, 0x00440E90, Text_GetScaleH);
    INJECT(enable, 0x00440ED0, Text_GetScaleV);
}

static void Inject_Input(const bool enable)
{
    INJECT(enable, 0x0044DA10, Input_Update);
}

static void Inject_Output(const bool enable)
{
    INJECT(enable, 0x004019E0, Output_InsertPolygons);
    INJECT(enable, 0x00401AE0, Output_InsertRoom);
    INJECT(enable, 0x00401BD0, Output_CalcSkyboxLight);
    INJECT(enable, 0x00401C10, Output_InsertSkybox);
    INJECT(enable, 0x00401D60, Output_CalcObjectVertices);
    INJECT(enable, 0x00401F40, Output_CalcVerticeLight);
    INJECT(enable, 0x004020B0, Output_CalcRoomVertices);
    INJECT(enable, 0x00402330, Output_RotateLight);
    INJECT(enable, 0x00402400, Output_InitPolyList);
    INJECT(enable, 0x00402430, Output_SortPolyList);
    INJECT(enable, 0x00402470, Output_QuickSort);
    INJECT(enable, 0x00402540, Output_PrintPolyList);
    INJECT(enable, 0x00402580, Output_AlterFOV);
    INJECT(enable, 0x00402690, Output_SetNearZ);
    INJECT(enable, 0x004026E0, Output_SetFarZ);
    INJECT(enable, 0x00402700, Output_Init);
    INJECT(enable, 0x00402970, Output_DrawPolyLine);
    INJECT(enable, 0x00402B10, Output_DrawPolyFlat);
    INJECT(enable, 0x00402B50, Output_DrawPolyTrans);
    INJECT(enable, 0x00402B90, Output_DrawPolyGouraud);
    INJECT(enable, 0x00402BD0, Output_DrawPolyGTMap);
    INJECT(enable, 0x00402C10, Output_DrawPolyWGTMap);
    INJECT(enable, 0x00402C50, Output_XGenX);
    INJECT(enable, 0x00402D30, Output_XGenXG);
    INJECT(enable, 0x00402E80, Output_XGenXGUV);
    INJECT(enable, 0x004030A0, Output_XGenXGUVPerspFP);
    INJECT(enable, 0x00403330, Output_GTMapPersp32FP);
    INJECT(enable, 0x00404300, Output_WGTMapPersp32FP);
    INJECT(enable, 0x004057D0, Output_DrawPolyGTMapPersp);
    INJECT(enable, 0x00405810, Output_DrawPolyWGTMapPersp);
    INJECT(enable, 0x00405850, Output_VisibleZClip);
    INJECT(enable, 0x004058C0, Output_ZedClipper);
    INJECT(enable, 0x00405A00, Output_XYGUVClipper);
    INJECT(enable, 0x00405F20, Output_InsertObjectGT4);
    INJECT(enable, 0x00406980, Output_InsertObjectGT3);
    INJECT(enable, 0x00407200, Output_XYGClipper);
    INJECT(enable, 0x00407630, Output_InsertObjectG4);
    INJECT(enable, 0x00407A10, Output_InsertObjectG3);
    INJECT(enable, 0x00407D30, Output_XYClipper);
    INJECT(enable, 0x00408000, Output_InsertTrans8);
    INJECT(enable, 0x004084B0, Output_InsertTransQuad);
    INJECT(enable, 0x00408590, Output_InsertFlatRect);
    INJECT(enable, 0x00408660, Output_InsertLine);
    INJECT(enable, 0x00408720, Output_InsertGT3_ZBuffered);
    INJECT(enable, 0x00408D70, Output_DrawClippedPoly_Textured);
    INJECT(enable, 0x00408EB0, Output_InsertGT4_ZBuffered);
    INJECT(enable, 0x00409300, Output_InsertObjectGT4_ZBuffered);
    INJECT(enable, 0x004093A0, Output_InsertObjectGT3_ZBuffered);
    INJECT(enable, 0x00409450, Output_InsertObjectG4_ZBuffered);
    INJECT(enable, 0x004097F0, Output_DrawPoly_Gouraud);
    INJECT(enable, 0x004098F0, Output_InsertObjectG3_ZBuffered);
    INJECT(enable, 0x00409BD0, Output_InsertFlatRect_ZBuffered);
    INJECT(enable, 0x00409DA0, Output_InsertLine_ZBuffered);
    INJECT(enable, 0x00409EE0, Output_InsertGT3_Sorted);
    INJECT(enable, 0x0040A5F0, Output_InsertClippedPoly_Textured);
    INJECT(enable, 0x0040A7A0, Output_InsertGT4_Sorted);
    INJECT(enable, 0x0040AC80, Output_InsertObjectGT4_Sorted);
    INJECT(enable, 0x0040AD10, Output_InsertObjectGT3_Sorted);
    INJECT(enable, 0x0040ADB0, Output_InsertObjectG4_Sorted);
    INJECT(enable, 0x0040B1F0, Output_InsertPoly_Gouraud);
    INJECT(enable, 0x0040B370, Output_InsertObjectG3_Sorted);
    INJECT(enable, 0x0040B6C0, Output_InsertSprite_Sorted);
    INJECT(enable, 0x0040BA10, Output_InsertFlatRect_Sorted);
    INJECT(enable, 0x0040BB90, Output_InsertLine_Sorted);
    INJECT(enable, 0x0040BCC0, Output_InsertTrans8_Sorted);
    INJECT(enable, 0x0040BE60, Output_InsertTransQuad_Sorted);
    INJECT(enable, 0x0040BFA0, Output_InsertSprite);
    INJECT(enable, 0x0040C050, Output_DrawSprite);
    INJECT(enable, 0x0040C320, Output_DrawPickup);
    INJECT(enable, 0x0040C3B0, Output_InsertRoomSprite);
    INJECT(enable, 0x0040C510, Output_DrawScreenSprite2D);
    INJECT(enable, 0x0040C5B0, Output_DrawScreenSprite);
    INJECT(enable, 0x0040C650, Output_DrawScaledSpriteC);
    INJECT(enable, 0x0041BA50, Output_InsertPolygons_I);
}

static void Inject_Music(const bool enable)
{
    INJECT(enable, 0x004553E0, Music_Init);
    INJECT(enable, 0x00455460, Music_Shutdown);
    INJECT(enable, 0x00455500, Music_Play);
    INJECT(enable, 0x00455570, Music_Stop);
    INJECT(enable, 0x004555B0, Music_PlaySynced);
    INJECT(enable, 0x00455640, Music_GetFrames);
    INJECT(enable, 0x004556B0, Music_SetVolume);
}

static void Inject_Sound(const bool enable)
{
    INJECT(enable, 0x00455380, Sound_SetMasterVolume);
}

static void Inject_Overlay(const bool enable)
{
    INJECT(enable, 0x004219A0, Overlay_FlashCounter);
    INJECT(enable, 0x004219D0, Overlay_DrawAssaultTimer);
    INJECT(enable, 0x00421B20, Overlay_DrawGameInfo);
    INJECT(enable, 0x00421B70, Overlay_DrawHealthBar);
    INJECT(enable, 0x00421C20, Overlay_DrawAirBar);
    INJECT(enable, 0x00421CC0, Overlay_MakeAmmoString);
    INJECT(enable, 0x00421CF0, Overlay_DrawAmmoInfo);
    INJECT(enable, 0x00421E40, Overlay_InitialisePickUpDisplay);
    INJECT(enable, 0x00421E60, Overlay_DrawPickups);
    INJECT(enable, 0x00421F60, Overlay_AddDisplayPickup);
    INJECT(enable, 0x00421FD0, Overlay_DisplayModeInfo);
    INJECT(enable, 0x00422050, Overlay_DrawModeInfo);
}

static void Inject_Random(const bool enable)
{
    INJECT(enable, 0x0044C970, Random_GetControl);
    INJECT(enable, 0x0044C990, Random_SeedControl);
    INJECT(enable, 0x0044C9A0, Random_GetDraw);
    INJECT(enable, 0x0044C9C0, Random_SeedDraw);
    INJECT(enable, 0x0044D870, Random_Seed);
}

static void Inject_Items(const bool enable)
{
    INJECT(enable, 0x00426CF0, Item_InitialiseArray);
    INJECT(enable, 0x00426D50, Item_Kill);
    INJECT(enable, 0x00426E70, Item_Create);
    INJECT(enable, 0x00426EB0, Item_Initialise);
    INJECT(enable, 0x00427070, Item_RemoveActive);
    INJECT(enable, 0x00427100, Item_RemoveDrawn);
    INJECT(enable, 0x00427170, Item_AddActive);
    INJECT(enable, 0x004271D0, Item_NewRoom);
    INJECT(enable, 0x00427270, Item_GlobalReplace);
    INJECT(enable, 0x00427520, Item_ClearKilled);
}

static void Inject_Effects(const bool enable)
{
    INJECT(enable, 0x004272F0, Effect_InitialiseArray);
    INJECT(enable, 0x00427320, Effect_Create);
    INJECT(enable, 0x00427390, Effect_Kill);
    INJECT(enable, 0x00427480, Effect_NewRoom);
}

static void Inject_LOS(const bool enable)
{
    INJECT(enable, 0x00415BE0, LOS_Check);
    INJECT(enable, 0x00415C80, LOS_CheckZ);
    INJECT(enable, 0x00415F70, LOS_CheckX);
    INJECT(enable, 0x00416260, LOS_ClipTarget);
    INJECT(enable, 0x00416340, LOS_CheckSmashable);
}

static void Inject_Inventory(const bool enable)
{
    INJECT(enable, 0x00422080, Inv_Display);
    INJECT(enable, 0x00423310, Inv_Construct);
    INJECT(enable, 0x00423470, Inv_SelectMeshes);
    INJECT(enable, 0x00423500, Inv_AnimateInventoryItem);
    INJECT(enable, 0x00423590, Inv_DrawInventoryItem);
}

static void Inject_Lara_Control(const bool enable)
{
    INJECT(enable, 0x00427580, Lara_HandleAboveWater);
    INJECT(enable, 0x00431670, Lara_HandleSurface);
    INJECT(enable, 0x00431F50, Lara_HandleUnderwater);
}

static void Inject_Lara_Look(const bool enable)
{
    INJECT(enable, 0x00427720, Lara_LookUpDown);
    INJECT(enable, 0x00427790, Lara_LookLeftRight);
    INJECT(enable, 0x00427810, Lara_ResetLook);
}

static void Inject_Lara_Misc(const bool enable)
{
    INJECT(enable, 0x0042A0A0, Lara_GetCollisionInfo);
    INJECT(enable, 0x0042A0E0, Lara_SlideSlope);
    INJECT(enable, 0x0042A1D0, Lara_HitCeiling);
    INJECT(enable, 0x0042A240, Lara_DeflectEdge);
    INJECT(enable, 0x0042A2C0, Lara_DeflectEdgeJump);
    INJECT(enable, 0x0042A440, Lara_SlideEdgeJump);
    INJECT(enable, 0x0042A530, Lara_TestWall);
    INJECT(enable, 0x0042A640, Lara_TestHangOnClimbWall);
    INJECT(enable, 0x0042A750, Lara_TestClimbStance);
    INJECT(enable, 0x0042A810, Lara_HangTest);
    INJECT(enable, 0x0042AB70, Lara_TestEdgeCatch);
    INJECT(enable, 0x0042AC20, Lara_TestHangJumpUp);
    INJECT(enable, 0x0042AD90, Lara_TestHangJump);
    INJECT(enable, 0x0042AF30, Lara_TestHangSwingIn);
    INJECT(enable, 0x0042AFF0, Lara_TestVault);
    INJECT(enable, 0x0042B2E0, Lara_TestSlide);
    INJECT(enable, 0x0042B410, Lara_FloorFront);
    INJECT(enable, 0x0042B490, Lara_LandedBad);
    INJECT(enable, 0x0042DF70, Lara_CheckForLetGo);
    INJECT(enable, 0x0042B550, Lara_GetJointAbsPosition);
    INJECT(enable, 0x0042B8E0, Lara_GetJointAbsPosition_I);
}

static void Inject_Lara_State(const bool enable)
{
    INJECT(enable, 0x00432180, Lara_SwimTurn);
    INJECT(enable, 0x004278A0, Lara_State_Walk);
    INJECT(enable, 0x00427930, Lara_State_Run);
    INJECT(enable, 0x00427A80, Lara_State_Stop);
    INJECT(enable, 0x00427BD0, Lara_State_ForwardJump);
    INJECT(enable, 0x00427CB0, Lara_State_FastBack);
    INJECT(enable, 0x00427D10, Lara_State_TurnRight);
    INJECT(enable, 0x00427DA0, Lara_State_TurnLeft);
    INJECT(enable, 0x00427E30, Lara_State_Death);
    INJECT(enable, 0x00427E50, Lara_State_FastFall);
    INJECT(enable, 0x00427E90, Lara_State_Hang);
    INJECT(enable, 0x00427EF0, Lara_State_Reach);
    INJECT(enable, 0x00427F10, Lara_State_Splat);
    INJECT(enable, 0x00427F20, Lara_State_Compress);
    INJECT(enable, 0x00428030, Lara_State_Back);
    INJECT(enable, 0x004280C0, Lara_State_Null);
    INJECT(enable, 0x004280D0, Lara_State_FastTurn);
    INJECT(enable, 0x00428120, Lara_State_StepRight);
    INJECT(enable, 0x004281A0, Lara_State_StepLeft);
    INJECT(enable, 0x00428220, Lara_State_Slide);
    INJECT(enable, 0x00428250, Lara_State_BackJump);
    INJECT(enable, 0x004282A0, Lara_State_RightJump);
    INJECT(enable, 0x004282E0, Lara_State_LeftJump);
    INJECT(enable, 0x00428320, Lara_State_UpJump);
    INJECT(enable, 0x00428340, Lara_State_Fallback);
    INJECT(enable, 0x00428370, Lara_State_HangLeft);
    INJECT(enable, 0x004283B0, Lara_State_HangRight);
    INJECT(enable, 0x004283F0, Lara_State_SlideBack);
    INJECT(enable, 0x00428410, Lara_State_PushBlock);
    INJECT(enable, 0x00428440, Lara_State_PPReady);
    INJECT(enable, 0x00428470, Lara_State_Pickup);
    INJECT(enable, 0x004284A0, Lara_State_PickupFlare);
    INJECT(enable, 0x00428500, Lara_State_SwitchOn);
    INJECT(enable, 0x00428540, Lara_State_UseKey);
    INJECT(enable, 0x00428570, Lara_State_Special);
    INJECT(enable, 0x00428590, Lara_State_SwanDive);
    INJECT(enable, 0x004285C0, Lara_State_FastDive);
    INJECT(enable, 0x00428620, Lara_State_WaterOut);
    INJECT(enable, 0x00428640, Lara_State_Wade);
    INJECT(enable, 0x00428710, Lara_State_DeathSlide);
    INJECT(enable, 0x004287B0, Lara_State_Extra_Breath);
    INJECT(enable, 0x00428800, Lara_State_Extra_YetiKill);
    INJECT(enable, 0x00428850, Lara_State_Extra_SharkKill);
    INJECT(enable, 0x004288F0, Lara_State_Extra_Airlock);
    INJECT(enable, 0x00428910, Lara_State_Extra_GongBong);
    INJECT(enable, 0x00428930, Lara_State_Extra_DinoKill);
    INJECT(enable, 0x00428990, Lara_State_Extra_PullDagger);
    INJECT(enable, 0x00428A50, Lara_State_Extra_StartAnim);
    INJECT(enable, 0x00428AA0, Lara_State_Extra_StartHouse);
    INJECT(enable, 0x00428B50, Lara_State_Extra_FinalAnim);
    INJECT(enable, 0x0042D850, Lara_State_ClimbLeft);
    INJECT(enable, 0x0042D890, Lara_State_ClimbRight);
    INJECT(enable, 0x0042D8D0, Lara_State_ClimbStance);
    INJECT(enable, 0x0042D950, Lara_State_Climbing);
    INJECT(enable, 0x0042D970, Lara_State_ClimbEnd);
    INJECT(enable, 0x0042D990, Lara_State_ClimbDown);
    INJECT(enable, 0x004317D0, Lara_State_SurfSwim);
    INJECT(enable, 0x00431840, Lara_State_SurfBack);
    INJECT(enable, 0x004318A0, Lara_State_SurfLeft);
    INJECT(enable, 0x00431900, Lara_State_SurfRight);
    INJECT(enable, 0x00431960, Lara_State_SurfTread);
    INJECT(enable, 0x00432210, Lara_State_Swim);
    INJECT(enable, 0x00432280, Lara_State_Glide);
    INJECT(enable, 0x00432300, Lara_State_Tread);
    INJECT(enable, 0x00432390, Lara_State_Dive);
    INJECT(enable, 0x004323B0, Lara_State_UWDeath);
    INJECT(enable, 0x00432410, Lara_State_UWTwist);
}

static void Inject_Lara_Col(const bool enable)
{
    INJECT(enable, 0x00428C00, Lara_Fallen);
    INJECT(enable, 0x00428C60, Lara_CollideStop);
    INJECT(enable, 0x00431B40, Lara_SurfaceCollision);
    INJECT(enable, 0x00431C40, Lara_TestWaterStepOut);
    INJECT(enable, 0x00431D30, Lara_TestWaterClimbOut);

    INJECT(enable, 0x00428D20, Lara_Col_Walk);
    INJECT(enable, 0x00428EC0, Lara_Col_Run);
    INJECT(enable, 0x00429040, Lara_Col_Stop);
    INJECT(enable, 0x004290D0, Lara_Col_ForwardJump);
    INJECT(enable, 0x004291B0, Lara_Col_FastBack);
    INJECT(enable, 0x00429270, Lara_Col_TurnRight);
    INJECT(enable, 0x00429310, Lara_Col_TurnLeft);
    INJECT(enable, 0x00429330, Lara_Col_Death);
    INJECT(enable, 0x004293A0, Lara_Col_FastFall);
    INJECT(enable, 0x00429440, Lara_Col_Hang);
    INJECT(enable, 0x00429570, Lara_Col_Reach);
    INJECT(enable, 0x00429600, Lara_Col_Splat);
    INJECT(enable, 0x00429660, Lara_Col_Land);
    INJECT(enable, 0x00429680, Lara_Col_Compress);
    INJECT(enable, 0x00429720, Lara_Col_Back);
    INJECT(enable, 0x00429820, Lara_Col_StepRight);
    INJECT(enable, 0x004298E0, Lara_Col_StepLeft);
    INJECT(enable, 0x00429900, Lara_Col_Slide);
    INJECT(enable, 0x00429920, Lara_Col_BackJump);
    INJECT(enable, 0x00429950, Lara_Col_RightJump);
    INJECT(enable, 0x00429980, Lara_Col_LeftJump);
    INJECT(enable, 0x004299B0, Lara_Col_UpJump);
    INJECT(enable, 0x00429AD0, Lara_Col_Fallback);
    INJECT(enable, 0x00429B60, Lara_Col_HangLeft);
    INJECT(enable, 0x00429BA0, Lara_Col_HangRight);
    INJECT(enable, 0x00429BE0, Lara_Col_SlideBack);
    INJECT(enable, 0x00429C10, Lara_Col_Null);
    INJECT(enable, 0x00429C30, Lara_Col_Roll);
    INJECT(enable, 0x00429CC0, Lara_Col_Roll2);
    INJECT(enable, 0x00429D80, Lara_Col_SwanDive);
    INJECT(enable, 0x00429DF0, Lara_Col_FastDive);
    INJECT(enable, 0x00429E70, Lara_Col_Wade);
    INJECT(enable, 0x00429FE0, Lara_Col_Default);
    INJECT(enable, 0x0042A020, Lara_Col_Jumper);
    INJECT(enable, 0x0042D9B0, Lara_Col_ClimbLeft);
    INJECT(enable, 0x0042DA10, Lara_Col_ClimbRight);
    INJECT(enable, 0x0042DA70, Lara_Col_ClimbStance);
    INJECT(enable, 0x0042DC80, Lara_Col_Climbing);
    INJECT(enable, 0x0042DDD0, Lara_Col_ClimbDown);
    INJECT(enable, 0x00431A50, Lara_Col_SurfSwim);
    INJECT(enable, 0x00431A90, Lara_Col_SurfBack);
    INJECT(enable, 0x00431AC0, Lara_Col_SurfLeft);
    INJECT(enable, 0x00431AF0, Lara_Col_SurfRight);
    INJECT(enable, 0x00431B20, Lara_Col_SurfTread);
    INJECT(enable, 0x00432420, Lara_Col_Swim);
    INJECT(enable, 0x00432440, Lara_Col_UWDeath);
}

static void Inject_Creature(const bool enable)
{
    INJECT(enable, 0x0040E1B0, Creature_Initialise);
    INJECT(enable, 0x0040E1E0, Creature_Activate);
    INJECT(enable, 0x0040E230, Creature_AIInfo);
    INJECT(enable, 0x0040EA00, Creature_Mood);
    INJECT(enable, 0x0040F2D0, Creature_CheckBaddieOverlap);
    INJECT(enable, 0x0040F460, Creature_Die);
    INJECT(enable, 0x0040F520, Creature_Animate);
    INJECT(enable, 0x0040FDF0, Creature_Turn);
    INJECT(enable, 0x0040FED0, Creature_Tilt);
    INJECT(enable, 0x0040FF10, Creature_Head);
    INJECT(enable, 0x0040FF60, Creature_Neck);
    INJECT(enable, 0x0040FFB0, Creature_Float);
    INJECT(enable, 0x00410060, Creature_Underwater);
    INJECT(enable, 0x004100B0, Creature_Effect);
    INJECT(enable, 0x00410110, Creature_Vault);
    INJECT(enable, 0x00410250, Creature_Kill);
    INJECT(enable, 0x004103C0, Creature_GetBaddieTarget);
}

static void Inject_Box(const bool enable)
{
    INJECT(enable, 0x0040E490, Box_SearchLOT);
    INJECT(enable, 0x0040E690, Box_UpdateLOT);
    INJECT(enable, 0x0040E700, Box_TargetBox);
    INJECT(enable, 0x0040E7A0, Box_StalkBox);
    INJECT(enable, 0x0040E8A0, Box_EscapeBox);
    INJECT(enable, 0x0040E950, Box_ValidBox);
    INJECT(enable, 0x0040EE70, Box_CalculateTarget);
    INJECT(enable, 0x0040F3D0, Box_BadFloor);
}

static void Inject_Objects(const bool enable)
{
    INJECT(enable, 0x0040C880, Bird_Initialise);
    INJECT(enable, 0x0040C910, Bird_Control);
}

static void Inject_S_Audio_Sample(const bool enable)
{
    INJECT(enable, 0x00447BC0, S_Audio_Sample_GetAdapter);
    INJECT(enable, 0x00447C10, S_Audio_Sample_CloseAllTracks);
    INJECT(enable, 0x00447C40, S_Audio_Sample_Load);
    INJECT(enable, 0x00447D50, S_Audio_Sample_IsTrackPlaying);
    INJECT(enable, 0x00447DA0, S_Audio_Sample_Play);
    INJECT(enable, 0x00447E90, S_Audio_Sample_GetFreeTrackIndex);
    INJECT(enable, 0x00447ED0, S_Audio_Sample_AdjustTrackVolumeAndPan);
    INJECT(enable, 0x00447F00, S_Audio_Sample_AdjustTrackPitch);
    INJECT(enable, 0x00447F40, S_Audio_Sample_CloseTrack);
    INJECT(enable, 0x00447FB0, S_Audio_Sample_Init);
    INJECT(enable, 0x00448050, S_Audio_Sample_DSoundEnumerate);
    INJECT(enable, 0x00448070, S_Audio_Sample_DSoundEnumCallback);
    INJECT(enable, 0x00448160, S_Audio_Sample_Init2);
    INJECT(enable, 0x004482E0, S_Audio_Sample_DSoundCreate);
    INJECT(enable, 0x00448300, S_Audio_Sample_DSoundBufferTest);
    INJECT(enable, 0x004483D0, S_Audio_Sample_Shutdown);
    INJECT(enable, 0x00448400, S_Audio_Sample_IsEnabled);
    INJECT(enable, 0x00455220, S_Audio_Sample_OutPlay);
    INJECT(enable, 0x00455270, S_Audio_Sample_CalculateSampleVolume);
    INJECT(enable, 0x004552A0, S_Audio_Sample_CalculateSamplePan);
    INJECT(enable, 0x004552D0, S_Audio_Sample_OutPlayLooped);
    INJECT(enable, 0x00455320, S_Audio_Sample_OutSetPanAndVolume);
    INJECT(enable, 0x00455360, S_Audio_Sample_OutSetPitch);
    INJECT(enable, 0x00455390, S_Audio_Sample_OutCloseTrack);
    INJECT(enable, 0x004553B0, S_Audio_Sample_OutCloseAllTracks);
    INJECT(enable, 0x004553C0, S_Audio_Sample_OutIsTrackPlaying);
}

static void Inject_S_Input(const bool enable)
{
    INJECT(enable, 0x0044D8F0, S_Input_Key);
}

static void Inject_S_FlaggedString(const bool enable)
{
    INJECT(enable, 0x00445F00, S_FlaggedString_Delete);
    INJECT(enable, 0x00446100, S_FlaggedString_InitAdapter);
    INJECT(enable, 0x00447550, S_FlaggedString_Create);
}

void Inject_Exec(void)
{
    Inject_Decomp(true);

    Inject_Camera(true);
    Inject_Math(true);
    Inject_Matrix(true);
    Inject_Shell(true);
    Inject_Text(true);
    Inject_Input(true);
    Inject_Output(true);
    Inject_Music(true);
    Inject_Sound(true);

    Inject_Overlay(true);
    Inject_Random(true);
    Inject_Items(true);
    Inject_Effects(true);
    Inject_LOS(true);
    Inject_Inventory(true);
    Inject_Lara_Control(true);
    Inject_Lara_Look(true);
    Inject_Lara_Misc(true);
    Inject_Lara_State(true);
    Inject_Lara_Col(true);

    Inject_Creature(true);
    Inject_Box(true);
    Inject_Objects(true);

    Inject_S_Audio_Sample(true);
    Inject_S_Input(true);
    Inject_S_FlaggedString(true);
}
