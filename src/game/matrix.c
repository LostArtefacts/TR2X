#include "game/matrix.h"

#include "game/math.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/types.h"
#include "global/vars.h"
#include "util.h"

#include <stdint.h>

void __cdecl Matrix_Pop(void)
{
    g_MatrixPtr--;
}

void __cdecl Matrix_GenerateW2V(struct PHD_3DPOS *viewpos)
{
    g_MatrixPtr = &g_MatrixStack[0];
    int32_t sx = Math_Sin(viewpos->x_rot);
    int32_t cx = Math_Cos(viewpos->x_rot);
    int32_t sy = Math_Sin(viewpos->y_rot);
    int32_t cy = Math_Cos(viewpos->y_rot);
    int32_t sz = Math_Sin(viewpos->z_rot);
    int32_t cz = Math_Cos(viewpos->z_rot);

    g_MatrixPtr->_00 = ((cy * cz) >> W2V_SHIFT)
        + ((((sx * sy) >> W2V_SHIFT) * sz) >> W2V_SHIFT);
    g_MatrixPtr->_01 = (sz * cx) >> W2V_SHIFT;
    g_MatrixPtr->_02 = ((((sx * cy) >> W2V_SHIFT) * sz) >> W2V_SHIFT)
        - ((sy * cz) >> W2V_SHIFT);
    g_MatrixPtr->_10 = ((((sx * sy) >> W2V_SHIFT) * cz) >> W2V_SHIFT)
        - ((sz * cy) >> W2V_SHIFT);
    g_MatrixPtr->_11 = (cx * cz) >> W2V_SHIFT;
    g_MatrixPtr->_12 = ((sy * sz) >> W2V_SHIFT)
        + ((((sx * cy) >> W2V_SHIFT) * cz) >> W2V_SHIFT);
    g_MatrixPtr->_20 = (sy * cx) >> W2V_SHIFT;
    g_MatrixPtr->_21 = -sx;
    g_MatrixPtr->_22 = (cx * cy) >> W2V_SHIFT;
    g_MatrixPtr->_03 = viewpos->x;
    g_MatrixPtr->_13 = viewpos->y;
    g_MatrixPtr->_23 = viewpos->z;

    g_MatrixPtr->_10 *= g_ViewportAspectRatio;
    g_MatrixPtr->_11 *= g_ViewportAspectRatio;
    g_MatrixPtr->_12 *= g_ViewportAspectRatio;

    g_W2VMatrix = *g_MatrixPtr;
}

void __cdecl Matrix_LookAt(
    int32_t xsrc, int32_t ysrc, int32_t zsrc, int32_t xtar, int32_t ytar,
    int32_t ztar, int16_t roll)
{
    PHD_ANGLE angles[2];
    Math_GetVectorAngles(xtar - xsrc, ytar - ysrc, ztar - zsrc, angles);

    struct PHD_3DPOS viewer;
    viewer.x = xsrc;
    viewer.y = ysrc;
    viewer.z = zsrc;
    viewer.x_rot = angles[1];
    viewer.y_rot = angles[0];
    viewer.z_rot = roll;
    Matrix_GenerateW2V(&viewer);
}

void __cdecl Matrix_RotX(PHD_ANGLE rx)
{
    if (!rx) {
        return;
    }

    struct MATRIX *mptr = g_MatrixPtr;
    int32_t sx = Math_Sin(rx);
    int32_t cx = Math_Cos(rx);

    int32_t r0, r1;
    r0 = mptr->_01 * cx + mptr->_02 * sx;
    r1 = mptr->_02 * cx - mptr->_01 * sx;
    mptr->_01 = r0 >> W2V_SHIFT;
    mptr->_02 = r1 >> W2V_SHIFT;

    r0 = mptr->_11 * cx + mptr->_12 * sx;
    r1 = mptr->_12 * cx - mptr->_11 * sx;
    mptr->_11 = r0 >> W2V_SHIFT;
    mptr->_12 = r1 >> W2V_SHIFT;

    r0 = mptr->_21 * cx + mptr->_22 * sx;
    r1 = mptr->_22 * cx - mptr->_21 * sx;
    mptr->_21 = r0 >> W2V_SHIFT;
    mptr->_22 = r1 >> W2V_SHIFT;
}

void __cdecl Matrix_RotY(PHD_ANGLE ry)
{
    if (!ry) {
        return;
    }

    struct MATRIX *mptr = g_MatrixPtr;
    int32_t sy = Math_Sin(ry);
    int32_t cy = Math_Cos(ry);

    int32_t r0, r1;
    r0 = mptr->_00 * cy - mptr->_02 * sy;
    r1 = mptr->_02 * cy + mptr->_00 * sy;
    mptr->_00 = r0 >> W2V_SHIFT;
    mptr->_02 = r1 >> W2V_SHIFT;

    r0 = mptr->_10 * cy - mptr->_12 * sy;
    r1 = mptr->_12 * cy + mptr->_10 * sy;
    mptr->_10 = r0 >> W2V_SHIFT;
    mptr->_12 = r1 >> W2V_SHIFT;

    r0 = mptr->_20 * cy - mptr->_22 * sy;
    r1 = mptr->_22 * cy + mptr->_20 * sy;
    mptr->_20 = r0 >> W2V_SHIFT;
    mptr->_22 = r1 >> W2V_SHIFT;
}

void __cdecl Matrix_RotZ(PHD_ANGLE rz)
{
    if (!rz) {
        return;
    }

    struct MATRIX *mptr = g_MatrixPtr;
    int32_t sz = Math_Sin(rz);
    int32_t cz = Math_Cos(rz);

    int32_t r0, r1;
    r0 = mptr->_00 * cz + mptr->_01 * sz;
    r1 = mptr->_01 * cz - mptr->_00 * sz;
    mptr->_00 = r0 >> W2V_SHIFT;
    mptr->_01 = r1 >> W2V_SHIFT;

    r0 = mptr->_10 * cz + mptr->_11 * sz;
    r1 = mptr->_11 * cz - mptr->_10 * sz;
    mptr->_10 = r0 >> W2V_SHIFT;
    mptr->_11 = r1 >> W2V_SHIFT;

    r0 = mptr->_20 * cz + mptr->_21 * sz;
    r1 = mptr->_21 * cz - mptr->_20 * sz;
    mptr->_20 = r0 >> W2V_SHIFT;
    mptr->_21 = r1 >> W2V_SHIFT;
}

void __cdecl Matrix_RotYXZ(int16_t ry, int16_t rx, int16_t rz)
{
    int32_t r0, r1;
    struct MATRIX *mptr = g_MatrixPtr;

    if (ry) {
        int32_t sy = Math_Sin(ry);
        int32_t cy = Math_Cos(ry);
        r0 = mptr->_00 * cy - mptr->_02 * sy;
        r1 = mptr->_02 * cy + mptr->_00 * sy;
        mptr->_00 = r0 >> W2V_SHIFT;
        mptr->_02 = r1 >> W2V_SHIFT;

        r0 = mptr->_10 * cy - mptr->_12 * sy;
        r1 = mptr->_12 * cy + mptr->_10 * sy;
        mptr->_10 = r0 >> W2V_SHIFT;
        mptr->_12 = r1 >> W2V_SHIFT;

        r0 = mptr->_20 * cy - mptr->_22 * sy;
        r1 = mptr->_22 * cy + mptr->_20 * sy;
        mptr->_20 = r0 >> W2V_SHIFT;
        mptr->_22 = r1 >> W2V_SHIFT;
    }

    if (rx) {
        int32_t sx = Math_Sin(rx);
        int32_t cx = Math_Cos(rx);

        r0 = mptr->_01 * cx + mptr->_02 * sx;
        r1 = mptr->_02 * cx - mptr->_01 * sx;
        mptr->_01 = r0 >> W2V_SHIFT;
        mptr->_02 = r1 >> W2V_SHIFT;

        r0 = mptr->_11 * cx + mptr->_12 * sx;
        r1 = mptr->_12 * cx - mptr->_11 * sx;
        mptr->_11 = r0 >> W2V_SHIFT;
        mptr->_12 = r1 >> W2V_SHIFT;

        r0 = mptr->_21 * cx + mptr->_22 * sx;
        r1 = mptr->_22 * cx - mptr->_21 * sx;
        mptr->_21 = r0 >> W2V_SHIFT;
        mptr->_22 = r1 >> W2V_SHIFT;
    }

    if (rz) {
        int32_t sz = Math_Sin(rz);
        int32_t cz = Math_Cos(rz);
        r0 = mptr->_00 * cz + mptr->_01 * sz;
        r1 = mptr->_01 * cz - mptr->_00 * sz;
        mptr->_00 = r0 >> W2V_SHIFT;
        mptr->_01 = r1 >> W2V_SHIFT;

        r0 = mptr->_10 * cz + mptr->_11 * sz;
        r1 = mptr->_11 * cz - mptr->_10 * sz;
        mptr->_10 = r0 >> W2V_SHIFT;
        mptr->_11 = r1 >> W2V_SHIFT;

        r0 = mptr->_20 * cz + mptr->_21 * sz;
        r1 = mptr->_21 * cz - mptr->_20 * sz;
        mptr->_20 = r0 >> W2V_SHIFT;
        mptr->_21 = r1 >> W2V_SHIFT;
    }
}

void __cdecl Matrix_RotYXZpack(uint32_t rpack)
{
    struct MATRIX *mptr = g_MatrixPtr;
    int32_t r0, r1;

    PHD_ANGLE rx = (rpack >> 14) & 0xFFC0;
    PHD_ANGLE ry = (rpack >> 4) & 0xFFC0;
    PHD_ANGLE rz = (rpack & 0x3FF) << 6;

    if (ry) {
        int32_t sy = Math_Sin(ry);
        int32_t cy = Math_Cos(ry);
        r0 = mptr->_00 * cy - mptr->_02 * sy;
        r1 = mptr->_00 * sy + mptr->_02 * cy;
        mptr->_00 = r0 >> W2V_SHIFT;
        mptr->_02 = r1 >> W2V_SHIFT;

        r0 = mptr->_10 * cy - mptr->_12 * sy;
        r1 = mptr->_12 * cy + mptr->_10 * sy;
        mptr->_10 = r0 >> W2V_SHIFT;
        mptr->_12 = r1 >> W2V_SHIFT;

        r0 = mptr->_20 * cy - mptr->_22 * sy;
        r1 = mptr->_22 * cy + mptr->_20 * sy;
        mptr->_20 = r0 >> W2V_SHIFT;
        mptr->_22 = r1 >> W2V_SHIFT;
    }

    if (rx) {
        int32_t sx = Math_Sin(rx);
        int32_t cx = Math_Cos(rx);
        r0 = mptr->_01 * cx + mptr->_02 * sx;
        r1 = mptr->_02 * cx - mptr->_01 * sx;
        mptr->_01 = r0 >> W2V_SHIFT;
        mptr->_02 = r1 >> W2V_SHIFT;

        r0 = mptr->_11 * cx + mptr->_12 * sx;
        r1 = mptr->_12 * cx - mptr->_11 * sx;
        mptr->_11 = r0 >> W2V_SHIFT;
        mptr->_12 = r1 >> W2V_SHIFT;

        r0 = mptr->_21 * cx + mptr->_22 * sx;
        r1 = mptr->_22 * cx - mptr->_21 * sx;
        mptr->_21 = r0 >> W2V_SHIFT;
        mptr->_22 = r1 >> W2V_SHIFT;
    }

    if (rz) {
        int32_t sz = Math_Sin(rz);
        int32_t cz = Math_Cos(rz);
        r0 = mptr->_00 * cz + mptr->_01 * sz;
        r1 = mptr->_01 * cz - mptr->_00 * sz;
        mptr->_00 = r0 >> W2V_SHIFT;
        mptr->_01 = r1 >> W2V_SHIFT;

        r0 = mptr->_10 * cz + mptr->_11 * sz;
        r1 = mptr->_11 * cz - mptr->_10 * sz;
        mptr->_10 = r0 >> W2V_SHIFT;
        mptr->_11 = r1 >> W2V_SHIFT;

        r0 = mptr->_20 * cz + mptr->_21 * sz;
        r1 = mptr->_21 * cz - mptr->_20 * sz;
        mptr->_20 = r0 >> W2V_SHIFT;
        mptr->_21 = r1 >> W2V_SHIFT;
    }
}

void __cdecl Matrix_RotYXZsuperpack(int16_t **pprot, int32_t index)
{
    uint16_t *prot = (uint16_t *)*pprot;

    for (int i = 0; i < index; i++) {
        if ((*prot >> 14) == 0) {
            prot += 2;
        } else {
            prot += 1;
        }
    }

    switch (*prot >> 14) {
    case 0: {
        uint32_t packed = (prot[0] << 16) + prot[1];
        Matrix_RotYXZpack(packed);
        prot += 2;
        break;
    }
    case 1:
        Matrix_RotX((int16_t)((*prot & 1023) << 6));
        prot += 1;
        break;
    case 2:
        Matrix_RotY((int16_t)((*prot & 1023) << 6));
        prot += 1;
        break;
    default:
        Matrix_RotZ((int16_t)((*prot & 1023) << 6));
        prot += 1;
        break;
    }

    *pprot = (int16_t *)prot;
}

bool __cdecl Matrix_TranslateRel(int32_t x, int32_t y, int32_t z)
{
    struct MATRIX *mptr = g_MatrixPtr;
    mptr->_03 += z * mptr->_02 + y * mptr->_01 + x * mptr->_00;
    mptr->_13 += z * mptr->_12 + y * mptr->_11 + x * mptr->_10;
    mptr->_23 += z * mptr->_22 + y * mptr->_21 + x * mptr->_20;

    return (
        ABS(mptr->_03) <= g_PhdFarZ && ABS(mptr->_13) <= g_PhdFarZ
        && ABS(mptr->_23) <= g_PhdFarZ);
}

void __cdecl Matrix_InitInterpolate(int32_t frac, int32_t rate)
{
    g_IMRate = rate;
    g_IMFrac = frac;
    g_IMMatrixPtr = g_IMMatrixStack;
    *g_IMMatrixPtr = *g_MatrixPtr;
}

void __cdecl Matrix_Interpolate(void)
{
    const int32_t frac = g_IMFrac;
    const int32_t rate = g_IMRate;
    const struct MATRIX *iptr = g_IMMatrixPtr;
    struct MATRIX *mptr = g_MatrixPtr;

    if (g_IMRate == 2) {
        mptr->_00 += (iptr->_00 - mptr->_00) / 2;
        mptr->_01 += (iptr->_01 - mptr->_01) / 2;
        mptr->_02 += (iptr->_02 - mptr->_02) / 2;
        mptr->_03 += (iptr->_03 - mptr->_03) / 2;
        mptr->_10 += (iptr->_10 - mptr->_10) / 2;
        mptr->_11 += (iptr->_11 - mptr->_11) / 2;
        mptr->_12 += (iptr->_12 - mptr->_12) / 2;
        mptr->_13 += (iptr->_13 - mptr->_13) / 2;
        mptr->_20 += (iptr->_20 - mptr->_20) / 2;
        mptr->_21 += (iptr->_21 - mptr->_21) / 2;
        mptr->_22 += (iptr->_22 - mptr->_22) / 2;
        mptr->_23 += (iptr->_23 - mptr->_23) / 2;
    } else {
        mptr->_00 += (iptr->_00 - mptr->_00) * frac / rate;
        mptr->_01 += (iptr->_01 - mptr->_01) * frac / rate;
        mptr->_02 += (iptr->_02 - mptr->_02) * frac / rate;
        mptr->_03 += (iptr->_03 - mptr->_03) * frac / rate;
        mptr->_10 += (iptr->_10 - mptr->_10) * frac / rate;
        mptr->_11 += (iptr->_11 - mptr->_11) * frac / rate;
        mptr->_12 += (iptr->_12 - mptr->_12) * frac / rate;
        mptr->_13 += (iptr->_13 - mptr->_13) * frac / rate;
        mptr->_20 += (iptr->_20 - mptr->_20) * frac / rate;
        mptr->_21 += (iptr->_21 - mptr->_21) * frac / rate;
        mptr->_22 += (iptr->_22 - mptr->_22) * frac / rate;
        mptr->_23 += (iptr->_23 - mptr->_23) * frac / rate;
    }
}

void __cdecl Matrix_Push_I(void)
{
    Matrix_PushMatrix();
    g_IMMatrixPtr[1] = g_IMMatrixPtr[0];
    g_IMMatrixPtr++;
}

void __cdecl Matrix_Pop_I(void)
{
    g_MatrixPtr--;
    g_IMMatrixPtr--;
}

void __cdecl Matrix_RotX_I(int16_t ang)
{
    Matrix_RotX(ang);
    struct MATRIX *old_matrix = g_MatrixPtr;
    g_MatrixPtr = g_IMMatrixPtr;
    Matrix_RotX(ang);
    g_MatrixPtr = old_matrix;
}

void __cdecl Matrix_RotY_I(int16_t ang)
{
    Matrix_RotY(ang);
    struct MATRIX *old_matrix = g_MatrixPtr;
    g_MatrixPtr = g_IMMatrixPtr;
    Matrix_RotY(ang);
    g_MatrixPtr = old_matrix;
}

void __cdecl Matrix_RotZ_I(int16_t ang)
{
    Matrix_RotZ(ang);
    struct MATRIX *old_matrix = g_MatrixPtr;
    g_MatrixPtr = g_IMMatrixPtr;
    Matrix_RotZ(ang);
    g_MatrixPtr = old_matrix;
}

void __cdecl Matrix_RotYXZ_I(int16_t y, int16_t x, int16_t z)
{
    Matrix_RotYXZ(y, x, z);
    struct MATRIX *old_matrix = g_MatrixPtr;
    g_MatrixPtr = g_IMMatrixPtr;
    Matrix_RotYXZ(y, x, z);
    g_MatrixPtr = old_matrix;
}

void __cdecl Matrix_RotYXZsuperpack_I(
    int16_t **pprot1, int16_t **pprot2, int32_t index)
{
    Matrix_RotYXZsuperpack(pprot1, index);
    struct MATRIX *old_matrix = g_MatrixPtr;
    g_MatrixPtr = g_IMMatrixPtr;
    Matrix_RotYXZsuperpack(pprot2, index);
    g_MatrixPtr = old_matrix;
}

void __cdecl Matrix_TranslateRel_I(int32_t x, int32_t y, int32_t z)
{
    Matrix_TranslateRel(x, y, z);
    struct MATRIX *old_matrix = g_MatrixPtr;
    g_MatrixPtr = g_IMMatrixPtr;
    Matrix_TranslateRel(x, y, z);
    g_MatrixPtr = old_matrix;
}

void __cdecl Matrix_TranslateRel_ID(
    int32_t x, int32_t y, int32_t z, int32_t x2, int32_t y2, int32_t z2)
{
    Matrix_TranslateRel(x, y, z);
    struct MATRIX *old_matrix = g_MatrixPtr;
    g_MatrixPtr = g_IMMatrixPtr;
    Matrix_TranslateRel(x2, y2, z2);
    g_MatrixPtr = old_matrix;
}
