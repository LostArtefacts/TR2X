#include "game/matrix.h"

#include "game/math.h"
#include "global/const.h"
#include "global/types.h"
#include "global/vars.h"

#include <stdint.h>

void Matrix_GenerateW2V(struct PHD_3DPOS *viewpos)
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
