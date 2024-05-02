#include "game/console_cmd.h"

#include "game/console.h"
#include "game/items.h"
#include "game/random.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"

#include <math.h>
#include <stdio.h>

static bool Console_Cmd_IsFloatRound(const float num);
static bool Console_Cmd_Pos(const char *const args);
static bool Console_Cmd_Teleport(const char *const args);
static bool Console_Cmd_SetHealth(const char *const args);
static bool Console_Cmd_Heal(const char *const args);

static inline bool Console_Cmd_IsFloatRound(const float num)
{
    return (fabsf(num) - roundf(num)) < 0.0001f;
}

static bool Console_Cmd_Pos(const char *const args)
{
    if (!g_Objects[O_LARA].loaded) {
        return false;
    }

    Console_Log(
        "Room: %d\nPosition: %.3f, %.3f, %.3f\nRotation: %.3f,%.3f,%.3f",
        g_LaraItem->room_num, g_LaraItem->pos.x / (float)WALL_L,
        g_LaraItem->pos.y / (float)WALL_L, g_LaraItem->pos.z / (float)WALL_L,
        g_LaraItem->rot.x * 360.0f / (float)PHD_ONE,
        g_LaraItem->rot.y * 360.0f / (float)PHD_ONE,
        g_LaraItem->rot.z * 360.0f / (float)PHD_ONE);
    return true;
}

static bool Console_Cmd_Teleport(const char *const args)
{
    if (!g_Objects[O_LARA].loaded || !g_LaraItem->hit_points) {
        return false;
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

            if (Item_Teleport(g_LaraItem, x * WALL_L, y * WALL_L, z * WALL_L)) {
                Console_Log("Teleported to position: %.3f %.3f %.3f", x, y, z);
                return true;
            }

            Console_Log(
                "Failed to teleport to position: %.3f %.3f %.3f", x, y, z);
            return true;
        }
    }

    // Room number
    {
        int16_t room_num = NO_ROOM;
        if (sscanf(args, "%hd", &room_num) == 1) {
            if (room_num < 0 || room_num >= g_RoomCount) {
                Console_Log(
                    "Invalid room: %d. Valid rooms are 0-%d", room_num,
                    g_RoomCount - 1);
                return true;
            }

            const ROOM_INFO *const room = &g_Rooms[room_num];

            const int32_t x1 = room->pos.x + WALL_L;
            const int32_t x2 =
                (room->y_size << WALL_SHIFT) + room->pos.x - WALL_L;
            const int32_t y1 = room->min_floor;
            const int32_t y2 = room->max_ceiling;
            const int32_t z1 = room->pos.z + WALL_L;
            const int32_t z2 =
                (room->x_size << WALL_SHIFT) + room->pos.z - WALL_L;

            for (int i = 0; i < 100; i++) {
                int32_t x = x1 + Random_GetControl() * (x2 - x1) / 0x7FFF;
                int32_t y = y1;
                int32_t z = z1 + Random_GetControl() * (z2 - z1) / 0x7FFF;
                if (Item_Teleport(g_LaraItem, x, y, z)) {
                    Console_Log("Teleported to room: %d", room_num);
                    return true;
                }
            }

            Console_Log("Failed to teleport to room: %d", room_num);
            return true;
        }
    }

    return false;
}

static bool Console_Cmd_SetHealth(const char *const args)
{
    if (!g_Objects[O_LARA].loaded) {
        return false;
    }

    if (strcmp(args, "") == 0) {
        Console_Log("Current Lara's health: %d", g_LaraItem->hit_points);
        return true;
    }

    int32_t hp;
    if (sscanf(args, "%d", &hp) != 1) {
        return false;
    }

    g_LaraItem->hit_points = hp;
    Console_Log("Lara's health set to %d", hp);
    return true;
}

static bool Console_Cmd_Heal(const char *const args)
{
    if (!g_Objects[O_LARA].loaded) {
        return false;
    }

    if (g_LaraItem->hit_points == LARA_MAX_HITPOINTS) {
        Console_Log("Lara's already at full health");
        return true;
    }

    g_LaraItem->hit_points = LARA_MAX_HITPOINTS;
    Console_Log("Healed Lara back to full health");
    return true;
}

CONSOLE_COMMAND g_ConsoleCommands[] = {
    { .prefix = "pos", .proc = Console_Cmd_Pos },
    { .prefix = "tp", .proc = Console_Cmd_Teleport },
    { .prefix = "hp", .proc = Console_Cmd_SetHealth },
    { .prefix = "heal", .proc = Console_Cmd_Heal },
    { .prefix = NULL, .proc = NULL },
};
