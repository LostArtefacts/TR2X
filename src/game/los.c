#include "game/los.h"

#include "global/funcs.h"
#include "util.h"

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
