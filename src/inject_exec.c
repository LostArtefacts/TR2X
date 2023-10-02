#include "inject_exec.h"

#include "game/camera.h"
#include "game/math.h"
#include "game/matrix.h"
#include "game/shell.h"
#include "inject_util.h"

static void Inject_Camera(void);
static void Inject_Math(void);
static void Inject_Matrix(void);
static void Inject_Shell(void);

static void Inject_Camera(void)
{
    INJECT(0x004105A0, Camera_Initialise);
    INJECT(0x00410650, Camera_Move);
    INJECT(0x004109D0, Camera_Clip);
    INJECT(0x00410AB0, Camera_Shift);
    INJECT(0x00410C10, Camera_GoodPosition);
    INJECT(0x00410C60, Camera_SmartShift);
}

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
    INJECT(0x00457C58, Math_Cos);
    INJECT(0x00457C5E, Math_Sin);
    INJECT(0x00457C93, Math_Sqrt);
}

static void Inject_Shell(void)
{
    INJECT(0x0044E890, Shell_ExitSystem);
}

void Inject_Exec(void)
{
    Inject_Camera();
    Inject_Matrix();
    Inject_Math();
    Inject_Shell();
}
