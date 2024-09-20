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

typedef struct {
    int32_t score;
    GAME_OBJECT_ID object_id;
} MATCH;

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

static int32_t M_NameMatch(const char *user_input, const char *name);
static void M_TryMatch(
    VECTOR *matches, const char *user_input, const char *name,
    GAME_OBJECT_ID object_id);

static int32_t M_NameMatch(const char *const user_input, const char *const name)
{
    int32_t score;

    char *regex = Memory_Alloc(strlen(user_input) + 5);
    strcpy(regex, "\\b");
    strcat(regex, user_input);
    strcat(regex, "\\b");
    if (String_Match(name, regex)) {
        // Match by full word.
        score = GOOD_MATCH_THRESHOLD + strlen(user_input) * 100 / strlen(name);
    } else if (String_CaseSubstring(name, user_input)) {
        // Match by substring.
        score = strlen(user_input) * 100 / strlen(name);
    } else {
        // No match.
        score = 0;
    }
    Memory_FreePointer(&regex);
    return score;
}

static void M_TryMatch(
    VECTOR *const matches, const char *const user_input, const char *const name,
    const GAME_OBJECT_ID object_id)
{
    int32_t score = M_NameMatch(user_input, name);
    if (!g_Objects[object_id].loaded) {
        score -= GOOD_MATCH_THRESHOLD;
    }
    if (score > 0) {
        MATCH match = {
            .score = score,
            .object_id = object_id,
        };
        Vector_Add(matches, &match);
    }
}

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
    // first, calculate the number of matches to allocate
    VECTOR *matches = Vector_Create(sizeof(MATCH));

    // Store matches from customizable inventory strings
    for (const INVENTORY_ITEM *const *item_ptr = m_InvItems; *item_ptr != NULL;
         item_ptr++) {
        const INVENTORY_ITEM *const item = *item_ptr;
        const GAME_OBJECT_ID object_id =
            Object_GetCognateInverse(item->object_id, g_ItemToInvObjectMap);
        if (filter != NULL && !filter(object_id)) {
            continue;
        }
        M_TryMatch(matches, user_input, item->string, object_id);
    }

    // Store matches from hardcoded strings
    for (GAME_OBJECT_ID object_id = 0; object_id < O_NUMBER_OF; object_id++) {
        if (filter != NULL && !filter(object_id)) {
            continue;
        }
        M_TryMatch(matches, user_input, Object_GetName(object_id), object_id);
    }

    // If we got a perfect match, discard poor matches
    bool good_matches = false;
    for (int i = 0; i < matches->count; i++) {
        const MATCH *const match = Vector_Get(matches, i);
        if (match->score >= GOOD_MATCH_THRESHOLD) {
            good_matches = true;
        }
    }
    if (good_matches) {
        for (int i = matches->count - 1; i >= 0; i--) {
            const MATCH *const match = Vector_Get(matches, i);
            if (match->score < GOOD_MATCH_THRESHOLD) {
                Vector_RemoveAt(matches, i);
            }
        }
    }

    // sort by match length so that best-matching results appear first
    for (int i = 0; i < matches->count; i++) {
        for (int j = i + 1; j < matches->count; j++) {
            if (((const MATCH *)Vector_Get(matches, i))->score
                < ((const MATCH *)Vector_Get(matches, j))->score) {
                Vector_Swap(matches, i, j);
            }
        }
    }
    for (int i = 0; i < matches->count; i++) {
        const MATCH *const match = Vector_Get(matches, i);
        LOG_DEBUG(
            "%d. %s (%d)", i, Object_GetName(match->object_id), match->score);
    }

    // Make sure the returned matching object ids are unique
    GAME_OBJECT_ID *unique_ids =
        Memory_Alloc(sizeof(GAME_OBJECT_ID) * (matches->count + 1));

    int32_t unique_count = 0;
    for (int32_t i = 0; i < matches->count; i++) {
        const MATCH *const match = Vector_Get(matches, i);
        bool is_unique = true;
        for (int32_t j = 0; j < unique_count; j++) {
            if (match->object_id == unique_ids[j]) {
                is_unique = false;
                break;
            }
        }
        if (is_unique) {
            unique_ids[unique_count++] = match->object_id;
        }
    }

    Vector_Free(matches);
    matches = NULL;

    // Finalize results
    unique_ids[unique_count] = NO_OBJECT;
    if (out_match_count != NULL) {
        *out_match_count = unique_count;
    }

    Memory_FreePointer(&matches);
    return unique_ids;
}
