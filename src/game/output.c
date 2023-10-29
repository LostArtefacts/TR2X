#include "game/output.h"

#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

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

const int16_t *__cdecl Output_CalcObjectVertices(const int16_t *obj_ptr)
{
    const double base_z =
        g_SavedAppSettings.zbuffer ? 0.0 : (g_MidSort << (W2V_SHIFT + 8));
    uint8_t total_clip = 0xFF;

    obj_ptr++; // skip poly counter
    const int32_t vtx_count = *obj_ptr++;

    for (int i = 0; i < vtx_count; i++) {
        struct PHD_VBUF *const vbuf = &g_PhdVBuf[i];

        // clang-format off
        const struct MATRIX *const mptr = g_MatrixPtr;
        const double xv = (
            mptr->_00 * obj_ptr[0] +
            mptr->_01 * obj_ptr[1] +
            mptr->_02 * obj_ptr[2] +
            mptr->_03
        );
        const double yv = (
            mptr->_10 * obj_ptr[0] +
            mptr->_11 * obj_ptr[1] +
            mptr->_12 * obj_ptr[2] +
            mptr->_13
        );
        double zv = (
            mptr->_20 * obj_ptr[0] +
            mptr->_21 * obj_ptr[1] +
            mptr->_22 * obj_ptr[2] +
            mptr->_23
        );
        // clang-format on

        vbuf->xv = xv;
        vbuf->yv = yv;

        uint8_t clip_flags;
        if (zv < g_FltNearZ) {
            vbuf->zv = zv;
            clip_flags = 0x80;
        } else {

            if (zv >= g_FltFarZ) {
                zv = g_FltFarZ;
                vbuf->zv = zv;
            } else {
                vbuf->zv = zv + base_z;
            }

            const double persp = g_FltPersp / zv;
            vbuf->xs = persp * xv + g_FltWinCenterX;
            vbuf->ys = persp * yv + g_FltWinCenterY;
            vbuf->rhw = persp * g_FltRhwOPersp;

            clip_flags = 0x00;
            if (vbuf->xs < g_FltWinLeft) {
                clip_flags |= 1;
            } else if (vbuf->xs > g_FltWinRight) {
                clip_flags |= 2;
            }

            if (vbuf->ys < g_FltWinTop) {
                clip_flags |= 4;
            } else if (vbuf->ys > g_FltWinBottom) {
                clip_flags |= 8;
            }
        }

        vbuf->clip = clip_flags;
        total_clip &= clip_flags;
        obj_ptr += 3;
    }

    return total_clip == 0 ? obj_ptr : 0;
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

const int16_t *__cdecl Output_CalcVerticeLight(const int16_t *obj_ptr)
{
    int32_t vtx_count = *obj_ptr++;

    if (vtx_count > 0) {
        if (g_LsDivider) {
            // clang-format off
            int32_t xv = (
                g_LsVectorView.x * g_MatrixPtr->_00 +
                g_LsVectorView.y * g_MatrixPtr->_10 +
                g_LsVectorView.z * g_MatrixPtr->_20
            ) / g_LsDivider;
            int32_t yv = (
                g_LsVectorView.x * g_MatrixPtr->_01 +
                g_LsVectorView.y * g_MatrixPtr->_11 +
                g_LsVectorView.z * g_MatrixPtr->_21
            ) / g_LsDivider;
            int32_t zv = (
                g_LsVectorView.x * g_MatrixPtr->_02 +
                g_LsVectorView.y * g_MatrixPtr->_12 +
                g_LsVectorView.z * g_MatrixPtr->_22
            ) / g_LsDivider;
            // clang-format on

            for (int i = 0; i < vtx_count; i++) {
                int16_t shade = g_LsAdder
                    + ((xv * obj_ptr[0] + yv * obj_ptr[1] + zv * obj_ptr[2])
                       >> 16);
                CLAMP(shade, 0, 0x1FFF);

                g_PhdVBuf[i].g = shade;
                obj_ptr += 3;
            }
        } else {
            int16_t shade = g_LsAdder;
            CLAMP(shade, 0, 0x1FFF);
            obj_ptr += 3 * vtx_count;
            for (int i = 0; i < vtx_count; i++) {
                g_PhdVBuf[i].g = shade;
            }
        }
    } else if (vtx_count < 0) {
        for (int i = 0; i < -vtx_count; i++) {
            int16_t shade = g_LsAdder + *obj_ptr++;
            CLAMP(shade, 0, 0x1FFF);
            g_PhdVBuf[i].g = shade;
        }
    }

    return obj_ptr;
}
