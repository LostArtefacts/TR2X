#include "inject_exec.h"

#include "game/matrix.h"
#include "inject_util.h"

static void Inject_Matrix(void);

static void Inject_Matrix(void)
{
    INJECT(0x00401000, Matrix_GenerateW2V);
    INJECT(0x004011D0, Matrix_LookAt);
}

void Inject_Exec(void)
{
    Inject_Matrix();
}
