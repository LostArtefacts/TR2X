#include "game/console_cmd.h"

#include "decomp/effects.h"
#include "game/console.h"
#include "game/creature.h"
#include "game/game_string.h"
#include "game/gameflow/gameflow_new.h"
#include "game/inventory/backpack.h"
#include "game/items.h"
#include "game/lara/lara_cheat.h"
#include "game/lara/lara_control.h"
#include "game/objects/common.h"
#include "game/objects/names.h"
#include "game/objects/vars.h"
#include "game/random.h"
#include "game/room.h"
#include "game/sound.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"

#include <libtrx/memory.h>
#include <libtrx/strings.h>

#include <math.h>
#include <stdio.h>

static bool Console_Cmd_CanTargetObject(GAME_OBJECT_ID object_id);
static bool Console_Cmd_CanTargetObjectCreature(GAME_OBJECT_ID object_id);
static bool Console_Cmd_CanTargetObjectPickup(GAME_OBJECT_ID object_id);
static bool Console_Cmd_IsFloatRound(float num);
static COMMAND_RESULT Console_Cmd_Pos(const char *args);
static COMMAND_RESULT Console_Cmd_Teleport(const char *args);
static COMMAND_RESULT Console_Cmd_SetHealth(const char *args);
static COMMAND_RESULT Console_Cmd_Heal(const char *args);
static COMMAND_RESULT Console_Cmd_Fly(const char *const args);
static COMMAND_RESULT Console_Cmd_FlipMap(const char *args);
static COMMAND_RESULT Console_Cmd_GiveItem(const char *args);
static COMMAND_RESULT Console_Cmd_Kill(const char *args);
static COMMAND_RESULT Console_Cmd_EndLevel(const char *args);
static COMMAND_RESULT Console_Cmd_StartLevel(const char *args);
static COMMAND_RESULT Console_Cmd_LoadGame(const char *args);
static COMMAND_RESULT Console_Cmd_SaveGame(const char *args);
static COMMAND_RESULT Console_Cmd_StartDemo(const char *args);
static COMMAND_RESULT Console_Cmd_ExitToTitle(const char *args);
static COMMAND_RESULT Console_Cmd_ExitGame(const char *args);
static COMMAND_RESULT Console_Cmd_Abortion(const char *args);

static bool Console_Cmd_CanTargetObject(const GAME_OBJECT_ID object_id)
{
    return !Object_IsObjectType(object_id, g_NullObjects)
        && !Object_IsObjectType(object_id, g_AnimObjects)
        && !Object_IsObjectType(object_id, g_InvObjects);
}

static bool Console_Cmd_CanTargetObjectCreature(const GAME_OBJECT_ID object_id)
{
    return Object_IsObjectType(object_id, g_EnemyObjects)
        || Object_IsObjectType(object_id, g_FriendObjects);
}

static bool Console_Cmd_CanTargetObjectPickup(const GAME_OBJECT_ID object_id)
{
    return Object_IsObjectType(object_id, g_PickupObjects);
}

static inline bool Console_Cmd_IsFloatRound(const float num)
{
    return (fabsf(num) - roundf(num)) < 0.0001f;
}

static COMMAND_RESULT Console_Cmd_Pos(const char *const args)
{
    if (!g_Objects[O_LARA].loaded) {
        return CR_UNAVAILABLE;
    }

    Console_Log(
        GS(OSD_POS_GET), g_LaraItem->room_num,
        g_LaraItem->pos.x / (float)WALL_L, g_LaraItem->pos.y / (float)WALL_L,
        g_LaraItem->pos.z / (float)WALL_L,
        g_LaraItem->rot.x * 360.0f / (float)PHD_ONE,
        g_LaraItem->rot.y * 360.0f / (float)PHD_ONE,
        g_LaraItem->rot.z * 360.0f / (float)PHD_ONE);
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_Teleport(const char *const args)
{
    if (g_GameInfo.current_level.type == GFL_TITLE
        || g_GameInfo.current_level.type == GFL_DEMO
        || g_GameInfo.current_level.type == GFL_CUTSCENE) {
        return CR_UNAVAILABLE;
    }

    if (!g_Objects[O_LARA].loaded || !g_LaraItem->hit_points) {
        return CR_UNAVAILABLE;
    }

    // X Y Z
    {
        float x, y, z;
        if (sscanf(args, "%f %f %f", &x, &y, &z) == 3) {
            if (Console_Cmd_IsFloatRound(x)) {
                x += 0.5f;
            }
            if (Console_Cmd_IsFloatRound(z)) {
                z += 0.5f;
            }

            if (Lara_Cheat_Teleport(x * WALL_L, y * WALL_L, z * WALL_L)) {
                Console_Log(GS(OSD_POS_SET_POS), x, y, z);
                return CR_SUCCESS;
            }

            Console_Log(GS(OSD_POS_SET_POS_FAIL), x, y, z);
            return CR_FAILURE;
        }
    }

    // Room number
    {
        int16_t room_num = NO_ROOM_NEG;
        if (sscanf(args, "%hd", &room_num) == 1) {
            if (room_num < 0 || room_num >= g_RoomCount) {
                Console_Log(GS(OSD_INVALID_ROOM), room_num, g_RoomCount - 1);
                return CR_FAILURE;
            }

            const ROOM_INFO *const room = &g_Rooms[room_num];

            const int32_t x1 = room->pos.x + WALL_L;
            const int32_t x2 =
                (room->x_size << WALL_SHIFT) + room->pos.x - WALL_L;
            const int32_t y1 = room->min_floor;
            const int32_t y2 = room->max_ceiling;
            const int32_t z1 = room->pos.z + WALL_L;
            const int32_t z2 =
                (room->z_size << WALL_SHIFT) + room->pos.z - WALL_L;

            for (int32_t i = 0; i < 100; i++) {
                int32_t x = x1 + Random_GetControl() * (x2 - x1) / 0x7FFF;
                int32_t y = y1;
                int32_t z = z1 + Random_GetControl() * (z2 - z1) / 0x7FFF;
                if (Lara_Cheat_Teleport(x, y, z)) {
                    Console_Log(GS(OSD_POS_SET_ROOM), room_num);
                    return CR_SUCCESS;
                }
            }

            Console_Log(GS(OSD_POS_SET_ROOM_FAIL), room_num);
            return CR_FAILURE;
        }
    }

    // Nearest item of this name
    if (!String_Equivalent(args, "")) {
        int32_t match_count = 0;
        GAME_OBJECT_ID *matching_objs =
            Object_IdsFromName(args, &match_count, Console_Cmd_CanTargetObject);

        const ITEM_INFO *best_item = NULL;
        int32_t best_distance = INT32_MAX;

        for (int16_t item_num = 0; item_num < Item_GetTotalCount();
             item_num++) {
            const ITEM_INFO *const item = &g_Items[item_num];
            if (Object_IsObjectType(item->object_num, g_PickupObjects)
                && (item->status == IS_INVISIBLE
                    || item->status == IS_DEACTIVATED)) {
                continue;
            }

            if (item->flags & IF_KILLED) {
                continue;
            }

            bool is_matched = false;
            for (int32_t i = 0; i < match_count; i++) {
                if (matching_objs[i] == item->object_num) {
                    is_matched = true;
                    break;
                }
            }
            if (!is_matched) {
                continue;
            }

            const int32_t distance = Item_GetDistance(item, &g_LaraItem->pos);
            if (distance < best_distance) {
                best_distance = distance;
                best_item = item;
            }
        }

        if (best_item != NULL) {
            if (Lara_Cheat_Teleport(
                    best_item->pos.x, best_item->pos.y - STEP_L,
                    best_item->pos.z)) {
                Console_Log(
                    GS(OSD_POS_SET_ITEM),
                    Object_GetName(best_item->object_num));
            } else {
                Console_Log(
                    GS(OSD_POS_SET_ITEM_FAIL),
                    Object_GetName(best_item->object_num));
            }
            return CR_SUCCESS;
        } else {
            Console_Log(GS(OSD_POS_SET_ITEM_FAIL), args);
            return CR_FAILURE;
        }
    }

    return CR_BAD_INVOCATION;
}

static COMMAND_RESULT Console_Cmd_SetHealth(const char *const args)
{
    if (g_GameInfo.current_level.type == GFL_TITLE
        || g_GameInfo.current_level.type == GFL_DEMO
        || g_GameInfo.current_level.type == GFL_CUTSCENE) {
        return CR_UNAVAILABLE;
    }

    if (!g_Objects[O_LARA].loaded) {
        return CR_UNAVAILABLE;
    }

    if (strcmp(args, "") == 0) {
        Console_Log(GS(OSD_CURRENT_HEALTH_GET), g_LaraItem->hit_points);
        return CR_SUCCESS;
    }

    int32_t hp;
    if (sscanf(args, "%d", &hp) != 1) {
        return CR_BAD_INVOCATION;
    }

    g_LaraItem->hit_points = hp;
    Console_Log(GS(OSD_CURRENT_HEALTH_SET), hp);
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_Heal(const char *const args)
{
    if (g_GameInfo.current_level.type == GFL_TITLE
        || g_GameInfo.current_level.type == GFL_DEMO
        || g_GameInfo.current_level.type == GFL_CUTSCENE) {
        return CR_UNAVAILABLE;
    }

    if (!g_Objects[O_LARA].loaded) {
        return CR_UNAVAILABLE;
    }

    if (g_LaraItem->hit_points == LARA_MAX_HITPOINTS) {
        Console_Log(GS(OSD_HEAL_ALREADY_FULL_HP));
        return CR_SUCCESS;
    }

    g_LaraItem->hit_points = LARA_MAX_HITPOINTS;
    Console_Log(GS(OSD_HEAL_SUCCESS));
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_Fly(const char *const args)
{
    if (g_GameInfo.current_level.type == GFL_TITLE
        || g_GameInfo.current_level.type == GFL_DEMO
        || g_GameInfo.current_level.type == GFL_CUTSCENE) {
        return CR_UNAVAILABLE;
    }

    if (!g_Objects[O_LARA].loaded) {
        return CR_UNAVAILABLE;
    }
    Lara_Cheat_EnterFlyMode();
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_FlipMap(const char *const args)
{
    if (g_GameInfo.current_level.type == GFL_TITLE
        || g_GameInfo.current_level.type == GFL_DEMO
        || g_GameInfo.current_level.type == GFL_CUTSCENE) {
        return CR_UNAVAILABLE;
    }

    bool new_state;
    if (String_Equivalent(args, "")) {
        new_state = !g_FlipStatus;
    } else if (!String_ParseBool(args, &new_state)) {
        return CR_BAD_INVOCATION;
    }

    if (g_FlipStatus == new_state) {
        Console_Log(
            new_state ? GS(OSD_FLIPMAP_FAIL_ALREADY_ON)
                      : GS(OSD_FLIPMAP_FAIL_ALREADY_OFF));
        return CR_SUCCESS;
    }

    Room_FlipMap();
    Console_Log(new_state ? GS(OSD_FLIPMAP_ON) : GS(OSD_FLIPMAP_OFF));
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_Kill(const char *args)
{
    // kill all the enemies in the level
    if (String_Equivalent(args, "all")) {
        int32_t num_killed = 0;
        for (int16_t item_num = 0; item_num < Item_GetTotalCount();
             item_num++) {
            const ITEM_INFO *const item = &g_Items[item_num];
            if (!Creature_IsEnemy(item)) {
                continue;
            }
            if (Lara_Cheat_KillEnemy(item_num)) {
                num_killed++;
            }
        }

        if (num_killed == 0) {
            Console_Log(GS(OSD_KILL_ALL_FAIL));
            return CR_FAILURE;
        }

        Console_Log(GS(OSD_KILL_ALL), num_killed);
        return CR_SUCCESS;
    }

    // kill all the enemies around Lara within one tile, or a single nearest
    // enemy
    if (String_Equivalent(args, "")) {
        bool found = false;
        while (true) {
            const int16_t best_item_num = Lara_GetNearestEnemy();
            if (best_item_num == NO_ITEM) {
                break;
            }

            const ITEM_INFO *const item = &g_Items[best_item_num];
            const int32_t distance = Item_GetDistance(item, &g_LaraItem->pos);
            found |= Lara_Cheat_KillEnemy(best_item_num);
            if (distance >= WALL_L) {
                break;
            }
        }

        if (!found) {
            Console_Log(GS(OSD_KILL_FAIL));
            return CR_FAILURE;
        }

        Console_Log(GS(OSD_KILL));
        return CR_SUCCESS;
    }

    // kill a single enemy type
    {
        bool matches_found = false;
        int32_t num_killed = 0;
        int32_t match_count = 0;
        GAME_OBJECT_ID *matching_objs = Object_IdsFromName(
            args, &match_count, Console_Cmd_CanTargetObjectCreature);

        for (int16_t item_num = 0; item_num < Item_GetTotalCount();
             item_num++) {
            const ITEM_INFO *const item = &g_Items[item_num];

            bool is_matched = false;
            for (int32_t i = 0; i < match_count; i++) {
                if (matching_objs[i] == item->object_num) {
                    is_matched = true;
                    break;
                }
            }
            if (!is_matched) {
                continue;
            }
            matches_found = true;

            if (Lara_Cheat_KillEnemy(item_num)) {
                num_killed++;
            }
        }
        Memory_FreePointer(&matching_objs);

        if (!matches_found) {
            Console_Log(GS(OSD_INVALID_OBJECT), args);
            return CR_FAILURE;
        }
        if (num_killed == 0) {
            Console_Log(GS(OSD_OBJECT_NOT_FOUND), args);
            return CR_FAILURE;
        }
        Console_Log(GS(OSD_KILL_ALL), num_killed);
        return CR_SUCCESS;
    }
}

static COMMAND_RESULT Console_Cmd_GiveItem(const char *args)
{
    if (g_GameInfo.current_level.type == GFL_TITLE
        || g_GameInfo.current_level.type == GFL_DEMO
        || g_GameInfo.current_level.type == GFL_CUTSCENE) {
        return CR_UNAVAILABLE;
    }

    if (g_LaraItem == NULL) {
        return CR_UNAVAILABLE;
    }

    if (String_Equivalent(args, "keys")) {
        return Lara_Cheat_GiveAllKeys() ? CR_SUCCESS : CR_FAILURE;
    }

    if (String_Equivalent(args, "guns")) {
        return Lara_Cheat_GiveAllGuns() ? CR_SUCCESS : CR_FAILURE;
    }

    if (String_Equivalent(args, "all")) {
        return Lara_Cheat_GiveAllItems() ? CR_SUCCESS : CR_FAILURE;
    }

    int32_t num = 1;
    if (sscanf(args, "%d ", &num) == 1) {
        args = strstr(args, " ");
        if (!args) {
            return CR_BAD_INVOCATION;
        }
        args++;
    }

    if (String_Equivalent(args, "")) {
        return CR_BAD_INVOCATION;
    }

    bool found = false;
    int32_t match_count = 0;
    GAME_OBJECT_ID *matching_objs = Object_IdsFromName(
        args, &match_count, Console_Cmd_CanTargetObjectPickup);
    for (int32_t i = 0; i < match_count; i++) {
        const GAME_OBJECT_ID obj_id = matching_objs[i];
        if (g_Objects[obj_id].loaded) {
            Inv_AddItemNTimes(obj_id, num);
            Console_Log(GS(OSD_GIVE_ITEM), Object_GetName(obj_id));
            found = true;
        }
    }
    Memory_FreePointer(&matching_objs);

    if (!found) {
        Console_Log(GS(OSD_INVALID_ITEM), args);
        return CR_FAILURE;
    }

    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_EndLevel(const char *const args)
{
    if (strcmp(args, "") == 0) {
        Lara_Cheat_EndLevel();
        return CR_SUCCESS;
    }
    return CR_FAILURE;
}

static COMMAND_RESULT Console_Cmd_StartLevel(const char *const args)
{
    int32_t level_to_load = -1;

    if (level_to_load == -1) {
        int32_t num = 0;
        if (sscanf(args, "%d", &num) == 1) {
            level_to_load = num;
        }
    }

    if (level_to_load == -1 && strlen(args) >= 2) {
        for (int i = 0; i < g_GameFlow.num_levels; i++) {
            if (String_CaseSubstring(g_GF_LevelNames[i], args) != NULL) {
                level_to_load = i;
                break;
            }
        }
    }

    if (level_to_load == -1 && String_Equivalent(args, "gym")) {
        level_to_load = LV_GYM;
    }

    if (level_to_load >= g_GameFlow.num_levels) {
        Console_Log(GS(OSD_INVALID_LEVEL));
        return CR_FAILURE;
    }

    if (level_to_load != -1) {
        g_GF_OverrideDir = GFD_START_GAME | level_to_load;
        Console_Log(GS(OSD_PLAY_LEVEL), g_GF_LevelNames[level_to_load]);
        return CR_SUCCESS;
    }

    return CR_BAD_INVOCATION;
}

static COMMAND_RESULT Console_Cmd_LoadGame(const char *const args)
{
    int32_t slot_num;
    if (!String_ParseInteger(args, &slot_num)) {
        return CR_BAD_INVOCATION;
    }

    // convert 1-indexing to 0-indexing
    const int32_t slot_idx = slot_num - 1;

    if (slot_idx < 0 || slot_idx >= MAX_SAVE_SLOTS) {
        Console_Log(GS(OSD_INVALID_SAVE_SLOT), slot_num);
        return CR_FAILURE;
    }

    // TODO: replace this with a proper status check
    if (g_SavedLevels[slot_idx] <= 0) {
        Console_Log(GS(OSD_LOAD_GAME_FAIL_UNAVAILABLE_SLOT), slot_num);
        return CR_FAILURE;
    }

    g_GF_OverrideDir = GFD_START_SAVED_GAME | slot_idx;
    Console_Log(GS(OSD_LOAD_GAME), slot_num);
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_SaveGame(const char *const args)
{
    int32_t slot_num;
    if (!String_ParseInteger(args, &slot_num)) {
        return CR_BAD_INVOCATION;
    }

    // convert 1-indexing to 0-indexing
    const int32_t slot_idx = slot_num - 1;

    if (slot_idx < 0 || slot_idx >= MAX_SAVE_SLOTS) {
        Console_Log(GS(OSD_INVALID_SAVE_SLOT), slot_num);
        return CR_BAD_INVOCATION;
    }

    if (g_GameInfo.current_level.type == GFL_TITLE
        || g_GameInfo.current_level.type == GFL_DEMO
        || g_GameInfo.current_level.type == GFL_CUTSCENE) {
        return CR_UNAVAILABLE;
    }

    if (g_LaraItem == NULL || g_LaraItem->hit_points <= 0) {
        return CR_UNAVAILABLE;
    }

    CreateSaveGameInfo();
    S_SaveGame(&g_SaveGame, sizeof(SAVEGAME_INFO), slot_idx);
    GetSavedGamesList(&g_LoadGameRequester);
    Console_Log(GS(OSD_SAVE_GAME), slot_num);
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_StartDemo(const char *const args)
{
    g_GF_OverrideDir = GFD_START_DEMO;
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_ExitToTitle(const char *const args)
{
    g_GF_OverrideDir = GFD_EXIT_TO_TITLE;
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_ExitGame(const char *const args)
{
    g_GF_OverrideDir = GFD_EXIT_GAME;
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_Abortion(const char *const args)
{
    if (!g_Objects[O_LARA].loaded) {
        return CR_UNAVAILABLE;
    }

    if (g_LaraItem->hit_points <= 0) {
        return CR_UNAVAILABLE;
    }

    Sound_Effect(SFX_LARA_FALL, &g_LaraItem->pos, SPM_NORMAL);
    Effect_ExplodingDeath(g_Lara.item_num, -1, 1);

    g_LaraItem->hit_points = 0;
    g_LaraItem->flags |= IF_INVISIBLE;
    return CR_SUCCESS;
}

CONSOLE_COMMAND g_ConsoleCommands[] = {
    { .prefix = "pos", .proc = Console_Cmd_Pos },
    { .prefix = "tp", .proc = Console_Cmd_Teleport },
    { .prefix = "hp", .proc = Console_Cmd_SetHealth },
    { .prefix = "heal", .proc = Console_Cmd_Heal },
    { .prefix = "fly", .proc = Console_Cmd_Fly },
    { .prefix = "give", .proc = Console_Cmd_GiveItem },
    { .prefix = "gimme", .proc = Console_Cmd_GiveItem },
    { .prefix = "flip", .proc = Console_Cmd_FlipMap },
    { .prefix = "flipmap", .proc = Console_Cmd_FlipMap },
    { .prefix = "kill", .proc = Console_Cmd_Kill },
    { .prefix = "endlevel", .proc = Console_Cmd_EndLevel },
    { .prefix = "play", .proc = Console_Cmd_StartLevel },
    { .prefix = "level", .proc = Console_Cmd_StartLevel },
    { .prefix = "load", .proc = Console_Cmd_LoadGame },
    { .prefix = "save", .proc = Console_Cmd_SaveGame },
    { .prefix = "demo", .proc = Console_Cmd_StartDemo },
    { .prefix = "title", .proc = Console_Cmd_ExitToTitle },
    { .prefix = "exit", .proc = Console_Cmd_ExitGame },
    { .prefix = "abortion", .proc = Console_Cmd_Abortion },
    { .prefix = "natlastinks", .proc = Console_Cmd_Abortion },
    { .prefix = NULL, .proc = NULL },
};
