#include "game/output.h"

#include "game/math.h"
#include "game/shell.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

static void __fastcall Output_FlatA(int32_t y1, int32_t y2, uint8_t color_idx)
{
    int32_t y_size = y2 - y1;
    if (y_size <= 0) {
        return;
    }

    const int32_t stride = g_PhdScreenWidth;
    const struct XBUF_X *xbuf = (const struct XBUF_X *)g_XBuffer + y1;
    uint8_t *draw_ptr = g_PrintSurfacePtr + y1 * stride;

    while (y_size > 0) {
        const int32_t x = xbuf->x1 / PHD_ONE;
        const int32_t x_size = (xbuf->x2 / PHD_ONE) - x;
        if (x_size > 0) {
            memset(draw_ptr + x, color_idx, x_size);
        }
        y_size--;
        xbuf++;
        draw_ptr += stride;
    }
}

static void __fastcall Output_TransA(int32_t y1, int32_t y2, uint8_t depth_q)
{
    int32_t y_size = y2 - y1;
    if (y_size <= 0 || depth_q >= 32) {
        return;
    }

    const int32_t stride = g_PhdScreenWidth;
    const struct XBUF_X *xbuf = (const struct XBUF_X *)g_XBuffer + y1;
    uint8_t *draw_ptr = g_PrintSurfacePtr + y1 * stride;
    const DEPTHQ_ENTRY *qt = g_DepthQTable + depth_q;

    while (y_size > 0) {
        const int32_t x = xbuf->x1 / PHD_ONE;
        int32_t x_size = (xbuf->x2 / PHD_ONE) - x;
        if (x_size > 0) {
            uint8_t *line_ptr = draw_ptr + x;
            while (x_size > 0) {
                *line_ptr = qt->index[*line_ptr];
                line_ptr++;
                x_size--;
            }
        }

        y_size--;
        xbuf++;
        draw_ptr += stride;
    }
}

static void __fastcall Output_GourA(int32_t y1, int32_t y2, uint8_t color_idx)
{
    int32_t y_size = y2 - y1;
    if (y_size <= 0) {
        return;
    }

    const int32_t stride = g_PhdScreenWidth;
    const struct XBUF_XG *xbuf = (const struct XBUF_XG *)g_XBuffer + y1;
    uint8_t *draw_ptr = g_PrintSurfacePtr + y1 * stride;
    const struct GOURAUD_ENTRY *gt = g_GouraudTable + color_idx;

    while (y_size > 0) {
        const int32_t x = xbuf->x1 / PHD_ONE;
        int32_t x_size = (xbuf->x2 / PHD_ONE) - x;
        if (x_size > 0) {
            int32_t g = xbuf->g1;
            const int32_t g_add = (xbuf->g2 - g) / x_size;

            uint8_t *line_ptr = draw_ptr + x;
            while (x_size > 0) {
                *line_ptr = gt->index[(g >> 16) & 0xFF];
                line_ptr++;
                g += g_add;
                x_size--;
            }
        }

        y_size--;
        xbuf++;
        draw_ptr += stride;
    }
}

static void __fastcall Output_GTMapA(int y1, int y2, const uint8_t *tex_page)
{
    int32_t y_size = y2 - y1;
    if (y_size <= 0) {
        return;
    }

    const int32_t stride = g_PhdScreenWidth;
    const struct XBUF_XGUV *xbuf = (const struct XBUF_XGUV *)g_XBuffer + y1;
    uint8_t *draw_ptr = g_PrintSurfacePtr + y1 * stride;

    while (y_size > 0) {
        const int32_t x = xbuf->x1 / PHD_ONE;
        int32_t x_size = (xbuf->x2 / PHD_ONE) - x;

        if (x_size > 0) {
            int32_t g = xbuf->g1;
            int32_t u = xbuf->u1;
            int32_t v = xbuf->v1;
            const int32_t g_add = (xbuf->g2 - g) / x_size;
            const int32_t u_add = (xbuf->u2 - u) / x_size;
            const int32_t v_add = (xbuf->v2 - v) / x_size;

            uint8_t *line_ptr = draw_ptr + x;
            while (x_size > 0) {
                const int32_t tex_idx =
                    ((((v >> 16) & 0xFF) << 8) | ((u >> 16) & 0xFF));
                uint8_t color_idx = tex_page[tex_idx];
                *line_ptr = g_DepthQTable[(g >> 16) & 0xFF].index[color_idx];
                line_ptr++;
                g += g_add;
                u += u_add;
                v += v_add;
                x_size--;
            }
        }

        y_size--;
        xbuf++;
        draw_ptr += stride;
    }
}

static void __fastcall Output_WGTMapA(
    int32_t y1, int32_t y2, const uint8_t *tex_page)
{
    int32_t y_size = y2 - y1;
    if (y_size <= 0) {
        return;
    }

    const int32_t stride = g_PhdScreenWidth;
    const struct XBUF_XGUV *xbuf = (const struct XBUF_XGUV *)g_XBuffer + y1;
    uint8_t *draw_ptr = g_PrintSurfacePtr + y1 * stride;

    while (y_size > 0) {
        const int32_t x = xbuf->x1 / PHD_ONE;
        int32_t x_size = (xbuf->x2 / PHD_ONE) - x;

        if (x_size > 0) {
            int32_t g = xbuf->g1;
            int32_t u = xbuf->u1;
            int32_t v = xbuf->v1;
            const int32_t g_add = (xbuf->g2 - g) / x_size;
            const int32_t u_add = (xbuf->u2 - u) / x_size;
            const int32_t v_add = (xbuf->v2 - v) / x_size;

            uint8_t *line_ptr = draw_ptr + x;
            while (x_size > 0) {
                const int32_t tex_idx =
                    ((((v >> 16) & 0xFF) << 8) | ((u >> 16) & 0xFF));
                uint8_t color_idx = tex_page[tex_idx];
                if (color_idx != 0) {
                    *line_ptr =
                        g_DepthQTable[(g >> 16) & 0xFF].index[color_idx];
                }
                line_ptr++;
                g += g_add;
                u += u_add;
                v += v_add;
                x_size--;
            }
        }

        y_size--;
        xbuf++;
        draw_ptr += stride;
    }
}

void __cdecl Output_Init(
    int16_t x, int16_t y, int32_t width, int32_t height, int32_t near_z,
    int32_t far_z, int16_t view_angle, int32_t screen_width,
    int32_t screen_height)
{
    g_PhdWinMinX = x;
    g_PhdWinMinY = y;
    g_PhdWinMaxX = width - 1;
    g_PhdWinMaxY = height - 1;

    g_PhdWinWidth = width;
    g_PhdWinHeight = height;

    g_PhdWinCenterX = width / 2;
    g_PhdWinCenterY = height / 2;
    g_FltWinCenterX = width / 2;
    g_FltWinCenterY = height / 2;

    g_PhdNearZ = near_z << 14;
    g_PhdFarZ = far_z << W2V_SHIFT;
    g_PhdViewDistance = far_z;

    g_PhdWinLeft = 0;
    g_PhdWinTop = 0;
    g_PhdWinRight = g_PhdWinMaxX;
    g_PhdWinBottom = g_PhdWinMaxY;

    g_PhdWinRect.left = g_PhdWinMinX;
    g_PhdWinRect.bottom = g_PhdWinMinY + g_PhdWinHeight;
    g_PhdWinRect.top = g_PhdWinMinY;
    g_PhdWinRect.right = g_PhdWinMinX + g_PhdWinWidth;

    g_PhdScreenWidth = screen_width;
    g_PhdScreenHeight = screen_height;

    Output_AlterFOV(182 * view_angle);
    Output_SetNearZ(g_PhdNearZ);
    Output_SetFarZ(g_PhdFarZ);

    g_MatrixPtr = g_MatrixStack;

    switch (g_SavedAppSettings.render_mode) {
    case RM_SOFTWARE:
        g_PerspectiveDistance = g_SavedAppSettings.perspective_correct
            ? SW_DETAIL_HIGH
            : SW_DETAIL_MEDIUM;
        g_Output_DrawObjectGT3 = Output_InsertObjectGT3;
        g_Output_DrawObjectGT4 = Output_InsertObjectGT4;
        g_Output_DrawObjectG3 = Output_InsertObjectG3;
        g_Output_DrawObjectG4 = Output_InsertObjectG4;
        g_Output_InsertSprite = Output_InsertSprite;
        g_Output_InsertFlatRect = Output_InsertFlatRect;
        g_Output_DrawLine = Output_InsertLine;
        g_Output_InsertTrans8 = Output_InsertTrans8;
        g_Output_InsertTransQuad = Output_InsertTransQuad;
        break;

    case RM_HARDWARE:
        if (g_SavedAppSettings.zbuffer) {
            g_Output_DrawObjectGT3 = Output_InsertObjectGT3_ZBuffered;
            g_Output_DrawObjectGT4 = Output_InsertObjectGT4_ZBuffered;
            g_Output_DrawObjectG3 = Output_InsertObjectG3_ZBuffered;
            g_Output_DrawObjectG4 = Output_InsertObjectG4_ZBuffered;
            g_Output_InsertFlatRect = Output_InsertFlatRect_ZBuffered;
            g_Output_DrawLine = Output_InsertLine_ZBuffered;
        } else {
            g_Output_DrawObjectGT3 = Output_InsertObjectGT3_Sorted;
            g_Output_DrawObjectGT4 = Output_InsertObjectGT4_Sorted;
            g_Output_DrawObjectG3 = Output_InsertObjectG3_Sorted;
            g_Output_DrawObjectG4 = Output_InsertObjectG4_Sorted;
            g_Output_InsertFlatRect = Output_InsertFlatRect_Sorted;
            g_Output_DrawLine = Output_InsertLine_Sorted;
        }
        g_Output_InsertSprite = Output_InsertSprite_Sorted;
        g_Output_InsertTrans8 = Output_InsertTrans8_Sorted;
        g_Output_InsertTransQuad = Output_InsertTransQuad_Sorted;
        break;

    default:
        Shell_ExitSystem("unknown render mode");
    }
}

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
            const struct MATRIX *const mptr = g_MatrixPtr;
            int32_t xv = (
                g_LsVectorView.x * mptr->_00 +
                g_LsVectorView.y * mptr->_10 +
                g_LsVectorView.z * mptr->_20
            ) / g_LsDivider;
            int32_t yv = (
                g_LsVectorView.x * mptr->_01 +
                g_LsVectorView.y * mptr->_11 +
                g_LsVectorView.z * mptr->_21
            ) / g_LsDivider;
            int32_t zv = (
                g_LsVectorView.x * mptr->_02 +
                g_LsVectorView.y * mptr->_12 +
                g_LsVectorView.z * mptr->_22
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

const int16_t *__cdecl Output_CalcRoomVertices(
    const int16_t *obj_ptr, int32_t far_clip)
{
    const double base_z =
        g_ZBufferSurface ? 0.0 : (g_MidSort << (W2V_SHIFT + 8));
    int32_t vtx_count = *obj_ptr++;

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
        const int32_t zv_int = (
            mptr->_20 * obj_ptr[0] +
            mptr->_21 * obj_ptr[1] +
            mptr->_22 * obj_ptr[2] +
            mptr->_23
        );
        const double zv = zv_int;
        // clang-format on

        vbuf->xv = xv;
        vbuf->yv = yv;
        vbuf->zv = zv;

        int16_t shade = obj_ptr[5];
        if (g_IsWaterEffect) {
            shade += g_ShadesTable
                [((uint8_t)g_WibbleOffset
                  + (uint8_t)g_RandomTable[(vtx_count - i) % WIBBLE_SIZE])
                 % WIBBLE_SIZE];
        }

        uint16_t clip_flags = 0;
        if (zv < g_FltNearZ) {
            clip_flags = 0xFF80;
        } else {
            const double persp = g_FltPersp / zv;
            const int32_t depth = zv_int >> W2V_SHIFT;
            vbuf->zv += base_z;

            if (depth < FOG_END) {
                if (depth > FOG_START) {
                    shade += depth - FOG_START;
                }
                vbuf->rhw = persp * g_FltRhwOPersp;
            } else {
                clip_flags = far_clip;
                shade = 0x1FFF;
                vbuf->zv = g_FltFarZ;
                vbuf->rhw = 0.0; // *(int32_t*)0x00478054
            }

            double xs = xv * persp + g_FltWinCenterX;
            double ys = yv * persp + g_FltWinCenterY;

            if (g_IsWibbleEffect && obj_ptr[4] >= 0) {
                xs += g_WibbleTable
                    [((uint8_t)g_WibbleOffset + (uint8_t)vbuf->ys)
                     % WIBBLE_SIZE];
                ys += g_WibbleTable
                    [((uint8_t)g_WibbleOffset + (uint8_t)vbuf->xs)
                     % WIBBLE_SIZE];
            }

            if (xs < g_FltWinLeft) {
                clip_flags |= 1;
            } else if (xs > g_FltWinRight) {
                clip_flags |= 2;
            }

            if (ys < g_FltWinTop) {
                clip_flags |= 4;
            } else if (ys > g_FltWinBottom) {
                clip_flags |= 8;
            }

            vbuf->xs = xs;
            vbuf->ys = ys;
            clip_flags |= (~((uint8_t)(vbuf->zv / 0x155555.p0))) << 8;
        }

        CLAMP(shade, 0, 0x1FFF);
        vbuf->g = shade;
        vbuf->clip = clip_flags;
        obj_ptr += 6;
    }

    return obj_ptr;
}

void __cdecl Output_RotateLight(int16_t pitch, int16_t yaw)
{
    g_LsYaw = yaw;
    g_LsPitch = pitch;

    int32_t xcos = Math_Cos(pitch);
    int32_t xsin = Math_Sin(pitch);
    int32_t wcos = Math_Cos(yaw);
    int32_t wsin = Math_Sin(yaw);

    int32_t x = (xcos * wsin) >> W2V_SHIFT;
    int32_t y = -xsin;
    int32_t z = (xcos * wcos) >> W2V_SHIFT;

    const struct MATRIX *const m = &g_W2VMatrix;
    g_LsVectorView.x = (m->_00 * x + m->_01 * y + m->_02 * z) >> W2V_SHIFT;
    g_LsVectorView.y = (m->_10 * x + m->_11 * y + m->_12 * z) >> W2V_SHIFT;
    g_LsVectorView.z = (m->_20 * x + m->_21 * y + m->_22 * z) >> W2V_SHIFT;
}

void __cdecl Output_InitPolyList(void)
{
    g_SurfaceCount = 0;
    g_Sort3dPtr = g_SortBuffer;
    g_Info3DPtr = g_Info3DBuffer;
    if (g_SavedAppSettings.render_mode != RM_SOFTWARE) {
        g_HWR_VertexPtr = g_HWR_VertexBuffer;
    }
}

void __cdecl Output_SortPolyList(void)
{
    if (g_SurfaceCount) {
        for (int i = 0; i < g_SurfaceCount; i++) {
            g_SortBuffer[i]._1 += i;
        }
        Output_QuickSort(0, g_SurfaceCount - 1);
    }
}

void __cdecl Output_QuickSort(int32_t left, int32_t right)
{
    int32_t compare = g_SortBuffer[(left + right) / 2]._1;
    int32_t i = left;
    int32_t j = right;

    do {
        while ((i < right) && (g_SortBuffer[i]._1 > compare)) {
            i++;
        }
        while ((left < j) && (compare > g_SortBuffer[j]._1)) {
            j--;
        }
        if (i > j) {
            break;
        }

        struct SORT_ITEM tmp_item;
        SWAP(g_SortBuffer[i], g_SortBuffer[j], tmp_item);

        i++;
        j--;
    } while (i <= j);

    if (left < j) {
        Output_QuickSort(left, j);
    }
    if (i < right) {
        Output_QuickSort(i, right);
    }
}

void __cdecl Output_PrintPolyList(uint8_t *surface_ptr)
{
    g_PrintSurfacePtr = surface_ptr;

    for (int i = 0; i < g_SurfaceCount; i++) {
        const int16_t *obj_ptr = (const int16_t *)g_SortBuffer[i]._0;
        const int16_t poly_type = *obj_ptr++;
        g_PolyDrawRoutines[poly_type](obj_ptr);
    }
}

void __cdecl Output_SetNearZ(int32_t near_z)
{
    g_PhdNearZ = near_z;
    g_FltNearZ = near_z;
    g_FltRhwONearZ = g_RhwFactor / g_FltNearZ;
    g_FltPerspONearZ = g_FltPersp / g_FltNearZ;

    double res_z = 0.99 * g_FltNearZ * g_FltFarZ / (g_FltFarZ - g_FltNearZ);
    g_FltResZ = res_z;
    g_FltResZORhw = res_z / g_RhwFactor;
    g_FltResZBuf = 0.005 + res_z / g_FltNearZ;
}

void __cdecl Output_SetFarZ(int32_t far_z)
{
    g_PhdFarZ = far_z;
    g_FltFarZ = far_z;

    double res_z = g_FltFarZ * g_FltNearZ * 0.99 / (g_FltFarZ - g_FltNearZ);
    g_FltResZ = res_z;
    g_FltResZORhw = res_z / g_RhwFactor;
    g_FltResZBuf = 0.005 - res_z / g_FltNearZ;
}

void __cdecl Output_AlterFOV(int16_t fov)
{
    fov /= 2;

    g_PhdPersp = g_PhdWinWidth / 2 * Math_Cos(fov) / Math_Sin(fov);

    g_FltPersp = g_PhdPersp;
    g_FltRhwOPersp = g_RhwFactor / g_FltPersp;
    g_FltPerspONearZ = g_FltPersp / g_FltNearZ;

    double window_aspect_ratio = 4.0 / 3.0;
    if (!g_SavedAppSettings.fullscreen
        && g_SavedAppSettings.aspect_mode == AM_16_9) {
        window_aspect_ratio = 16.0 / 9.0;
    }

    g_ViewportAspectRatio =
        window_aspect_ratio / ((double)g_PhdWinWidth / (double)g_PhdWinHeight);
}

void __cdecl Output_DrawPolyLine(const int16_t *obj_ptr)
{
    int32_t x1 = *obj_ptr++;
    int32_t y1 = *obj_ptr++;
    int32_t x2 = *obj_ptr++;
    int32_t y2 = *obj_ptr++;
    uint8_t lcolor = (uint8_t)*obj_ptr;

    if (x2 < x1) {
        int32_t tmp;
        SWAP(x1, x2, tmp);
        SWAP(y1, y2, tmp);
    }

    if (x2 < 0 || x1 > g_PhdWinMaxX) {
        return;
    }

    if (x1 < 0) {
        y1 -= x1 * (y2 - y1) / (x2 - x1);
        x1 = 0;
    }

    if (x2 > g_PhdWinMaxX) {
        y2 = y1 + (y2 - y1) * (g_PhdWinMaxX - x1) / (x2 - x1);
        x2 = g_PhdWinMaxX;
    }

    if (y2 < y1) {
        int32_t tmp;
        SWAP(x1, x2, tmp);
        SWAP(y1, y2, tmp);
    }

    if (y2 < 0 || y1 > g_PhdWinMaxY) {
        return;
    }

    if (y1 < 0) {
        x1 -= y1 * (x2 - x1) / (y2 - y1);
        y1 = 0;
    }

    if (y2 > g_PhdWinMaxY) {
        x2 = x1 + (x2 - x1) * (g_PhdWinMaxY - y1) / (y2 - y1);
        y2 = g_PhdWinMaxY;
    }

    int32_t x_size = x2 - x1;
    int32_t y_size = y2 - y1;
    uint8_t *draw_ptr = &g_PrintSurfacePtr[x1 + g_PhdScreenWidth * y1];

    if (!x_size && !y_size) {
        *draw_ptr = lcolor;
        return;
    }

    int32_t x_add = 0;
    if (x_size < 0) {
        x_add = -1;
        x_size = -x_size;
    } else {
        x_add = 1;
    }

    int32_t y_add;
    if (y_size < 0) {
        y_add = -g_PhdScreenWidth;
        y_size = -y_size;
    } else {
        y_add = g_PhdScreenWidth;
    }

    int32_t col_add;
    int32_t row_add;
    int32_t cols;
    int32_t rows;
    if (x_size >= y_size) {
        col_add = x_add;
        row_add = y_add;
        cols = x_size + 1;
        rows = y_size + 1;
    } else {
        col_add = y_add;
        row_add = x_add;
        cols = y_size + 1;
        rows = x_size + 1;
    }

    int32_t part_sum = 0;
    int32_t part = PHD_ONE * rows / cols;
    for (int i = 0; i < cols; i++) {
        part_sum += part;
        *draw_ptr = lcolor;
        draw_ptr += col_add;
        if (part_sum >= PHD_ONE) {
            draw_ptr += row_add;
            part_sum -= PHD_ONE;
        }
    }
}

void __cdecl Output_DrawPolyFlat(const int16_t *obj_ptr)
{
    if (Output_XGenX(obj_ptr + 1)) {
        Output_FlatA(g_XGenY1, g_XGenY2, *obj_ptr);
    }
}

void __cdecl Output_DrawPolyTrans(const int16_t *obj_ptr)
{
    if (Output_XGenX(obj_ptr + 1)) {
        Output_TransA(g_XGenY1, g_XGenY2, *obj_ptr);
    }
}

void __cdecl Output_DrawPolyGouraud(const int16_t *obj_ptr)
{
    if (Output_XGenXG(obj_ptr + 1)) {
        Output_GourA(g_XGenY1, g_XGenY2, *obj_ptr);
    }
}

void __cdecl Output_DrawPolyGTMap(const int16_t *obj_ptr)
{
    if (Output_XGenXGUV(obj_ptr + 1)) {
        Output_GTMapA(g_XGenY1, g_XGenY2, g_TexturePageBuffer8[*obj_ptr]);
    }
}

void __cdecl Output_DrawPolyWGTMap(const int16_t *obj_ptr)
{
    if (Output_XGenXGUV(obj_ptr + 1)) {
        Output_WGTMapA(g_XGenY1, g_XGenY2, g_TexturePageBuffer8[*obj_ptr]);
    }
}

int32_t __cdecl Output_XGenX(const int16_t *obj_ptr)
{
    int32_t pt_count = *obj_ptr++;
    const struct XGEN_X *pt2 = (const struct XGEN_X *)obj_ptr;
    const struct XGEN_X *pt1 = pt2 + (pt_count - 1);

    int32_t y_min = pt1->y;
    int32_t y_max = pt1->y;

    while (pt_count--) {
        const int32_t x1 = pt1->x;
        const int32_t y1 = pt1->y;
        const int32_t x2 = pt2->x;
        const int32_t y2 = pt2->y;
        pt1 = pt2++;

        if (y1 < y2) {
            CLAMPG(y_min, y1);
            const int32_t x_size = x2 - x1;
            int32_t y_size = y2 - y1;

            struct XBUF_X *x_ptr = (struct XBUF_X *)g_XBuffer + y1;
            const int32_t x_add = PHD_ONE * x_size / y_size;
            int32_t x = x1 * PHD_ONE + (PHD_ONE - 1);

            while (y_size--) {
                x += x_add;
                x_ptr->x2 = x;
                x_ptr++;
            }
        } else if (y2 < y1) {
            CLAMPL(y_max, y1);
            const int32_t x_size = x1 - x2;
            int32_t y_size = y1 - y2;

            struct XBUF_X *x_ptr = (struct XBUF_X *)g_XBuffer + y2;
            const int32_t x_add = PHD_ONE * x_size / y_size;
            int32_t x = x2 * PHD_ONE + 1;

            while (y_size--) {
                x += x_add;
                x_ptr->x1 = x;
                x_ptr++;
            }
        }
    }

    if (y_min == y_max) {
        return 0;
    }

    g_XGenY1 = y_min;
    g_XGenY2 = y_max;
    return 1;
}

int32_t __cdecl Output_XGenXG(const int16_t *obj_ptr)
{
    int32_t pt_count = *obj_ptr++;
    const struct XGEN_XG *pt2 = (const struct XGEN_XG *)obj_ptr;
    const struct XGEN_XG *pt1 = pt2 + (pt_count - 1);

    int32_t y_min = pt1->y;
    int32_t y_max = pt1->y;

    while (pt_count--) {
        const int32_t x1 = pt1->x;
        const int32_t y1 = pt1->y;
        const int32_t g1 = pt1->g;
        const int32_t x2 = pt2->x;
        const int32_t y2 = pt2->y;
        const int32_t g2 = pt2->g;
        pt1 = pt2++;

        if (y1 < y2) {
            CLAMPG(y_min, y1);
            const int32_t g_size = g2 - g1;
            const int32_t x_size = x2 - x1;
            int32_t y_size = y2 - y1;

            struct XBUF_XG *xg_ptr = (struct XBUF_XG *)g_XBuffer + y1;
            const int32_t x_add = PHD_ONE * x_size / y_size;
            const int32_t g_add = PHD_HALF * g_size / y_size;
            int32_t x = x1 * PHD_ONE + (PHD_ONE - 1);
            int32_t g = g1 * PHD_HALF;

            while (y_size--) {
                x += x_add;
                g += g_add;
                xg_ptr->x2 = x;
                xg_ptr->g2 = g;
                xg_ptr++;
            }
        } else if (y2 < y1) {
            CLAMPL(y_max, y1);
            const int32_t g_size = g1 - g2;
            const int32_t x_size = x1 - x2;
            int32_t y_size = y1 - y2;

            struct XBUF_XG *xg_ptr = (struct XBUF_XG *)g_XBuffer + y2;
            const int32_t x_add = PHD_ONE * x_size / y_size;
            const int32_t g_add = PHD_HALF * g_size / y_size;
            int32_t x = x2 * PHD_ONE + 1;
            int32_t g = g2 * PHD_HALF;

            while (y_size--) {
                x += x_add;
                g += g_add;
                xg_ptr->x1 = x;
                xg_ptr->g1 = g;
                xg_ptr++;
            }
        }
    }

    if (y_min == y_max) {
        return 0;
    }

    g_XGenY1 = y_min;
    g_XGenY2 = y_max;
    return 1;
}

int32_t __cdecl Output_XGenXGUV(const int16_t *obj_ptr)
{
    int32_t pt_count = *obj_ptr++;
    const struct XGEN_XGUV *pt2 = (const struct XGEN_XGUV *)obj_ptr;
    const struct XGEN_XGUV *pt1 = pt2 + (pt_count - 1);

    int32_t y_min = pt1->y;
    int32_t y_max = pt1->y;

    while (pt_count--) {
        const int32_t x1 = pt1->x;
        const int32_t y1 = pt1->y;
        const int32_t g1 = pt1->g;
        const int32_t u1 = pt1->u;
        const int32_t v1 = pt1->v;
        const int32_t x2 = pt2->x;
        const int32_t y2 = pt2->y;
        const int32_t g2 = pt2->g;
        const int32_t u2 = pt2->u;
        const int32_t v2 = pt2->v;
        pt1 = pt2++;

        if (y1 < y2) {
            CLAMPG(y_min, y1);
            const int32_t g_size = g2 - g1;
            const int32_t u_size = u2 - u1;
            const int32_t v_size = v2 - v1;
            const int32_t x_size = x2 - x1;
            int32_t y_size = y2 - y1;

            struct XBUF_XGUV *xguv_ptr = (struct XBUF_XGUV *)g_XBuffer + y1;
            const int32_t x_add = PHD_ONE * x_size / y_size;
            const int32_t g_add = PHD_HALF * g_size / y_size;
            const int32_t u_add = PHD_HALF * u_size / y_size;
            const int32_t v_add = PHD_HALF * v_size / y_size;
            int32_t x = x1 * PHD_ONE + (PHD_ONE - 1);
            int32_t g = g1 * PHD_HALF;
            int32_t u = u1 * PHD_HALF;
            int32_t v = v1 * PHD_HALF;

            while (y_size--) {
                x += x_add;
                g += g_add;
                u += u_add;
                v += v_add;
                xguv_ptr->x2 = x;
                xguv_ptr->g2 = g;
                xguv_ptr->u2 = u;
                xguv_ptr->v2 = v;
                xguv_ptr++;
            }
        } else if (y2 < y1) {
            CLAMPL(y_max, y1);
            const int32_t g_size = g1 - g2;
            const int32_t u_size = u1 - u2;
            const int32_t v_size = v1 - v2;
            const int32_t x_size = x1 - x2;
            int32_t y_size = y1 - y2;

            struct XBUF_XGUV *xguv_ptr = (struct XBUF_XGUV *)g_XBuffer + y2;
            const int32_t x_add = PHD_ONE * x_size / y_size;
            const int32_t g_add = PHD_HALF * g_size / y_size;
            const int32_t u_add = PHD_HALF * u_size / y_size;
            const int32_t v_add = PHD_HALF * v_size / y_size;
            int32_t x = x2 * PHD_ONE + 1;
            int32_t g = g2 * PHD_HALF;
            int32_t u = u2 * PHD_HALF;
            int32_t v = v2 * PHD_HALF;

            while (y_size--) {
                x += x_add;
                g += g_add;
                u += u_add;
                v += v_add;
                xguv_ptr->x1 = x;
                xguv_ptr->g1 = g;
                xguv_ptr->u1 = u;
                xguv_ptr->v1 = v;
                xguv_ptr++;
            }
        }
    }

    if (y_min == y_max) {
        return 0;
    }

    g_XGenY1 = y_min;
    g_XGenY2 = y_max;
    return 1;
}

int32_t __cdecl Output_XGen_XGUVPerspFP(const int16_t *obj_ptr)
{
    int32_t pt_count = *obj_ptr++;
    const struct XGEN_XGUVP *pt2 = (const struct XGEN_XGUVP *)obj_ptr;
    const struct XGEN_XGUVP *pt1 = pt2 + (pt_count - 1);

    int32_t y_min = pt1->y;
    int32_t y_max = pt1->y;

    while (pt_count--) {
        const int32_t x1 = pt1->x;
        const int32_t y1 = pt1->y;
        const int32_t g1 = pt1->g;
        const float u1 = pt1->u;
        const float v1 = pt1->v;

        const float rhw1 = pt1->rhw;
        const int32_t x2 = pt2->x;
        const int32_t y2 = pt2->y;
        const int32_t g2 = pt2->g;
        const float u2 = pt2->u;
        const float v2 = pt2->v;
        const float rhw2 = pt2->rhw;

        pt1 = pt2++;

        if (y1 < y2) {
            CLAMPG(y_min, y1);
            const int32_t g_size = g2 - g1;
            const float u_size = u2 - u1;
            const float v_size = v2 - v1;
            const float rhw_size = rhw2 - rhw1;
            const int32_t x_size = x2 - x1;
            int32_t y_size = y2 - y1;

            struct XBUF_XGUVP *xguv_ptr = (struct XBUF_XGUVP *)g_XBuffer + y1;
            const int32_t x_add = PHD_ONE * x_size / y_size;
            const int32_t g_add = PHD_HALF * g_size / y_size;
            const float u_add = u_size / (float)y_size;
            const float v_add = v_size / (float)y_size;
            const float rhw_add = rhw_size / (float)y_size;
            int32_t x = x1 * PHD_ONE + (PHD_ONE - 1);
            int32_t g = g1 * PHD_HALF;
            float u = u1;
            float v = v1;
            float rhw = rhw1;

            while (y_size--) {
                x += x_add;
                g += g_add;
                u += u_add;
                v += v_add;
                rhw += rhw_add;
                xguv_ptr->x2 = x;
                xguv_ptr->g2 = g;
                xguv_ptr->u2 = u;
                xguv_ptr->v2 = v;
                xguv_ptr->rhw2 = rhw;
                xguv_ptr++;
            }
        } else if (y2 < y1) {
            CLAMPL(y_max, y1);
            const int32_t g_size = g1 - g2;
            const float u_size = u1 - u2;
            const float v_size = v1 - v2;
            const float rhw_size = rhw1 - rhw2;
            const int32_t x_size = x1 - x2;
            int32_t y_size = y1 - y2;

            struct XBUF_XGUVP *xguv_ptr = (struct XBUF_XGUVP *)g_XBuffer + y2;
            const int32_t x_add = PHD_ONE * x_size / y_size;
            const int32_t g_add = PHD_HALF * g_size / y_size;
            const float u_add = u_size / (float)y_size;
            const float v_add = v_size / (float)y_size;
            const float rhw_add = rhw_size / (float)y_size;
            int32_t x = x2 * PHD_ONE + 1;
            int32_t g = g2 * PHD_HALF;
            float u = u2;
            float v = v2;
            float rhw = rhw2;

            while (y_size--) {
                x += x_add;
                g += g_add;
                u += u_add;
                v += v_add;
                rhw += rhw_add;
                xguv_ptr->x1 = x;
                xguv_ptr->g1 = g;
                xguv_ptr->u1 = u;
                xguv_ptr->v1 = v;
                xguv_ptr->rhw1 = rhw;
                xguv_ptr++;
            }
        }
    }

    if (y_min == y_max) {
        return 0;
    }

    g_XGenY1 = y_min;
    g_XGenY2 = y_max;
    return 1;
}
