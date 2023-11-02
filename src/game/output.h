#pragma once

#include "global/types.h"

#include <stdint.h>

void __cdecl Output_Init(
    int16_t x, int16_t y, int32_t width, int32_t height, int32_t near_z,
    int32_t far_z, int16_t view_angle, int32_t screen_width,
    int32_t screen_height);

void __cdecl Output_InsertPolygons(const int16_t *obj_ptr, int32_t clip);
void __cdecl Output_InsertRoom(const int16_t *obj_ptr, int32_t is_outside);
void __cdecl Output_InsertSkybox(const int16_t *obj_ptr);
const int16_t *__cdecl Output_CalcObjectVertices(const int16_t *obj_ptr);
const int16_t *__cdecl Output_CalcSkyboxLight(const int16_t *obj_ptr);
const int16_t *__cdecl Output_CalcVerticeLight(const int16_t *obj_ptr);
const int16_t *__cdecl Output_CalcRoomVertices(
    const int16_t *obj_ptr, int32_t far_clip);
void __cdecl Output_RotateLight(int16_t pitch, int16_t yaw);
void __cdecl Output_InitPolyList(void);
void __cdecl Output_SortPolyList(void);
void __cdecl Output_QuickSort(int32_t left, int32_t right);
void __cdecl Output_PrintPolyList(uint8_t *surface_ptr);
void __cdecl Output_SetNearZ(int32_t near_z);
void __cdecl Output_SetFarZ(int32_t far_z);
void __cdecl Output_AlterFOV(int16_t fov);

void __cdecl Output_DrawPolyLine(const int16_t *obj_ptr);
void __cdecl Output_DrawPolyFlat(const int16_t *obj_ptr);
void __cdecl Output_DrawPolyTrans(const int16_t *obj_ptr);
void __cdecl Output_DrawPolyGouraud(const int16_t *obj_ptr);
void __cdecl Output_DrawPolyGTMap(const int16_t *obj_ptr);
void __cdecl Output_DrawPolyWGTMap(const int16_t *obj_ptr);
void __cdecl Output_DrawPolyGTMapPersp(const int16_t *obj_ptr);
void __cdecl Output_DrawPolyWGTMapPersp(const int16_t *obj_ptr);

int32_t __cdecl Output_XGenX(const int16_t *obj_ptr);
int32_t __cdecl Output_XGenXG(const int16_t *obj_ptr);
int32_t __cdecl Output_XGenXGUV(const int16_t *obj_ptr);
int32_t __cdecl Output_XGenXGUVPerspFP(const int16_t *obj_ptr);
void __cdecl Output_GTMapPersp32FP(
    int32_t y1, int32_t y2, const uint8_t *tex_page);
void __cdecl Output_WGTMapPersp32FP(
    int32_t y1, int32_t y2, const uint8_t *tex_page);

int32_t __cdecl Output_VisibleZClip(
    const struct PHD_VBUF *vtx0, const struct PHD_VBUF *vtx1,
    const struct PHD_VBUF *vtx2);
int32_t __cdecl Output_ZedClipper(
    int32_t vtx_count, const struct POINT_INFO *pts, struct VERTEX_INFO *vtx);
int32_t __cdecl Output_XYClipper(int32_t vtx_count, struct VERTEX_INFO *vtx);
int32_t __cdecl Output_XYGClipper(int32_t vtx_count, struct VERTEX_INFO *vtx);
int32_t __cdecl Output_XYGUVClipper(int32_t vtx_count, struct VERTEX_INFO *vtx);

const int16_t *__cdecl Output_InsertObjectG3(
    const int16_t *obj_ptr, int32_t num, enum SORT_TYPE sort_type);
const int16_t *__cdecl Output_InsertObjectG4(
    const int16_t *obj_ptr, int32_t num, enum SORT_TYPE sort_type);
const int16_t *__cdecl Output_InsertObjectGT3(
    const int16_t *obj_ptr, int32_t num, enum SORT_TYPE sort_type);
const int16_t *__cdecl Output_InsertObjectGT4(
    const int16_t *obj_ptr, int32_t num, enum SORT_TYPE sort_type);
