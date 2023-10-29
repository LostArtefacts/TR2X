#include "game/output.h"

#include "global/funcs.h"
#include "global/vars.h"

void __cdecl Output_InsertPolygons(const int16_t *obj_ptr, int32_t clip)
{
    g_FltWinLeft = g_PhdWinMinX;
    g_FltWinTop = g_PhdWinMinY;
    g_FltWinRight = (g_PhdWinMaxX + g_PhdWinMinX + 1);
    g_FltWinBottom = (g_PhdWinMaxY + g_PhdWinMinY + 1);
    g_FltWinCenterX = (g_PhdWinMinX + g_PhdWinCenterX);
    g_FltWinCenterY = (g_PhdWinMinY + g_PhdWinCenterY);

    obj_ptr = Output_CalcObjectVertices(obj_ptr + 4);
    if (obj_ptr) {
        obj_ptr = Output_CalcVerticeLight(obj_ptr);
        obj_ptr = g_Output_DrawObjectGT4(obj_ptr + 1, *obj_ptr, ST_AVG_Z);
        obj_ptr = g_Output_DrawObjectGT3(obj_ptr + 1, *obj_ptr, ST_AVG_Z);
        obj_ptr = g_Output_DrawObjectG4(obj_ptr + 1, *obj_ptr, ST_AVG_Z);
        obj_ptr = g_Output_DrawObjectG3(obj_ptr + 1, *obj_ptr, ST_AVG_Z);
    }
}

void __cdecl Output_InsertRoom(const int16_t *obj_ptr, int32_t is_outside)
{
    g_FltWinLeft = (g_PhdWinMinX + g_PhdWinLeft);
    g_FltWinTop = (g_PhdWinMinY + g_PhdWinTop);
    g_FltWinRight = (g_PhdWinRight + g_PhdWinMinX + 1);
    g_FltWinBottom = (g_PhdWinBottom + g_PhdWinMinY + 1);
    g_FltWinCenterX = (g_PhdWinMinX + g_PhdWinCenterX);
    g_FltWinCenterY = (g_PhdWinMinY + g_PhdWinCenterY);
    obj_ptr = Output_CalcRoomVertices(obj_ptr, is_outside ? 0 : 16);
    obj_ptr = g_Output_DrawObjectGT4(obj_ptr + 1, *obj_ptr, ST_MAX_Z);
    obj_ptr = g_Output_DrawObjectGT3(obj_ptr + 1, *obj_ptr, ST_MAX_Z);
    Output_InsertRoomSprite(obj_ptr + 1, *obj_ptr);
}

void __cdecl Output_InsertSkybox(const int16_t *obj_ptr)
{
    g_FltWinLeft = (g_PhdWinMinX + g_PhdWinLeft);
    g_FltWinTop = (g_PhdWinMinY + g_PhdWinTop);
    g_FltWinRight = (g_PhdWinRight + g_PhdWinMinX + 1);
    g_FltWinBottom = (g_PhdWinBottom + g_PhdWinMinY + 1);
    g_FltWinCenterX = (g_PhdWinMinX + g_PhdWinCenterX);
    g_FltWinCenterY = (g_PhdWinMinY + g_PhdWinCenterY);

    obj_ptr = Output_CalcObjectVertices(obj_ptr + 4);
    if (obj_ptr) {
        if (g_SavedAppSettings.render_mode == RM_HARDWARE) {
            HWR_EnableZBuffer(0, 0);
        }

        obj_ptr = Output_CalcSkyboxLight(obj_ptr);
        obj_ptr = g_Output_DrawObjectGT4(obj_ptr + 1, *obj_ptr, ST_FAR_Z);
        obj_ptr = g_Output_DrawObjectGT3(obj_ptr + 1, *obj_ptr, ST_FAR_Z);
        obj_ptr = g_Output_DrawObjectG4(obj_ptr + 1, *obj_ptr, ST_FAR_Z);
        obj_ptr = g_Output_DrawObjectG3(obj_ptr + 1, *obj_ptr, ST_FAR_Z);

        if (g_SavedAppSettings.render_mode == RM_HARDWARE) {
            HWR_EnableZBuffer(1, 1);
        }
    }
}

const int16_t *__cdecl Output_CalcSkyboxLight(const int16_t *obj_ptr)
{
    int32_t count = *obj_ptr++;
    if (count > 0) {
        obj_ptr += 3 * count;
    } else if (count < 0) {
        count = -count;
        obj_ptr += count;
    }

    for (int i = 0; i < count; i++) {
        g_PhdVBuf[i].g = 0xFFF;
    }

    return obj_ptr;
}
