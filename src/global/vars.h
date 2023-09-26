#pragma once

#include "global/types.h"
#include "inject_util.h"

#define g_MatrixPtr VAR_U_(0x004B2A28, MATRIX *)
#define g_MatrixStack ARRAY_(0x004BCB58, MATRIX, [])
#define g_W2VMatrix VAR_U_(0x004B2AC0, MATRIX)
#define g_ViewportAspectRatio VAR_I_(0x0046C304, float, 0.0f)
