#include "inject_exec.h"

#include "game/matrix.h"
#include "game/math.h"
#include "inject_util.h"

static void Inject_Matrix(void);

static void Inject_Matrix(void)
{
    INJECT(0x00401000, Matrix_GenerateW2V);
    INJECT(0x004011D0, Matrix_LookAt);
    INJECT(0x004012D0, Matrix_RotX);
    INJECT(0x00401380, Matrix_RotY);
    INJECT(0x00401430, Matrix_RotZ);
    INJECT(0x004014E0, Matrix_RotYXZ);
    INJECT(0x004016C0, Matrix_RotYXZpack);
    INJECT(0x004018B0, Matrix_TranslateRel);
}

static void Inject_Math(void)
{
    INJECT(0x00457C10, Math_Atan);
}

void Inject_Exec(void)
{
    Inject_Matrix();
    Inject_Math();
}
