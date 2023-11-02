#include "game/output.h"

#include "game/math.h"
#include "game/shell.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

#define VBUF_VISIBLE(a, b, c)                                                  \
    (((a).ys - (b).ys) * ((c).xs - (b).xs)                                     \
     >= ((c).ys - (b).ys) * ((a).xs - (b).xs))
#define MAKE_ZSORT(z) ((uint32_t)(z))

static double Output_CalculatePolyZ(
    enum SORT_TYPE sort_type, double z0, double z1, double z2, double z3)
{
    double zv = 0.0;

    switch (sort_type) {
    case ST_AVG_Z:
        zv = (z3 > 0.0) ? (z0 + z1 + z2 + z3) / 4.0 : (z0 + z1 + z2) / 3.0;
        break;

    case ST_MAX_Z:
        zv = z0;
        CLAMPL(zv, z1);
        CLAMPL(zv, z2);
        if (z3 > 0.0) {
            CLAMPL(zv, z3);
        }
        break;

    case ST_FAR_Z:
    default:
        zv = 1000000000.0;
        break;
    }
    return zv;
}

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
        if (x_size <= 0) {
            goto loop_end;
        }

        uint8_t *line_ptr = draw_ptr + x;
        while (x_size > 0) {
            *line_ptr = qt->index[*line_ptr];
            line_ptr++;
            x_size--;
        }

    loop_end:
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
        if (x_size <= 0) {
            goto loop_end;
        }

        int32_t g = xbuf->g1;
        const int32_t g_add = (xbuf->g2 - g) / x_size;

        uint8_t *line_ptr = draw_ptr + x;
        while (x_size > 0) {
            *line_ptr = gt->index[(g >> 16) & 0xFF];
            line_ptr++;
            g += g_add;
            x_size--;
        }

    loop_end:
        y_size--;
        xbuf++;
        draw_ptr += stride;
    }
}

static void __fastcall Output_GTMapA(
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
        if (x_size <= 0) {
            goto loop_end;
        }

        int32_t g = xbuf->g1;
        int32_t u = xbuf->u1;
        int32_t v = xbuf->v1;
        const int32_t g_add = (xbuf->g2 - g) / x_size;
        const int32_t u_add = (xbuf->u2 - u) / x_size;
        const int32_t v_add = (xbuf->v2 - v) / x_size;

        uint8_t *line_ptr = draw_ptr + x;
        while (x_size > 0) {
            const int32_t tex_idx = ((BYTE2(v) << 8) | BYTE2(u));
            uint8_t color_idx = tex_page[tex_idx];
            *line_ptr = g_DepthQTable[(g >> 16) & 0xFF].index[color_idx];
            line_ptr++;
            g += g_add;
            u += u_add;
            v += v_add;
            x_size--;
        }

    loop_end:
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
        if (x_size <= 0) {
            goto loop_end;
        }

        int32_t g = xbuf->g1;
        int32_t u = xbuf->u1;
        int32_t v = xbuf->v1;
        const int32_t g_add = (xbuf->g2 - g) / x_size;
        const int32_t u_add = (xbuf->u2 - u) / x_size;
        const int32_t v_add = (xbuf->v2 - v) / x_size;

        uint8_t *line_ptr = draw_ptr + x;
        while (x_size > 0) {
            const int32_t tex_idx = ((BYTE2(v) << 8) | BYTE2(u));
            uint8_t color_idx = tex_page[tex_idx];
            if (color_idx != 0) {
                *line_ptr = g_DepthQTable[(g >> 16) & 0xFF].index[color_idx];
            }
            line_ptr++;
            g += g_add;
            u += u_add;
            v += v_add;
            x_size--;
        }

    loop_end:
        y_size--;
        xbuf++;
        draw_ptr += stride;
    }
}

static inline void Output_ClipG(
    struct VERTEX_INFO *const buf, const struct VERTEX_INFO *const vtx1,
    const struct VERTEX_INFO *const vtx2, const float clip)
{
    buf->rhw = vtx2->rhw + (vtx1->rhw - vtx2->rhw) * clip;
    buf->g = vtx2->g + (vtx1->g - vtx2->g) * clip;
}

static inline void Output_ClipGUV(
    struct VERTEX_INFO *const buf, const struct VERTEX_INFO *const vtx1,
    const struct VERTEX_INFO *const vtx2, const float clip)
{
    buf->rhw = vtx2->rhw + (vtx1->rhw - vtx2->rhw) * clip;
    buf->g = vtx2->g + (vtx1->g - vtx2->g) * clip;
    buf->u = vtx2->u + (vtx1->u - vtx2->u) * clip;
    buf->v = vtx2->v + (vtx1->v - vtx2->v) * clip;
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
    g_Sort3DPtr = g_SortBuffer;
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
    g_FltResZBuf = 0.005 + res_z / g_FltNearZ;
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

void __cdecl Output_DrawPolyGTMapPersp(const int16_t *obj_ptr)
{
    if (Output_XGenXGUVPerspFP(obj_ptr + 1)) {
        Output_GTMapPersp32FP(
            g_XGenY1, g_XGenY2, g_TexturePageBuffer8[*obj_ptr]);
    }
}

void __cdecl Output_DrawPolyWGTMapPersp(const int16_t *obj_ptr)
{
    if (Output_XGenXGUVPerspFP(obj_ptr + 1)) {
        Output_WGTMapPersp32FP(
            g_XGenY1, g_XGenY2, g_TexturePageBuffer8[*obj_ptr]);
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

int32_t __cdecl Output_XGenXGUVPerspFP(const int16_t *obj_ptr)
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

void __cdecl Output_GTMapPersp32FP(
    const int32_t y1, const int32_t y2, const uint8_t *const tex_page)
{
    int32_t y_size = y2 - y1;
    if (y_size <= 0) {
        return;
    }

    const int32_t stride = g_PhdScreenWidth;
    const struct XBUF_XGUVP *xbuf = (const struct XBUF_XGUVP *)g_XBuffer + y1;
    uint8_t *draw_ptr = g_PrintSurfacePtr + y1 * stride;

    while (y_size > 0) {
        const int32_t x = xbuf->x1 / PHD_ONE;
        int32_t x_size = (xbuf->x2 / PHD_ONE) - x;
        if (x_size <= 0) {
            goto loop_end;
        }

        int32_t g = xbuf->g1;
        double u = xbuf->u1;
        double v = xbuf->v1;
        double rhw = xbuf->rhw1;

        const int32_t g_add = (xbuf->g2 - g) / x_size;

        int32_t u0 = PHD_HALF * u / rhw;
        int32_t v0 = PHD_HALF * v / rhw;

        uint8_t *line_ptr = draw_ptr + x;
        int32_t batch_size = 32;

        if (x_size >= batch_size) {
            const double u_add =
                (xbuf->u2 - u) / (double)x_size * (double)batch_size;
            const double v_add =
                (xbuf->v2 - v) / (double)x_size * (double)batch_size;
            const double rhw_add =
                (xbuf->rhw2 - rhw) / (double)x_size * (double)batch_size;

            while (x_size >= batch_size) {
                u += u_add;
                v += v_add;
                rhw += rhw_add;

                const int32_t u1 = PHD_HALF * u / rhw;
                const int32_t v1 = PHD_HALF * v / rhw;

                const int32_t u0_add = (u1 - u0) / batch_size;
                const int32_t v0_add = (v1 - v0) / batch_size;

                if ((ABS(u0_add) + ABS(v0_add)) < (PHD_ONE / 2)) {
                    int32_t batch_counter = batch_size / 2;
                    while (batch_counter--) {
                        uint8_t color_idx =
                            tex_page[(BYTE2(v0) << 8) | BYTE2(u0)];
                        color_idx =
                            g_DepthQTable[(g >> 16) & 0xFF].index[color_idx];
                        *line_ptr++ = color_idx;
                        *line_ptr++ = color_idx;
                        g += g_add * 2;
                        u0 += u0_add * 2;
                        v0 += v0_add * 2;
                    }
                } else {
                    int32_t batch_counter = batch_size;
                    while (batch_counter--) {
                        uint8_t color_idx =
                            tex_page[(BYTE2(v0) << 8) | BYTE2(u0)];
                        *line_ptr++ =
                            g_DepthQTable[(g >> 16) & 0xFF].index[color_idx];
                        g += g_add;
                        u0 += u0_add;
                        v0 += v0_add;
                    }
                }

                u0 = u1;
                v0 = v1;
                x_size -= batch_size;
            }
        }

        if (x_size > 1) {
            const int32_t u1 = PHD_HALF * xbuf->u2 / xbuf->rhw2;
            const int32_t v1 = PHD_HALF * xbuf->v2 / xbuf->rhw2;
            const int32_t u0_add = (u1 - u0) / x_size;
            const int32_t v0_add = (v1 - v0) / x_size;

            batch_size = x_size & ~1;
            x_size -= batch_size;

            if ((ABS(u0_add) + ABS(v0_add)) < (PHD_ONE / 2)) {
                int32_t batch_counter = batch_size / 2;
                while (batch_counter--) {
                    uint8_t color_idx = tex_page[(BYTE2(v0) << 8) | BYTE2(u0)];
                    color_idx =
                        g_DepthQTable[(g >> 16) & 0xFF].index[color_idx];
                    *line_ptr++ = color_idx;
                    *line_ptr++ = color_idx;
                    g += g_add * 2;
                    u0 += u0_add * 2;
                    v0 += v0_add * 2;
                }
            } else {
                int32_t batch_counter = batch_size;
                while (batch_counter--) {
                    uint8_t color_idx = tex_page[(BYTE2(v0) << 8) | BYTE2(u0)];
                    *line_ptr++ =
                        g_DepthQTable[(g >> 16) & 0xFF].index[color_idx];
                    g += g_add;
                    u0 += u0_add;
                    v0 += v0_add;
                }
            }
        }

        if (x_size == 1) {
            const uint8_t color_idx = tex_page[(BYTE2(v0) << 8) | BYTE2(u0)];
            *line_ptr = g_DepthQTable[(g >> 16) & 0xFF].index[color_idx];
        }

    loop_end:
        y_size--;
        xbuf++;
        draw_ptr += stride;
    }
}

void __cdecl Output_WGTMapPersp32FP(
    int32_t y1, int32_t y2, const uint8_t *const tex_page)
{
    int32_t y_size = y2 - y1;
    if (y_size <= 0) {
        return;
    }

    const int32_t stride = g_PhdScreenWidth;
    const struct XBUF_XGUVP *xbuf = (const struct XBUF_XGUVP *)g_XBuffer + y1;
    uint8_t *draw_ptr = g_PrintSurfacePtr + y1 * stride;

    while (y_size > 0) {
        const int32_t x = xbuf->x1 / PHD_ONE;
        int32_t x_size = (xbuf->x2 / PHD_ONE) - x;
        if (x_size <= 0) {
            goto loop_end;
        }

        int32_t g = xbuf->g1;
        double u = xbuf->u1;
        double v = xbuf->v1;
        double rhw = xbuf->rhw1;

        const int32_t g_add = (xbuf->g2 - g) / x_size;

        int32_t u0 = PHD_HALF * u / rhw;
        int32_t v0 = PHD_HALF * v / rhw;

        uint8_t *line_ptr = draw_ptr + x;
        int32_t batch_size = 32;

        if (x_size >= batch_size) {
            const double u_add =
                (xbuf->u2 - u) / (double)x_size * (double)batch_size;
            const double v_add =
                (xbuf->v2 - v) / (double)x_size * (double)batch_size;
            const double rhw_add =
                (xbuf->rhw2 - rhw) / (double)x_size * (double)batch_size;

            while (x_size >= batch_size) {
                u += u_add;
                v += v_add;
                rhw += rhw_add;

                const int32_t u1 = PHD_HALF * u / rhw;
                const int32_t v1 = PHD_HALF * v / rhw;

                const int32_t u0_add = (u1 - u0) / batch_size;
                const int32_t v0_add = (v1 - v0) / batch_size;

                if ((ABS(u0_add) + ABS(v0_add)) < (PHD_ONE / 2)) {
                    int32_t batch_counter = batch_size / 2;
                    while (batch_counter--) {
                        uint8_t color_idx =
                            tex_page[(BYTE2(v0) << 8) | BYTE2(u0)];
                        if (color_idx != 0) {
                            color_idx = g_DepthQTable[(g >> 16) & 0xFF]
                                            .index[color_idx];
                            line_ptr[0] = color_idx;
                            line_ptr[1] = color_idx;
                        }
                        line_ptr += 2;
                        g += g_add * 2;
                        u0 += u0_add * 2;
                        v0 += v0_add * 2;
                    }
                } else {
                    int32_t batch_counter = batch_size;
                    while (batch_counter--) {
                        uint8_t color_idx =
                            tex_page[(BYTE2(v0) << 8) | BYTE2(u0)];
                        if (color_idx != 0) {
                            *line_ptr =
                                g_DepthQTable[BYTE2(g)].index[color_idx];
                        }
                        line_ptr++;
                        g += g_add;
                        u0 += u0_add;
                        v0 += v0_add;
                    }
                }

                u0 = u1;
                v0 = v1;
                x_size -= batch_size;
            }
        }

        if (x_size > 1) {
            const int32_t u1 = PHD_HALF * xbuf->u2 / xbuf->rhw2;
            const int32_t v1 = PHD_HALF * xbuf->v2 / xbuf->rhw2;
            const int32_t u0_add = (u1 - u0) / x_size;
            const int32_t v0_add = (v1 - v0) / x_size;

            batch_size = x_size & ~1;
            x_size -= batch_size;

            if ((ABS(u0_add) + ABS(v0_add)) < (PHD_ONE / 2)) {
                int32_t batch_counter = batch_size / 2;
                while (batch_counter--) {
                    uint8_t color_idx = tex_page[(BYTE2(v0) << 8) | BYTE2(u0)];
                    if (color_idx != 0) {
                        color_idx =
                            g_DepthQTable[(g >> 16) & 0xFF].index[color_idx];
                        line_ptr[0] = color_idx;
                        line_ptr[1] = color_idx;
                    }
                    line_ptr += 2;
                    g += g_add * 2;
                    u0 += u0_add * 2;
                    v0 += v0_add * 2;
                };
            } else {
                int32_t batch_counter = batch_size;
                while (batch_counter--) {
                    uint8_t color_idx = tex_page[(BYTE2(v0) << 8) | BYTE2(u0)];
                    if (color_idx != 0) {
                        *line_ptr =
                            g_DepthQTable[(g >> 16) & 0xFF].index[color_idx];
                    }
                    line_ptr++;
                    g += g_add;
                    u0 += u0_add;
                    v0 += v0_add;
                }
            }
        }

        if (x_size == 1) {
            const uint8_t color_idx = tex_page[(BYTE2(v0) << 8) | BYTE2(u0)];
            if (color_idx != 0) {
                *line_ptr = g_DepthQTable[(g >> 16) & 0xFF].index[color_idx];
            }
        }

    loop_end:
        y_size--;
        xbuf++;
        draw_ptr += stride;
    }
}

int32_t __cdecl Output_VisibleZClip(
    const struct PHD_VBUF *const vtx0, const struct PHD_VBUF *const vtx1,
    const struct PHD_VBUF *const vtx2)
{
    // clang-format off
    return (
        vtx1->xv * (vtx0->yv * vtx2->zv - vtx0->zv * vtx2->yv) +
        vtx1->yv * (vtx0->zv * vtx2->xv - vtx0->xv * vtx2->zv) +
        vtx1->zv * (vtx0->xv * vtx2->yv - vtx0->yv * vtx2->xv) < 0.0
    );
    // clang-format on
}

int32_t __cdecl Output_ZedClipper(
    const int32_t vtx_count, const struct POINT_INFO *const pts,
    struct VERTEX_INFO *const vtx)
{
    int j = 0;
    const struct POINT_INFO *pts0 = &pts[0];
    const struct POINT_INFO *pts1 = &pts[vtx_count - 1];

    for (int i = 0; i < vtx_count; i++) {
        int32_t diff0 = g_FltNearZ - pts0->zv;
        int32_t diff1 = g_FltNearZ - pts1->zv;
        if ((diff0 | diff1) >= 0) {
            goto loop_end;
        }

        if ((diff0 ^ diff1) < 0) {
            double clip = diff0 / (pts1->zv - pts0->zv);
            vtx[j].x =
                (pts0->xv + (pts1->xv - pts0->xv) * clip) * g_FltPerspONearZ
                + g_FltWinCenterX;
            vtx[j].y =
                (pts0->yv + (pts1->yv - pts0->yv) * clip) * g_FltPerspONearZ
                + g_FltWinCenterY;
            vtx[j].rhw = g_FltRhwONearZ;
            vtx[j].g = pts0->g + (pts1->g - pts0->g) * clip;
            vtx[j].u = (pts0->u + (pts1->u - pts0->u) * clip) * g_FltRhwONearZ;
            vtx[j].v = (pts0->v + (pts1->v - pts0->v) * clip) * g_FltRhwONearZ;
            j++;
        }

        if (diff0 < 0) {
            vtx[j].x = pts0->xs;
            vtx[j].y = pts0->ys;
            vtx[j].rhw = pts0->rhw;
            vtx[j].g = pts0->g;
            vtx[j].u = pts0->u * pts0->rhw;
            vtx[j].v = pts0->v * pts0->rhw;
            j++;
        }

    loop_end:
        pts1 = pts0++;
    }

    return (j < 3) ? 0 : j;
}

int32_t __cdecl Output_XYGClipper(int32_t vtx_count, struct VERTEX_INFO *vtx)
{
    struct VERTEX_INFO vtx_buf[8];
    struct VERTEX_INFO *vtx1;
    struct VERTEX_INFO *vtx2;
    int j;

    if (vtx_count < 3) {
        return 0;
    }

    // horizontal clip
    j = 0;
    vtx2 = &vtx[vtx_count - 1];
    for (int i = 0; i < vtx_count; i++) {
        vtx1 = vtx2;
        vtx2 = &vtx[i];

        if (vtx1->x < g_FltWinLeft) {
            if (vtx2->x < g_FltWinLeft) {
                continue;
            }
            const float clip = (g_FltWinLeft - vtx2->x) / (vtx1->x - vtx2->x);
            vtx_buf[j].x = g_FltWinLeft;
            vtx_buf[j].y = vtx2->y + (vtx1->y - vtx2->y) * clip;
            Output_ClipG(&vtx_buf[j++], vtx1, vtx2, clip);
        } else if (vtx1->x > g_FltWinRight) {
            if (vtx2->x > g_FltWinRight) {
                continue;
            }
            const float clip = (g_FltWinRight - vtx2->x) / (vtx1->x - vtx2->x);
            vtx_buf[j].x = g_FltWinRight;
            vtx_buf[j].y = vtx2->y + (vtx1->y - vtx2->y) * clip;
            Output_ClipG(&vtx_buf[j++], vtx1, vtx2, clip);
        }

        if (vtx2->x < g_FltWinLeft) {
            const float clip = (g_FltWinLeft - vtx2->x) / (vtx1->x - vtx2->x);
            vtx_buf[j].x = g_FltWinLeft;
            vtx_buf[j].y = vtx2->y + (vtx1->y - vtx2->y) * clip;
            Output_ClipG(&vtx_buf[j++], vtx1, vtx2, clip);
        } else if (vtx2->x > g_FltWinRight) {
            const float clip = (g_FltWinRight - vtx2->x) / (vtx1->x - vtx2->x);
            vtx_buf[j].x = g_FltWinRight;
            vtx_buf[j].y = vtx2->y + (vtx1->y - vtx2->y) * clip;
            Output_ClipG(&vtx_buf[j++], vtx1, vtx2, clip);
        } else {
            vtx_buf[j++] = *vtx2;
        }
    }
    vtx_count = j;

    if (vtx_count < 3) {
        return 0;
    }

    // vertical clip
    j = 0;
    vtx2 = &vtx_buf[vtx_count - 1];
    for (int i = 0; i < vtx_count; i++) {
        vtx1 = vtx2;
        vtx2 = &vtx_buf[i];

        if (vtx1->y < g_FltWinTop) {
            if (vtx2->y < g_FltWinTop) {
                continue;
            }
            const float clip = (g_FltWinTop - vtx2->y) / (vtx1->y - vtx2->y);
            vtx[j].x = vtx2->x + (vtx1->x - vtx2->x) * clip;
            vtx[j].y = g_FltWinTop;
            Output_ClipG(&vtx[j++], vtx1, vtx2, clip);
        } else if (vtx1->y > g_FltWinBottom) {
            if (vtx2->y > g_FltWinBottom) {
                continue;
            }
            const float clip = (g_FltWinBottom - vtx2->y) / (vtx1->y - vtx2->y);
            vtx[j].x = vtx2->x + (vtx1->x - vtx2->x) * clip;
            vtx[j].y = g_FltWinBottom;
            Output_ClipG(&vtx[j++], vtx1, vtx2, clip);
        }

        if (vtx2->y < g_FltWinTop) {
            const float clip = (g_FltWinTop - vtx2->y) / (vtx1->y - vtx2->y);
            vtx[j].x = vtx2->x + (vtx1->x - vtx2->x) * clip;
            vtx[j].y = g_FltWinTop;
            Output_ClipG(&vtx[j++], vtx1, vtx2, clip);
        } else if (vtx2->y > g_FltWinBottom) {
            const float clip = (g_FltWinBottom - vtx2->y) / (vtx1->y - vtx2->y);
            vtx[j].x = vtx2->x + (vtx1->x - vtx2->x) * clip;
            vtx[j].y = g_FltWinBottom;
            Output_ClipG(&vtx[j++], vtx1, vtx2, clip);
        } else {
            vtx[j++] = *vtx2;
        }
    }

    return (j < 3) ? 0 : j;
}

int32_t __cdecl Output_XYGUVClipper(
    int32_t vtx_count, struct VERTEX_INFO *const vtx)
{
    struct VERTEX_INFO vtx_buf[8];
    struct VERTEX_INFO *vtx1;
    struct VERTEX_INFO *vtx2;
    int j;

    if (vtx_count < 3) {
        return 0;
    }

    // horizontal clip
    j = 0;
    vtx2 = &vtx[vtx_count - 1];
    for (int i = 0; i < vtx_count; i++) {
        vtx1 = vtx2;
        vtx2 = &vtx[i];

        if (vtx1->x < g_FltWinLeft) {
            if (vtx2->x < g_FltWinLeft) {
                continue;
            }
            float clip = (g_FltWinLeft - vtx2->x) / (vtx1->x - vtx2->x);
            vtx_buf[j].x = g_FltWinLeft;
            vtx_buf[j].y = vtx2->y + (vtx1->y - vtx2->y) * clip;
            Output_ClipGUV(&vtx_buf[j++], vtx1, vtx2, clip);
        } else if (vtx1->x > g_FltWinRight) {
            if (vtx2->x > g_FltWinRight) {
                continue;
            }
            float clip = (g_FltWinRight - vtx2->x) / (vtx1->x - vtx2->x);
            vtx_buf[j].x = g_FltWinRight;
            vtx_buf[j].y = vtx2->y + (vtx1->y - vtx2->y) * clip;
            Output_ClipGUV(&vtx_buf[j++], vtx1, vtx2, clip);
        }

        if (vtx2->x < g_FltWinLeft) {
            float clip = (g_FltWinLeft - vtx2->x) / (vtx1->x - vtx2->x);
            vtx_buf[j].x = g_FltWinLeft;
            vtx_buf[j].y = vtx2->y + (vtx1->y - vtx2->y) * clip;
            Output_ClipGUV(&vtx_buf[j++], vtx1, vtx2, clip);
        } else if (vtx2->x > g_FltWinRight) {
            float clip = (g_FltWinRight - vtx2->x) / (vtx1->x - vtx2->x);
            vtx_buf[j].x = g_FltWinRight;
            vtx_buf[j].y = vtx2->y + (vtx1->y - vtx2->y) * clip;
            Output_ClipGUV(&vtx_buf[j++], vtx1, vtx2, clip);
        } else {
            vtx_buf[j++] = *vtx2;
        }
    }

    vtx_count = j;
    if (vtx_count < 3) {
        return 0;
    }

    // vertical clip
    j = 0;
    vtx2 = &vtx_buf[vtx_count - 1];
    for (int i = 0; i < vtx_count; i++) {
        vtx1 = vtx2;
        vtx2 = &vtx_buf[i];

        if (vtx1->y < g_FltWinTop) {
            if (vtx2->y < g_FltWinTop) {
                continue;
            }
            const float clip = (g_FltWinTop - vtx2->y) / (vtx1->y - vtx2->y);
            vtx[j].x = vtx2->x + (vtx1->x - vtx2->x) * clip;
            vtx[j].y = g_FltWinTop;
            Output_ClipGUV(&vtx[j++], vtx1, vtx2, clip);
        } else if (vtx1->y > g_FltWinBottom) {
            if (vtx2->y > g_FltWinBottom) {
                continue;
            }
            const float clip = (g_FltWinBottom - vtx2->y) / (vtx1->y - vtx2->y);
            vtx[j].x = vtx2->x + (vtx1->x - vtx2->x) * clip;
            vtx[j].y = g_FltWinBottom;
            Output_ClipGUV(&vtx[j++], vtx1, vtx2, clip);
        }

        if (vtx2->y < g_FltWinTop) {
            const float clip = (g_FltWinTop - vtx2->y) / (vtx1->y - vtx2->y);
            vtx[j].x = vtx2->x + (vtx1->x - vtx2->x) * clip;
            vtx[j].y = g_FltWinTop;
            Output_ClipGUV(&vtx[j++], vtx1, vtx2, clip);
        } else if (vtx2->y > g_FltWinBottom) {
            const float clip = (g_FltWinBottom - vtx2->y) / (vtx1->y - vtx2->y);
            vtx[j].x = vtx2->x + (vtx1->x - vtx2->x) * clip;
            vtx[j].y = g_FltWinBottom;
            Output_ClipGUV(&vtx[j++], vtx1, vtx2, clip);
        } else {
            vtx[j++] = *vtx2;
        }
    }

    return (j < 3) ? 0 : j;
}

const int16_t *__cdecl Output_InsertObjectG3(
    const int16_t *obj_ptr, int32_t num, enum SORT_TYPE sort_type)
{
    for (int i = 0; i < num; i++) {
        const struct PHD_VBUF *const vtx[3] = {
            &g_PhdVBuf[*obj_ptr++],
            &g_PhdVBuf[*obj_ptr++],
            &g_PhdVBuf[*obj_ptr++],
        };
        const uint8_t color_idx = *obj_ptr++;
        int32_t num_points = 3;

        int8_t clip_or = BYTE0(vtx[0]->clip | vtx[1]->clip | vtx[2]->clip);
        int8_t clip_and = BYTE0(vtx[0]->clip & vtx[1]->clip & vtx[2]->clip);

        if (clip_and != 0) {
            continue;
        }

        if (clip_or >= 0) {
            if (!VBUF_VISIBLE(*vtx[0], *vtx[1], *vtx[2])) {
                continue;
            }

            g_VBuffer[0].x = vtx[0]->xs;
            g_VBuffer[0].y = vtx[0]->ys;
            g_VBuffer[0].rhw = vtx[0]->rhw;
            g_VBuffer[0].g = (float)vtx[0]->g;

            g_VBuffer[1].x = vtx[1]->xs;
            g_VBuffer[1].y = vtx[1]->ys;
            g_VBuffer[1].rhw = vtx[1]->rhw;
            g_VBuffer[1].g = (float)vtx[1]->g;

            g_VBuffer[2].x = vtx[2]->xs;
            g_VBuffer[2].y = vtx[2]->ys;
            g_VBuffer[2].rhw = vtx[2]->rhw;
            g_VBuffer[2].g = (float)vtx[2]->g;

            if (clip_or > 0) {
                num_points = Output_XYGClipper(num_points, g_VBuffer);
            }
        } else {
            if (!Output_VisibleZClip(vtx[0], vtx[1], vtx[2])) {
                continue;
            }

            struct POINT_INFO pts[3] = {
                {
                    .xv = vtx[0]->xv,
                    .yv = vtx[0]->yv,
                    .zv = vtx[0]->zv,
                    .rhw = vtx[0]->rhw,
                    .xs = vtx[0]->xs,
                    .ys = vtx[0]->ys,
                    .g = (float)vtx[0]->g,
                },
                {
                    .xv = vtx[1]->xv,
                    .yv = vtx[1]->yv,
                    .zv = vtx[1]->zv,
                    .rhw = vtx[1]->rhw,
                    .xs = vtx[1]->xs,
                    .ys = vtx[1]->ys,
                    .g = (float)vtx[1]->g,
                },
                {
                    .xv = vtx[2]->xv,
                    .yv = vtx[2]->yv,
                    .zv = vtx[2]->zv,
                    .rhw = vtx[2]->rhw,
                    .xs = vtx[2]->xs,
                    .ys = vtx[2]->ys,
                    .g = (float)vtx[2]->g,
                },
            };

            num_points = Output_ZedClipper(num_points, pts, g_VBuffer);
            if (num_points == 0) {
                continue;
            }

            num_points = Output_XYGClipper(num_points, g_VBuffer);
        }

        if (num_points == 0) {
            continue;
        }

        const float zv = Output_CalculatePolyZ(
            sort_type, vtx[0]->zv, vtx[1]->zv, vtx[2]->zv, -1.0);
        g_Sort3DPtr->_0 = (uint32_t)g_Info3DPtr;
        g_Sort3DPtr->_1 = MAKE_ZSORT(zv);
        g_Sort3DPtr++;

        *g_Info3DPtr++ = POLY_GOURAUD;
        *g_Info3DPtr++ = color_idx;
        *g_Info3DPtr++ = num_points;

        for (int j = 0; j < num_points; j++) {
            *g_Info3DPtr++ = (int)g_VBuffer[j].x;
            *g_Info3DPtr++ = (int)g_VBuffer[j].y;
            *g_Info3DPtr++ = (int)g_VBuffer[j].g;
        }
        g_SurfaceCount++;
    }

    return obj_ptr;
}

const int16_t *__cdecl Output_InsertObjectGT3(
    const int16_t *obj_ptr, int32_t num, enum SORT_TYPE sort_type)
{
    for (int i = 0; i < num; i++) {
        const struct PHD_VBUF *const vtx[3] = {
            &g_PhdVBuf[*obj_ptr++],
            &g_PhdVBuf[*obj_ptr++],
            &g_PhdVBuf[*obj_ptr++],
        };
        const int16_t texture_idx = *obj_ptr++;
        const struct PHD_TEXTURE *const texture =
            &g_PhdTextureInfo[texture_idx];
        const struct PHD_UV *const uv = texture->uv;
        int32_t num_points = 3;

        const int8_t clip_or =
            BYTE0(vtx[0]->clip | vtx[1]->clip | vtx[2]->clip);
        const int8_t clip_and =
            BYTE0(vtx[0]->clip & vtx[1]->clip & vtx[2]->clip);

        if (clip_and != 0) {
            continue;
        }

        if (clip_or >= 0) {
            if (!VBUF_VISIBLE(*vtx[0], *vtx[1], *vtx[2])) {
                continue;
            }

            if (clip_or == 0) {
                const float zv = Output_CalculatePolyZ(
                    sort_type, vtx[0]->zv, vtx[1]->zv, vtx[2]->zv, -1.0);
                g_Sort3DPtr->_0 = (uint32_t)g_Info3DPtr;
                g_Sort3DPtr->_1 = MAKE_ZSORT(zv);
                g_Sort3DPtr++;

                if (zv >= (double)g_PerspectiveDistance) {
                    *g_Info3DPtr++ = (texture->draw_type == DRAW_OPAQUE)
                        ? POLY_GTMAP
                        : POLY_WGTMAP;
                    *g_Info3DPtr++ = texture->tex_page;
                    *g_Info3DPtr++ = 3;

                    *g_Info3DPtr++ = (int)vtx[0]->xs;
                    *g_Info3DPtr++ = (int)vtx[0]->ys;
                    *g_Info3DPtr++ = (int)vtx[0]->g;
                    *g_Info3DPtr++ = uv[0].u;
                    *g_Info3DPtr++ = uv[0].v;

                    *g_Info3DPtr++ = (int)vtx[1]->xs;
                    *g_Info3DPtr++ = (int)vtx[1]->ys;
                    *g_Info3DPtr++ = (int)vtx[1]->g;
                    *g_Info3DPtr++ = uv[1].u;
                    *g_Info3DPtr++ = uv[1].v;

                    *g_Info3DPtr++ = (int)vtx[2]->xs;
                    *g_Info3DPtr++ = (int)vtx[2]->ys;
                    *g_Info3DPtr++ = (int)vtx[2]->g;
                    *g_Info3DPtr++ = uv[2].u;
                    *g_Info3DPtr++ = uv[2].v;
                } else {
                    *g_Info3DPtr++ = (texture->draw_type == DRAW_OPAQUE)
                        ? POLY_GTMAP_PERSP
                        : POLY_WGTMAP_PERSP;
                    *g_Info3DPtr++ = texture->tex_page;
                    *g_Info3DPtr++ = 3;

                    *g_Info3DPtr++ = (int)vtx[0]->xs;
                    *g_Info3DPtr++ = (int)vtx[0]->ys;
                    *g_Info3DPtr++ = (int)vtx[0]->g;
                    *(float *)g_Info3DPtr = vtx[0]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                    *(float *)g_Info3DPtr = (double)uv[0].u * vtx[0]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                    *(float *)g_Info3DPtr = (double)uv[0].v * vtx[0]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);

                    *g_Info3DPtr++ = (int)vtx[1]->xs;
                    *g_Info3DPtr++ = (int)vtx[1]->ys;
                    *g_Info3DPtr++ = (int)vtx[1]->g;
                    *(float *)g_Info3DPtr = vtx[1]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                    *(float *)g_Info3DPtr = (double)uv[1].u * vtx[1]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                    *(float *)g_Info3DPtr = (double)uv[1].v * vtx[1]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);

                    *g_Info3DPtr++ = (int)vtx[2]->xs;
                    *g_Info3DPtr++ = (int)vtx[2]->ys;
                    *g_Info3DPtr++ = (int)vtx[2]->g;
                    *(float *)g_Info3DPtr = vtx[2]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                    *(float *)g_Info3DPtr = (double)uv[2].u * vtx[2]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                    *(float *)g_Info3DPtr = (double)uv[2].v * vtx[2]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                }
                g_SurfaceCount++;
                continue;
            }

            g_VBuffer[0].x = vtx[0]->xs;
            g_VBuffer[0].y = vtx[0]->ys;
            g_VBuffer[0].rhw = vtx[0]->rhw;
            g_VBuffer[0].g = (float)vtx[0]->g;
            g_VBuffer[0].u = (double)uv[0].u * vtx[0]->rhw;
            g_VBuffer[0].v = (double)uv[0].v * vtx[0]->rhw;

            g_VBuffer[1].x = vtx[1]->xs;
            g_VBuffer[1].y = vtx[1]->ys;
            g_VBuffer[1].rhw = vtx[1]->rhw;
            g_VBuffer[1].g = (float)vtx[1]->g;
            g_VBuffer[1].u = (double)uv[1].u * vtx[1]->rhw;
            g_VBuffer[1].v = (double)uv[1].v * vtx[1]->rhw;

            g_VBuffer[2].x = vtx[2]->xs;
            g_VBuffer[2].y = vtx[2]->ys;
            g_VBuffer[2].rhw = vtx[2]->rhw;
            g_VBuffer[2].g = (float)vtx[2]->g;
            g_VBuffer[2].u = (double)uv[2].u * vtx[2]->rhw;
            g_VBuffer[2].v = (double)uv[2].v * vtx[2]->rhw;
        } else {
            if (!Output_VisibleZClip(vtx[0], vtx[1], vtx[2])) {
                continue;
            }

            const struct POINT_INFO points[3] = {
                {
                    .xv = vtx[0]->xv,
                    .yv = vtx[0]->yv,
                    .zv = vtx[0]->zv,
                    .rhw = vtx[0]->rhw,
                    .xs = vtx[0]->xs,
                    .ys = vtx[0]->ys,
                    .g = (float)vtx[0]->g,
                    .u = (float)uv[0].u,
                    .v = (float)uv[0].v,
                },
                {
                    .yv = vtx[1]->yv,
                    .xv = vtx[1]->xv,
                    .zv = vtx[1]->zv,
                    .rhw = vtx[1]->rhw,
                    .xs = vtx[1]->xs,
                    .ys = vtx[1]->ys,
                    .g = (float)vtx[1]->g,
                    .u = (float)uv[1].u,
                    .v = (float)uv[1].v,
                },
                {
                    .xv = vtx[2]->xv,
                    .yv = vtx[2]->yv,
                    .zv = vtx[2]->zv,
                    .rhw = vtx[2]->rhw,
                    .xs = vtx[2]->xs,
                    .ys = vtx[2]->ys,
                    .g = (float)vtx[2]->g,
                    .u = (float)uv[2].u,
                    .v = (float)uv[2].v,
                },
            };

            num_points = Output_ZedClipper(num_points, points, g_VBuffer);
            if (num_points == 0) {
                continue;
            }
        }

        num_points = Output_XYGUVClipper(num_points, g_VBuffer);
        if (num_points == 0) {
            continue;
        }

        const float zv = Output_CalculatePolyZ(
            sort_type, vtx[0]->zv, vtx[1]->zv, vtx[2]->zv, -1.0);
        g_Sort3DPtr->_0 = (uint32_t)g_Info3DPtr;
        g_Sort3DPtr->_1 = MAKE_ZSORT(zv);
        g_Sort3DPtr++;

        if (zv >= (double)g_PerspectiveDistance) {
            *g_Info3DPtr++ =
                (texture->draw_type == DRAW_OPAQUE) ? POLY_GTMAP : POLY_WGTMAP;
            *g_Info3DPtr++ = texture->tex_page;
            *g_Info3DPtr++ = num_points;

            for (int j = 0; j < num_points; j++) {
                *g_Info3DPtr++ = (int)g_VBuffer[j].x;
                *g_Info3DPtr++ = (int)g_VBuffer[j].y;
                *g_Info3DPtr++ = (int)g_VBuffer[j].g;
                *g_Info3DPtr++ = (int)(g_VBuffer[j].u / g_VBuffer[j].rhw);
                *g_Info3DPtr++ = (int)(g_VBuffer[j].v / g_VBuffer[j].rhw);
            }
        } else {
            *g_Info3DPtr++ = (texture->draw_type == DRAW_OPAQUE)
                ? POLY_GTMAP_PERSP
                : POLY_WGTMAP_PERSP;
            *g_Info3DPtr++ = texture->tex_page;
            *g_Info3DPtr++ = num_points;

            for (int j = 0; j < num_points; j++) {
                *g_Info3DPtr++ = (int)g_VBuffer[j].x;
                *g_Info3DPtr++ = (int)g_VBuffer[j].y;
                *g_Info3DPtr++ = (int)g_VBuffer[j].g;
                *(float *)g_Info3DPtr = g_VBuffer[j].rhw;
                g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                *(float *)g_Info3DPtr = g_VBuffer[j].u;
                g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                *(float *)g_Info3DPtr = g_VBuffer[j].v;
                g_Info3DPtr += sizeof(float) / sizeof(int16_t);
            }
        }
        g_SurfaceCount++;
    }

    return obj_ptr;
}

const int16_t *__cdecl Output_InsertObjectG4(
    const int16_t *obj_ptr, int32_t num, enum SORT_TYPE sort_type)
{
    for (int i = 0; i < num; i++) {
        const struct PHD_VBUF *const vtx[4] = {
            &g_PhdVBuf[*obj_ptr++],
            &g_PhdVBuf[*obj_ptr++],
            &g_PhdVBuf[*obj_ptr++],
            &g_PhdVBuf[*obj_ptr++],
        };
        const uint8_t color_idx = *obj_ptr++;
        int32_t num_points = 4;

        const int8_t clip_or =
            BYTE0(vtx[0]->clip | vtx[1]->clip | vtx[2]->clip | vtx[3]->clip);
        const int8_t clip_and =
            LOBYTE(vtx[0]->clip & vtx[1]->clip & vtx[2]->clip & vtx[3]->clip);

        if (clip_and != 0) {
            continue;
        }

        if (clip_or >= 0) {
            if (!VBUF_VISIBLE(*vtx[0], *vtx[1], *vtx[2])) {
                continue;
            }

            g_VBuffer[0].x = vtx[0]->xs;
            g_VBuffer[0].y = vtx[0]->ys;
            g_VBuffer[0].rhw = vtx[0]->rhw;
            g_VBuffer[0].g = (float)vtx[0]->g;

            g_VBuffer[1].x = vtx[1]->xs;
            g_VBuffer[1].y = vtx[1]->ys;
            g_VBuffer[1].rhw = vtx[1]->rhw;
            g_VBuffer[1].g = (float)vtx[1]->g;

            g_VBuffer[2].x = vtx[2]->xs;
            g_VBuffer[2].y = vtx[2]->ys;
            g_VBuffer[2].rhw = vtx[2]->rhw;
            g_VBuffer[2].g = (float)vtx[2]->g;

            g_VBuffer[3].x = vtx[3]->xs;
            g_VBuffer[3].y = vtx[3]->ys;
            g_VBuffer[3].rhw = vtx[3]->rhw;
            g_VBuffer[3].g = (float)vtx[3]->g;

            if (clip_or > 0) {
                num_points = Output_XYGClipper(num_points, g_VBuffer);
            }
        } else {
            if (!Output_VisibleZClip(vtx[0], vtx[1], vtx[2])) {
                continue;
            }

            const struct POINT_INFO points[4] = {
                {
                    .xv = vtx[0]->xv,
                    .yv = vtx[0]->yv,
                    .zv = vtx[0]->zv,
                    .rhw = vtx[0]->rhw,
                    .xs = vtx[0]->xs,
                    .ys = vtx[0]->ys,
                    .g = (float)vtx[0]->g,
                },
                {
                    .xv = vtx[1]->xv,
                    .yv = vtx[1]->yv,
                    .zv = vtx[1]->zv,
                    .rhw = vtx[1]->rhw,
                    .xs = vtx[1]->xs,
                    .ys = vtx[1]->ys,
                    .g = (float)vtx[1]->g,
                },
                {
                    .xv = vtx[2]->xv,
                    .yv = vtx[2]->yv,
                    .zv = vtx[2]->zv,
                    .rhw = vtx[2]->rhw,
                    .xs = vtx[2]->xs,
                    .ys = vtx[2]->ys,
                    .g = (float)vtx[2]->g,
                },
                {
                    .xv = vtx[3]->xv,
                    .yv = vtx[3]->yv,
                    .zv = vtx[3]->zv,
                    .rhw = vtx[3]->rhw,
                    .xs = vtx[3]->xs,
                    .ys = vtx[3]->ys,
                    .g = (float)vtx[3]->g,
                },
            };

            num_points = Output_ZedClipper(num_points, points, g_VBuffer);
            if (num_points == 0) {
                continue;
            }

            num_points = Output_XYGClipper(num_points, g_VBuffer);
        }

        if (num_points == 0) {
            continue;
        }

        const float zv = Output_CalculatePolyZ(
            sort_type, vtx[0]->zv, vtx[1]->zv, vtx[2]->zv, vtx[3]->zv);
        g_Sort3DPtr->_0 = (uint32_t)g_Info3DPtr;
        g_Sort3DPtr->_1 = MAKE_ZSORT(zv);
        g_Sort3DPtr++;

        *g_Info3DPtr++ = POLY_GOURAUD;
        *g_Info3DPtr++ = color_idx;
        *g_Info3DPtr++ = num_points;

        for (int j = 0; j < num_points; j++) {
            *g_Info3DPtr++ = g_VBuffer[j].x;
            *g_Info3DPtr++ = g_VBuffer[j].y;
            *g_Info3DPtr++ = g_VBuffer[j].g;
        }
        g_SurfaceCount++;
    }

    return obj_ptr;
}

const int16_t *__cdecl Output_InsertObjectGT4(
    const int16_t *obj_ptr, int32_t num, enum SORT_TYPE sort_type)
{
    for (int i = 0; i < num; i++) {
        const struct PHD_VBUF *const vtx[4] = {
            &g_PhdVBuf[*obj_ptr++],
            &g_PhdVBuf[*obj_ptr++],
            &g_PhdVBuf[*obj_ptr++],
            &g_PhdVBuf[*obj_ptr++],
        };
        const int16_t texture_idx = *obj_ptr++;
        const struct PHD_TEXTURE *const texture =
            &g_PhdTextureInfo[texture_idx];
        const struct PHD_UV *const uv = texture->uv;
        int32_t num_points = 4;

        const int8_t clip_or =
            BYTE0(vtx[0]->clip | vtx[1]->clip | vtx[2]->clip | vtx[3]->clip);
        const int8_t clip_and =
            BYTE0(vtx[0]->clip & vtx[1]->clip & vtx[2]->clip & vtx[3]->clip);

        if (clip_and != 0) {
            continue;
        }

        if (clip_or >= 0) {
            if (!VBUF_VISIBLE(*vtx[0], *vtx[1], *vtx[2])) {
                continue;
            }

            if (clip_or == 0) {
                const float zv = Output_CalculatePolyZ(
                    sort_type, vtx[0]->zv, vtx[1]->zv, vtx[2]->zv, vtx[3]->zv);
                g_Sort3DPtr->_0 = (uint32_t)g_Info3DPtr;
                g_Sort3DPtr->_1 = MAKE_ZSORT(zv);
                g_Sort3DPtr++;

                if (zv >= (double)g_PerspectiveDistance) {
                    *g_Info3DPtr++ = (texture->draw_type == DRAW_OPAQUE)
                        ? POLY_GTMAP
                        : POLY_WGTMAP;
                    *g_Info3DPtr++ = texture->tex_page;
                    *g_Info3DPtr++ = 4;

                    *g_Info3DPtr++ = (int)vtx[0]->xs;
                    *g_Info3DPtr++ = (int)vtx[0]->ys;
                    *g_Info3DPtr++ = (int)vtx[0]->g;
                    *g_Info3DPtr++ = uv[0].u;
                    *g_Info3DPtr++ = uv[0].v;

                    *g_Info3DPtr++ = (int)vtx[1]->xs;
                    *g_Info3DPtr++ = (int)vtx[1]->ys;
                    *g_Info3DPtr++ = (int)vtx[1]->g;
                    *g_Info3DPtr++ = uv[1].u;
                    *g_Info3DPtr++ = uv[1].v;

                    *g_Info3DPtr++ = (int)vtx[2]->xs;
                    *g_Info3DPtr++ = (int)vtx[2]->ys;
                    *g_Info3DPtr++ = (int)vtx[2]->g;
                    *g_Info3DPtr++ = uv[2].u;
                    *g_Info3DPtr++ = uv[2].v;

                    *g_Info3DPtr++ = (int)vtx[3]->xs;
                    *g_Info3DPtr++ = (int)vtx[3]->ys;
                    *g_Info3DPtr++ = (int)vtx[3]->g;
                    *g_Info3DPtr++ = uv[3].u;
                    *g_Info3DPtr++ = uv[3].v;
                } else {
                    *g_Info3DPtr++ = (texture->draw_type == DRAW_OPAQUE)
                        ? POLY_GTMAP_PERSP
                        : POLY_WGTMAP_PERSP;
                    *g_Info3DPtr++ = texture->tex_page;
                    *g_Info3DPtr++ = 4;

                    *g_Info3DPtr++ = (int)vtx[0]->xs;
                    *g_Info3DPtr++ = (int)vtx[0]->ys;
                    *g_Info3DPtr++ = (int)vtx[0]->g;
                    *(float *)g_Info3DPtr = vtx[0]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                    *(float *)g_Info3DPtr = (double)uv[0].u * vtx[0]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                    *(float *)g_Info3DPtr = (double)uv[0].v * vtx[0]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);

                    *g_Info3DPtr++ = (int)vtx[1]->xs;
                    *g_Info3DPtr++ = (int)vtx[1]->ys;
                    *g_Info3DPtr++ = (int)vtx[1]->g;
                    *(float *)g_Info3DPtr = vtx[1]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                    *(float *)g_Info3DPtr = (double)uv[1].u * vtx[1]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                    *(float *)g_Info3DPtr = (double)uv[1].v * vtx[1]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);

                    *g_Info3DPtr++ = (int)vtx[2]->xs;
                    *g_Info3DPtr++ = (int)vtx[2]->ys;
                    *g_Info3DPtr++ = (int)vtx[2]->g;
                    *(float *)g_Info3DPtr = vtx[2]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                    *(float *)g_Info3DPtr = (double)uv[2].u * vtx[2]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                    *(float *)g_Info3DPtr = (double)uv[2].v * vtx[2]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);

                    *g_Info3DPtr++ = (int)vtx[3]->xs;
                    *g_Info3DPtr++ = (int)vtx[3]->ys;
                    *g_Info3DPtr++ = (int)vtx[3]->g;
                    *(float *)g_Info3DPtr = vtx[3]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                    *(float *)g_Info3DPtr = (double)uv[3].u * vtx[3]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                    *(float *)g_Info3DPtr = (double)uv[3].v * vtx[3]->rhw;
                    g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                }
                g_SurfaceCount++;
                continue;
            }

            g_VBuffer[0].x = vtx[0]->xs;
            g_VBuffer[0].y = vtx[0]->ys;
            g_VBuffer[0].rhw = vtx[0]->rhw;
            g_VBuffer[0].g = (float)vtx[0]->g;
            g_VBuffer[0].u = (double)uv[0].u * vtx[0]->rhw;
            g_VBuffer[0].v = (double)uv[0].v * vtx[0]->rhw;

            g_VBuffer[1].x = vtx[1]->xs;
            g_VBuffer[1].y = vtx[1]->ys;
            g_VBuffer[1].rhw = vtx[1]->rhw;
            g_VBuffer[1].g = (float)vtx[1]->g;
            g_VBuffer[1].u = (double)uv[1].u * vtx[1]->rhw;
            g_VBuffer[1].v = (double)uv[1].v * vtx[1]->rhw;

            g_VBuffer[2].x = vtx[2]->xs;
            g_VBuffer[2].y = vtx[2]->ys;
            g_VBuffer[2].rhw = vtx[2]->rhw;
            g_VBuffer[2].g = (float)vtx[2]->g;
            g_VBuffer[2].u = (double)uv[2].u * vtx[2]->rhw;
            g_VBuffer[2].v = (double)uv[2].v * vtx[2]->rhw;

            g_VBuffer[3].x = vtx[3]->xs;
            g_VBuffer[3].y = vtx[3]->ys;
            g_VBuffer[3].rhw = vtx[3]->rhw;
            g_VBuffer[3].g = (float)vtx[3]->g;
            g_VBuffer[3].u = (double)uv[3].u * vtx[3]->rhw;
            g_VBuffer[3].v = (double)uv[3].v * vtx[3]->rhw;
        } else {
            if (!Output_VisibleZClip(vtx[0], vtx[1], vtx[2])) {
                continue;
            }

            const struct POINT_INFO points[4] = {
                {
                    .xv = vtx[0]->xv,
                    .yv = vtx[0]->yv,
                    .zv = vtx[0]->zv,
                    .rhw = vtx[0]->rhw,
                    .xs = vtx[0]->xs,
                    .ys = vtx[0]->ys,
                    .g = (float)vtx[0]->g,
                    .u = (float)uv[0].u,
                    .v = (float)uv[0].v,
                },
                {
                    .yv = vtx[1]->yv,
                    .xv = vtx[1]->xv,
                    .zv = vtx[1]->zv,
                    .rhw = vtx[1]->rhw,
                    .xs = vtx[1]->xs,
                    .ys = vtx[1]->ys,
                    .g = (float)vtx[1]->g,
                    .u = (float)uv[1].u,
                    .v = (float)uv[1].v,
                },
                {
                    .xv = vtx[2]->xv,
                    .yv = vtx[2]->yv,
                    .zv = vtx[2]->zv,
                    .rhw = vtx[2]->rhw,
                    .xs = vtx[2]->xs,
                    .ys = vtx[2]->ys,
                    .g = (float)vtx[2]->g,
                    .u = (float)uv[2].u,
                    .v = (float)uv[2].v,
                },
                {
                    .xv = vtx[3]->xv,
                    .yv = vtx[3]->yv,
                    .zv = vtx[3]->zv,
                    .rhw = vtx[3]->rhw,
                    .xs = vtx[3]->xs,
                    .ys = vtx[3]->ys,
                    .g = (float)vtx[3]->g,
                    .u = (float)uv[3].u,
                    .v = (float)uv[3].v,
                },
            };

            num_points = Output_ZedClipper(num_points, points, g_VBuffer);
            if (num_points == 0) {
                continue;
            }
        }

        num_points = Output_XYGUVClipper(num_points, g_VBuffer);
        if (num_points == 0) {
            continue;
        }

        const float zv = Output_CalculatePolyZ(
            sort_type, vtx[0]->zv, vtx[1]->zv, vtx[2]->zv, vtx[3]->zv);
        g_Sort3DPtr->_0 = (uint32_t)g_Info3DPtr;
        g_Sort3DPtr->_1 = MAKE_ZSORT(zv);
        g_Sort3DPtr++;

        if (zv >= (double)g_PerspectiveDistance) {
            *g_Info3DPtr++ =
                (texture->draw_type == DRAW_OPAQUE) ? POLY_GTMAP : POLY_WGTMAP;
            *g_Info3DPtr++ = texture->tex_page;
            *g_Info3DPtr++ = num_points;

            for (int j = 0; j < num_points; j++) {
                *g_Info3DPtr++ = (int)g_VBuffer[j].x;
                *g_Info3DPtr++ = (int)g_VBuffer[j].y;
                *g_Info3DPtr++ = (int)g_VBuffer[j].g;
                *g_Info3DPtr++ = (int)(g_VBuffer[j].u / g_VBuffer[j].rhw);
                *g_Info3DPtr++ = (int)(g_VBuffer[j].v / g_VBuffer[j].rhw);
            }
        } else {
            *g_Info3DPtr++ = (texture->draw_type == DRAW_OPAQUE)
                ? POLY_GTMAP_PERSP
                : POLY_WGTMAP_PERSP;
            *g_Info3DPtr++ = texture->tex_page;
            *g_Info3DPtr++ = num_points;

            for (int j = 0; j < num_points; j++) {
                *g_Info3DPtr++ = (int)g_VBuffer[j].x;
                *g_Info3DPtr++ = (int)g_VBuffer[j].y;
                *g_Info3DPtr++ = (int)g_VBuffer[j].g;
                *(float *)g_Info3DPtr = g_VBuffer[j].rhw;
                g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                *(float *)g_Info3DPtr = g_VBuffer[j].u;
                g_Info3DPtr += sizeof(float) / sizeof(int16_t);
                *(float *)g_Info3DPtr = g_VBuffer[j].v;
                g_Info3DPtr += sizeof(float) / sizeof(int16_t);
            }
        }
        g_SurfaceCount++;
    }

    return obj_ptr;
}
