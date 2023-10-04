#pragma once

#include "global/types.h"

#include <stdbool.h>

void __thiscall S_FlaggedString_Create(
    struct STRING_FLAGGED *string, int32_t size);
void __thiscall S_FlaggedString_Delete(struct STRING_FLAGGED *string);
void __thiscall S_FlaggedString_InitAdapter(struct DISPLAY_ADAPTER *adapter);
