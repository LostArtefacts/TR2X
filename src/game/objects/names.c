#include "game/objects/names.h"

#include "game/inventory/common.h"
#include "game/objects/common.h"
#include "game/objects/vars.h"
#include "global/vars.h"

#include <libtrx/log.h>
#include <libtrx/memory.h>
#include <libtrx/strings.h>
#include <libtrx/utils.h>
#include <libtrx/vector.h>

#include <assert.h>
#include <string.h>

#define GOOD_MATCH_THRESHOLD 50

static const INVENTORY_ITEM *const m_InvItems[] = {
    &g_Inv_Item_Stopwatch,   &g_Inv_Item_Pistols,
    &g_Inv_Item_Flare,       &g_Inv_Item_Shotgun,
    &g_Inv_Item_Magnums,     &g_Inv_Item_Uzis,
    &g_Inv_Item_Harpoon,     &g_Inv_Item_M16,
    &g_Inv_Item_Grenade,     &g_Inv_Item_PistolAmmo,
    &g_Inv_Item_ShotgunAmmo, &g_Inv_Item_MagnumAmmo,
    &g_Inv_Item_UziAmmo,     &g_Inv_Item_HarpoonAmmo,
    &g_Inv_Item_M16Ammo,     &g_Inv_Item_GrenadeAmmo,
    &g_Inv_Item_SmallMedi,   &g_Inv_Item_LargeMedi,
    &g_Inv_Item_Pickup1,     &g_Inv_Item_Pickup2,
    &g_Inv_Item_Puzzle1,     &g_Inv_Item_Puzzle2,
    &g_Inv_Item_Puzzle3,     &g_Inv_Item_Puzzle4,
    &g_Inv_Item_Key1,        &g_Inv_Item_Key2,
    &g_Inv_Item_Key3,        &g_Inv_Item_Key4,
    &g_Inv_Item_Passport,    &g_Inv_Item_Graphics,
    &g_Inv_Item_Sound,       &g_Inv_Item_Controls,
    &g_Inv_Item_Photo,       NULL,
};

static const char *m_ObjectNames[O_NUMBER_OF] = { NULL };

#undef OBJ_NAME_DEFINE
#define OBJ_NAME_DEFINE(id, str) str,
static const char *m_DefaultObjectNames[O_NUMBER_OF] = {
#include "game/objects/names.def"
};

#undef OBJ_NAME_DEFINE
#define OBJ_NAME_DEFINE(id, str)                                               \
    {                                                                          \
        QUOTE(id),                                                             \
        id,                                                                    \
    },
ENUM_STRING_MAP ENUM_STRING_MAP(GAME_OBJECT_ID)[] = {
#include "game/objects/names.def"
    { NULL, -1 }
};

const char *Object_GetName(const GAME_OBJECT_ID object_id)
{
    // TODO: remove this in favor of changing the INVENTORY_ITEM.text directly
    // clang-format off
    switch (object_id) {
        case O_PUZZLE_ITEM_1: return g_GF_Puzzle1Strings[g_CurrentLevel];
        case O_PUZZLE_ITEM_2: return g_GF_Puzzle2Strings[g_CurrentLevel];
        case O_PUZZLE_ITEM_3: return g_GF_Puzzle3Strings[g_CurrentLevel];
        case O_PUZZLE_ITEM_4: return g_GF_Puzzle4Strings[g_CurrentLevel];
        case O_KEY_ITEM_1:    return g_GF_Key1Strings[g_CurrentLevel];
        case O_KEY_ITEM_2:    return g_GF_Key2Strings[g_CurrentLevel];
        case O_KEY_ITEM_3:    return g_GF_Key3Strings[g_CurrentLevel];
        case O_KEY_ITEM_4:    return g_GF_Key4Strings[g_CurrentLevel];
        case O_PICKUP_ITEM_1: return g_GF_Pickup1Strings[g_CurrentLevel];
        case O_PICKUP_ITEM_2: return g_GF_Pickup2Strings[g_CurrentLevel];
        default: break;
    }
    // clang-format on

    return m_ObjectNames[object_id] != NULL
        ? (const char *)m_ObjectNames[object_id]
        : m_DefaultObjectNames[object_id];
}

void Object_SetName(const GAME_OBJECT_ID object_id, const char *const name)
{
    assert(object_id >= 0);
    assert(object_id < O_NUMBER_OF);
    Memory_FreePointer(&m_ObjectNames[object_id]);
    m_ObjectNames[object_id] = Memory_DupStr(name);
}

GAME_OBJECT_ID *Object_IdsFromName(
    const char *user_input, int32_t *out_match_count,
    bool (*filter)(GAME_OBJECT_ID))
{
    VECTOR *source = Vector_Create(sizeof(STRING_FUZZY_SOURCE));

    // Check customizable inventory strings
    for (const INVENTORY_ITEM *const *item_ptr = m_InvItems; *item_ptr != NULL;
         item_ptr++) {
        const INVENTORY_ITEM *const item = *item_ptr;
        const GAME_OBJECT_ID object_id =
            Object_GetCognateInverse(item->object_id, g_ItemToInvObjectMap);
        LOG_DEBUG("%d %s", object_id, item->string);
        if (filter != NULL && !filter(object_id)) {
            continue;
        }
        STRING_FUZZY_SOURCE source_item = {
            .key = item->string,
            .value = (void *)(intptr_t)object_id,
            .weight = 1,
        };
        Vector_Add(source, &source_item);
    }

    // Check hardcoded object names
    for (GAME_OBJECT_ID object_id = 0; object_id < O_NUMBER_OF; object_id++) {
        if (filter != NULL && !filter(object_id)) {
            continue;
        }
        STRING_FUZZY_SOURCE source_item = {
            .key = Object_GetName(object_id),
            .value = (void *)(intptr_t)object_id,
            .weight = 1,
        };
        Vector_Add(source, &source_item);
    }

    VECTOR *matches = String_FuzzyMatch(user_input, source);
    GAME_OBJECT_ID *results =
        Memory_Alloc(sizeof(GAME_OBJECT_ID) * (matches->count + 1));
    for (int32_t i = 0; i < matches->count; i++) {
        const STRING_FUZZY_MATCH *const match = Vector_Get(matches, i);
        results[i] = (GAME_OBJECT_ID)(intptr_t)match->value;
    }
    results[matches->count] = NO_OBJECT;
    if (out_match_count != NULL) {
        *out_match_count = matches->count;
    }

    Vector_Free(matches);
    matches = NULL;

    return results;
}
