#include "game/los.h"

#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

int32_t __cdecl LOS_CheckZ(
    struct GAME_VECTOR *const start, struct GAME_VECTOR *const target)
{
    const int32_t dz = target->z - start->z;
    if (dz == 0) {
        return 1;
    }

    const int32_t dx = ((target->x - start->x) << WALL_SHIFT) / dz;
    const int32_t dy = ((target->y - start->y) << WALL_SHIFT) / dz;

    int16_t room_num = start->room_num;
    int16_t last_room_num = room_num;

    g_LOSRooms[0] = room_num;
    g_LOSNumRooms = 1;

    if (dz < 0) {
        int32_t z = start->z & (~(WALL_L - 1));
        int32_t x = start->x + ((dx * (z - start->z)) >> WALL_SHIFT);
        int32_t y = start->y + ((dy * (z - start->z)) >> WALL_SHIFT);

        while (z > target->z) {
            {
                const struct FLOOR_INFO *const floor =
                    Room_GetFloor(x, y, z, &room_num);
                const int32_t height = Room_GetHeight(floor, x, y, z);
                const int32_t ceiling = Room_GetCeiling(floor, x, y, z);
                if (y > height || y < ceiling) {
                    target->x = x;
                    target->y = y;
                    target->z = z;
                    target->room_num = room_num;
                    return -1;
                }
            }

            if (room_num != last_room_num) {
                last_room_num = room_num;
                g_LOSRooms[g_LOSNumRooms++] = room_num;
            }

            {
                const struct FLOOR_INFO *const floor =
                    Room_GetFloor(x, y, z - 1, &room_num);
                const int32_t height = Room_GetHeight(floor, x, y, z - 1);
                const int32_t ceiling = Room_GetCeiling(floor, x, y, z - 1);
                if (y > height || y < ceiling) {
                    target->x = x;
                    target->y = y;
                    target->z = z;
                    target->room_num = last_room_num;
                    return 0;
                }
            }

            z -= WALL_L;
            x -= dx;
            y -= dy;
        }
    } else {
        int32_t z = start->z | (WALL_L - 1);
        int32_t x = start->x + ((dx * (z - start->z)) >> WALL_SHIFT);
        int32_t y = start->y + ((dy * (z - start->z)) >> WALL_SHIFT);

        while (z < target->z) {
            {
                const struct FLOOR_INFO *const floor =
                    Room_GetFloor(x, y, z, &room_num);
                const int32_t height = Room_GetHeight(floor, x, y, z);
                const int32_t ceiling = Room_GetCeiling(floor, x, y, z);
                if (y > height || y < ceiling) {
                    target->x = x;
                    target->y = y;
                    target->z = z;
                    target->room_num = room_num;
                    return -1;
                }
            }

            if (room_num != last_room_num) {
                last_room_num = room_num;
                g_LOSRooms[g_LOSNumRooms++] = room_num;
            }

            {
                const struct FLOOR_INFO *const floor =
                    Room_GetFloor(x, y, z + 1, &room_num);
                const int32_t height = Room_GetHeight(floor, x, y, z + 1);
                const int32_t ceiling = Room_GetCeiling(floor, x, y, z + 1);
                if (y > height || y < ceiling) {
                    target->x = x;
                    target->y = y;
                    target->z = z;
                    target->room_num = last_room_num;
                    return 0;
                }
            }

            z += WALL_L;
            x += dx;
            y += dy;
        }
    }

    target->room_num = room_num;
    return 1;
}

int32_t __cdecl LOS_Check(struct GAME_VECTOR *start, struct GAME_VECTOR *target)
{
    int32_t los1;
    int32_t los2;

    if (ABS(target->z - start->z) > ABS(target->x - start->x)) {
        los1 = LOS_CheckX(start, target);
        los2 = LOS_CheckZ(start, target);
    } else {
        los1 = LOS_CheckZ(start, target);
        los2 = LOS_CheckX(start, target);
    }

    if (!los2) {
        return 0;
    }

    const FLOOR_INFO *const floor =
        Room_GetFloor(target->x, target->y, target->z, &target->room_num);

    if (!LOS_ClipTarget(start, target, floor)) {
        return 0;
    }
    if (los1 == 1 && los2 == 1) {
        return 1;
    }
    return 0;
}
