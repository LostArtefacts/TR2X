#include "specific/s_flagged_string.h"

#include "global/types.h"

void __thiscall S_FlaggedString_Create(
    struct STRING_FLAGGED *string, int32_t size)
{
    string->content = malloc(size);
    if (string->content != NULL) {
        *string->content = '\0';
        string->is_valid = true;
    }
}

void __thiscall S_FlaggedString_InitAdapter(struct DISPLAY_ADAPTER *adapter)
{
    S_FlaggedString_Create(&adapter->driver_description, 256);
    S_FlaggedString_Create(&adapter->driver_name, 256);
}

void __thiscall S_FlaggedString_Delete(struct STRING_FLAGGED *string)
{
    if (string->is_valid && string->content) {
        free(string->content);
        string->content = NULL;
        string->is_valid = false;
    }
}
