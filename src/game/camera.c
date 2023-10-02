#include "game/camera.h"

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
