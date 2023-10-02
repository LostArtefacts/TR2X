#include "game/camera.h"

#include "game/math.h"
#include "game/matrix.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"

void __cdecl Camera_Initialise(void)
{
    g_Camera.shift = g_LaraItem->pos.y - WALL_L;

    g_Camera.target.x = g_LaraItem->pos.x;
    g_Camera.target.y = g_Camera.shift;
    g_Camera.target.z = g_LaraItem->pos.z;
    g_Camera.target.room_num = g_LaraItem->room_num;

    g_Camera.pos.x = g_Camera.target.x;
    g_Camera.pos.y = g_Camera.shift;
    g_Camera.pos.z = g_Camera.target.z - 100;
    g_Camera.pos.room_num = g_LaraItem->room_num;

    g_Camera.target_distance = WALL_L * 3 / 2;
    g_Camera.item = NULL;

    g_Camera.num_frames = 1;
    if (!g_Lara.extra_anim) {
        g_Camera.type = CAM_CHASE;
    }

    g_Camera.speed = 1;
    g_Camera.flags = 0;
    g_Camera.bounce = 0;
    g_Camera.num = -1;
    g_Camera.fixed_camera = 0;

    Viewport_AlterFOV(GAME_FOV * PHD_DEGREE);
    Camera_Update();
}

void __cdecl Camera_Move(struct GAME_VECTOR *target, int32_t speed)
{
    g_Camera.pos.x += (target->x - g_Camera.pos.x) / speed;
    g_Camera.pos.z += (target->z - g_Camera.pos.z) / speed;
    g_Camera.pos.y += (target->y - g_Camera.pos.y) / speed;
    g_Camera.pos.room_num = target->room_num;

    g_IsChunkyCamera = 0;

    const struct FLOOR_INFO *floor = Room_GetFloor(
        g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z, &g_Camera.pos.room_num);
    int32_t height =
        Room_GetHeight(floor, g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z)
        - STEP_L;

    if (g_Camera.pos.y >= height && target->y >= height) {
        LOS_Check(&g_Camera.target, &g_Camera.pos);
        floor = Room_GetFloor(
            g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z,
            &g_Camera.pos.room_num);
        height = Room_GetHeight(
                     floor, g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z)
            - STEP_L;
    }

    int32_t ceiling =
        Room_GetCeiling(floor, g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z)
        + STEP_L;
    if (height < ceiling) {
        ceiling = (height + ceiling) >> 1;
        height = ceiling;
    }

    if (g_Camera.bounce > 0) {
        g_Camera.pos.y += g_Camera.bounce;
        g_Camera.target.y += g_Camera.bounce;
        g_Camera.bounce = 0;
    } else if (g_Camera.bounce < 0) {
        struct PHD_VECTOR shake = {
            .x = g_Camera.bounce * (Random_GetControl() - 0x4000) / 0x7FFF,
            .y = g_Camera.bounce * (Random_GetControl() - 0x4000) / 0x7FFF,
            .z = g_Camera.bounce * (Random_GetControl() - 0x4000) / 0x7FFF,
        };
        g_Camera.pos.x += shake.x;
        g_Camera.pos.y += shake.y;
        g_Camera.pos.z += shake.z;
        g_Camera.target.y += shake.x;
        g_Camera.target.y += shake.y;
        g_Camera.target.z += shake.z;
        g_Camera.bounce += 5;
    }

    if (g_Camera.pos.y > height) {
        g_Camera.shift = height - g_Camera.pos.y;
    } else if (g_Camera.pos.y < ceiling) {
        g_Camera.shift = ceiling - g_Camera.pos.y;
    } else {
        g_Camera.shift = 0;
    }

    Room_GetFloor(
        g_Camera.pos.x, g_Camera.pos.y + g_Camera.shift, g_Camera.pos.z,
        &g_Camera.pos.room_num);

    Matrix_LookAt(
        g_Camera.pos.x, g_Camera.shift + g_Camera.pos.y, g_Camera.pos.z,
        g_Camera.target.x, g_Camera.target.y, g_Camera.target.z, 0);

    if (g_Camera.is_lara_mic) {
        g_Camera.actual_angle =
            g_Lara.torso_y_rot + g_Lara.head_y_rot + g_LaraItem->pos.y_rot;
        g_Camera.mic_pos.x = g_LaraItem->pos.x;
        g_Camera.mic_pos.y = g_LaraItem->pos.y;
        g_Camera.mic_pos.z = g_LaraItem->pos.z;
    } else {
        g_Camera.actual_angle = Math_Atan(
            g_Camera.target.z - g_Camera.pos.z,
            g_Camera.target.x - g_Camera.pos.x);
        g_Camera.mic_pos.x = g_Camera.pos.x
            + ((g_PhdPersp * Math_Sin(g_Camera.actual_angle)) >> W2V_SHIFT);
        g_Camera.mic_pos.z = g_Camera.pos.z
            + ((g_PhdPersp * Math_Cos(g_Camera.actual_angle)) >> W2V_SHIFT);
        g_Camera.mic_pos.y = g_Camera.pos.y;
    }
}
