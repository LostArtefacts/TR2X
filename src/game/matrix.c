#include "game/matrix.h"

#include "game/math.h"
#include "game/math_misc.h"
#include "global/const.h"
#include "global/types.h"
#include "global/vars.h"

#include <stdint.h>

void __cdecl Matrix_GenerateW2V(struct PHD_3DPOS *viewpos)
{
    g_MatrixPtr = &g_MatrixStack[0];
    int32_t sx = Math_Sin(viewpos->x_rot);
    int32_t cx = Math_Cos(viewpos->x_rot);
    int32_t sy = Math_Sin(viewpos->y_rot);
    int32_t cy = Math_Cos(viewpos->y_rot);
    int32_t sz = Math_Sin(viewpos->z_rot);
    int32_t cz = Math_Cos(viewpos->z_rot);

    g_MatrixPtr->_00 = ((cy * cz) >> W2V_MATRIX)
        + ((((sx * sy) >> W2V_MATRIX) * sz) >> W2V_MATRIX);
    g_MatrixPtr->_01 = (sz * cx) >> W2V_MATRIX;
    g_MatrixPtr->_02 = ((((sx * cy) >> W2V_MATRIX) * sz) >> W2V_MATRIX)
        - ((sy * cz) >> W2V_MATRIX);
    g_MatrixPtr->_10 = ((((sx * sy) >> W2V_MATRIX) * cz) >> W2V_MATRIX)
        - ((sz * cy) >> W2V_MATRIX);
    g_MatrixPtr->_11 = (cx * cz) >> W2V_MATRIX;
    g_MatrixPtr->_12 = ((sy * sz) >> W2V_MATRIX)
        + ((((sx * cy) >> W2V_MATRIX) * cz) >> W2V_MATRIX);
    g_MatrixPtr->_20 = (sy * cx) >> W2V_MATRIX;
    g_MatrixPtr->_21 = -sx;
    g_MatrixPtr->_22 = (cx * cy) >> W2V_MATRIX;
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
    mptr->_01 = r0 >> W2V_MATRIX;
    mptr->_02 = r1 >> W2V_MATRIX;

    r0 = mptr->_11 * cx + mptr->_12 * sx;
    r1 = mptr->_12 * cx - mptr->_11 * sx;
    mptr->_11 = r0 >> W2V_MATRIX;
    mptr->_12 = r1 >> W2V_MATRIX;

    r0 = mptr->_21 * cx + mptr->_22 * sx;
    r1 = mptr->_22 * cx - mptr->_21 * sx;
    mptr->_21 = r0 >> W2V_MATRIX;
    mptr->_22 = r1 >> W2V_MATRIX;
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
    mptr->_00 = r0 >> W2V_MATRIX;
    mptr->_02 = r1 >> W2V_MATRIX;

    r0 = mptr->_10 * cy - mptr->_12 * sy;
    r1 = mptr->_12 * cy + mptr->_10 * sy;
    mptr->_10 = r0 >> W2V_MATRIX;
    mptr->_12 = r1 >> W2V_MATRIX;

    r0 = mptr->_20 * cy - mptr->_22 * sy;
    r1 = mptr->_22 * cy + mptr->_20 * sy;
    mptr->_20 = r0 >> W2V_MATRIX;
    mptr->_22 = r1 >> W2V_MATRIX;
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
    mptr->_00 = r0 >> W2V_MATRIX;
    mptr->_01 = r1 >> W2V_MATRIX;

    r0 = mptr->_10 * cz + mptr->_11 * sz;
    r1 = mptr->_11 * cz - mptr->_10 * sz;
    mptr->_10 = r0 >> W2V_MATRIX;
    mptr->_11 = r1 >> W2V_MATRIX;

    r0 = mptr->_20 * cz + mptr->_21 * sz;
    r1 = mptr->_21 * cz - mptr->_20 * sz;
    mptr->_20 = r0 >> W2V_MATRIX;
    mptr->_21 = r1 >> W2V_MATRIX;
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
        mptr->_00 = r0 >> W2V_MATRIX;
        mptr->_02 = r1 >> W2V_MATRIX;

        r0 = mptr->_10 * cy - mptr->_12 * sy;
        r1 = mptr->_12 * cy + mptr->_10 * sy;
        mptr->_10 = r0 >> W2V_MATRIX;
        mptr->_12 = r1 >> W2V_MATRIX;

        r0 = mptr->_20 * cy - mptr->_22 * sy;
        r1 = mptr->_22 * cy + mptr->_20 * sy;
        mptr->_20 = r0 >> W2V_MATRIX;
        mptr->_22 = r1 >> W2V_MATRIX;
    }

    if (rx) {
        int32_t sx = Math_Sin(rx);
        int32_t cx = Math_Cos(rx);

        r0 = mptr->_01 * cx + mptr->_02 * sx;
        r1 = mptr->_02 * cx - mptr->_01 * sx;
        mptr->_01 = r0 >> W2V_MATRIX;
        mptr->_02 = r1 >> W2V_MATRIX;

        r0 = mptr->_11 * cx + mptr->_12 * sx;
        r1 = mptr->_12 * cx - mptr->_11 * sx;
        mptr->_11 = r0 >> W2V_MATRIX;
        mptr->_12 = r1 >> W2V_MATRIX;

        r0 = mptr->_21 * cx + mptr->_22 * sx;
        r1 = mptr->_22 * cx - mptr->_21 * sx;
        mptr->_21 = r0 >> W2V_MATRIX;
        mptr->_22 = r1 >> W2V_MATRIX;
    }

    if (rz) {
        int32_t sz = Math_Sin(rz);
        int32_t cz = Math_Cos(rz);
        r0 = mptr->_00 * cz + mptr->_01 * sz;
        r1 = mptr->_01 * cz - mptr->_00 * sz;
        mptr->_00 = r0 >> W2V_MATRIX;
        mptr->_01 = r1 >> W2V_MATRIX;

        r0 = mptr->_10 * cz + mptr->_11 * sz;
        r1 = mptr->_11 * cz - mptr->_10 * sz;
        mptr->_10 = r0 >> W2V_MATRIX;
        mptr->_11 = r1 >> W2V_MATRIX;

        r0 = mptr->_20 * cz + mptr->_21 * sz;
        r1 = mptr->_21 * cz - mptr->_20 * sz;
        mptr->_20 = r0 >> W2V_MATRIX;
        mptr->_21 = r1 >> W2V_MATRIX;
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
        mptr->_00 = r0 >> W2V_MATRIX;
        mptr->_02 = r1 >> W2V_MATRIX;

        r0 = mptr->_10 * cy - mptr->_12 * sy;
        r1 = mptr->_12 * cy + mptr->_10 * sy;
        mptr->_10 = r0 >> W2V_MATRIX;
        mptr->_12 = r1 >> W2V_MATRIX;

        r0 = mptr->_20 * cy - mptr->_22 * sy;
        r1 = mptr->_22 * cy + mptr->_20 * sy;
        mptr->_20 = r0 >> W2V_MATRIX;
        mptr->_22 = r1 >> W2V_MATRIX;
    }

    if (rx) {
        int32_t sx = Math_Sin(rx);
        int32_t cx = Math_Cos(rx);
        r0 = mptr->_01 * cx + mptr->_02 * sx;
        r1 = mptr->_02 * cx - mptr->_01 * sx;
        mptr->_01 = r0 >> W2V_MATRIX;
        mptr->_02 = r1 >> W2V_MATRIX;

        r0 = mptr->_11 * cx + mptr->_12 * sx;
        r1 = mptr->_12 * cx - mptr->_11 * sx;
        mptr->_11 = r0 >> W2V_MATRIX;
        mptr->_12 = r1 >> W2V_MATRIX;

        r0 = mptr->_21 * cx + mptr->_22 * sx;
        r1 = mptr->_22 * cx - mptr->_21 * sx;
        mptr->_21 = r0 >> W2V_MATRIX;
        mptr->_22 = r1 >> W2V_MATRIX;
    }

    if (rz) {
        int32_t sz = Math_Sin(rz);
        int32_t cz = Math_Cos(rz);
        r0 = mptr->_00 * cz + mptr->_01 * sz;
        r1 = mptr->_01 * cz - mptr->_00 * sz;
        mptr->_00 = r0 >> W2V_MATRIX;
        mptr->_01 = r1 >> W2V_MATRIX;

        r0 = mptr->_10 * cz + mptr->_11 * sz;
        r1 = mptr->_11 * cz - mptr->_10 * sz;
        mptr->_10 = r0 >> W2V_MATRIX;
        mptr->_11 = r1 >> W2V_MATRIX;

        r0 = mptr->_20 * cz + mptr->_21 * sz;
        r1 = mptr->_21 * cz - mptr->_20 * sz;
        mptr->_20 = r0 >> W2V_MATRIX;
        mptr->_21 = r1 >> W2V_MATRIX;
    }
}
