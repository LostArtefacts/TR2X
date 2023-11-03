#include "game/camera.h"

#include "game/los.h"
#include "game/math.h"
#include "game/matrix.h"
#include "game/music.h"
#include "game/output.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

#define CHASE_SPEED 10
#define CHASE_ELEVATION (WALL_L * 3 / 2) // = 1536

#define COMBAT_SPEED 8
#define COMBAT_DISTANCE (WALL_L * 5 / 2) // = 2560

#define LOOK_DISTANCE (WALL_L * 3 / 2) // = 1536
#define LOOK_CLAMP (STEP_L + 50) // = 296
#define LOOK_SPEED 4

#define MAX_ELEVATION (85 * PHD_DEGREE) // = 15470

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
    g_Camera.flags = CF_NORMAL;
    g_Camera.bounce = 0;
    g_Camera.num = NO_CAMERA;
    g_Camera.fixed_camera = 0;

    Output_AlterFOV(GAME_FOV * PHD_DEGREE);
    Camera_Update();
}

void __cdecl Camera_Move(const struct GAME_VECTOR *target, int32_t speed)
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

void __cdecl Camera_Clip(
    int32_t *x, int32_t *y, int32_t *h, int32_t target_x, int32_t target_y,
    int32_t target_h, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    if ((right > left) != (target_x < left)) {
        *y = target_y + (left - target_x) * (*y - target_y) / (*x - target_x);
        *h = target_h + (left - target_x) * (*h - target_h) / (*x - target_x);
        *x = left;
    }

    if ((bottom > top && target_y > top && (*y) < top)
        || (bottom < top && target_y < top && (*y) > top)) {
        *x = target_x + (top - target_y) * (*x - target_x) / (*y - target_y);
        *h = target_h + (top - target_y) * (*h - target_h) / (*y - target_y);
        *y = top;
    }
}

void __cdecl Camera_Shift(
    int32_t *x, int32_t *y, int32_t *h, int32_t target_x, int32_t target_y,
    int32_t target_h, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    int32_t shift;

    int32_t l_square = SQUARE(target_x - left);
    int32_t r_square = SQUARE(target_x - right);
    int32_t t_square = SQUARE(target_y - top);
    int32_t b_square = SQUARE(target_y - bottom);
    int32_t tl_square = t_square + l_square;
    int32_t tr_square = t_square + r_square;
    int32_t bl_square = b_square + l_square;

    if (g_Camera.target_square < tl_square) {
        *x = left;
        shift = g_Camera.target_square - l_square;
        if (shift >= 0) {
            shift = Math_Sqrt(shift);
            *y = target_y + (top >= bottom ? shift : -shift);
        }
    } else if (tl_square > MIN_SQUARE) {
        *x = left;
        *y = top;
    } else if (g_Camera.target_square < bl_square) {
        *x = left;
        shift = g_Camera.target_square - l_square;
        if (shift >= 0) {
            shift = Math_Sqrt(shift);
            *y = target_y + (top < bottom ? shift : -shift);
        }
    } else if (2 * g_Camera.target_square < tr_square) {
        shift = 2 * g_Camera.target_square - t_square;
        if (shift >= 0) {
            shift = Math_Sqrt(shift);
            *x = target_x + (left < right ? shift : -shift);
            *y = top;
        }
    } else if (bl_square <= tr_square) {
        *x = right;
        *y = top;
    } else {
        *x = left;
        *y = bottom;
    }
}

const struct FLOOR_INFO *__cdecl Camera_GoodPosition(
    int32_t x, int32_t y, int32_t z, int16_t room_num)
{
    const struct FLOOR_INFO *floor = Room_GetFloor(x, y, z, &room_num);
    int32_t height = Room_GetHeight(floor, x, y, z);
    int32_t ceiling = Room_GetCeiling(floor, x, y, z);
    if (y > height || y < ceiling) {
        return NULL;
    }

    return floor;
}

void __cdecl Camera_SmartShift(
    struct GAME_VECTOR *target,
    void(__cdecl *shift)(
        int32_t *x, int32_t *y, int32_t *h, int32_t target_x, int32_t target_y,
        int32_t target_h, int32_t left, int32_t top, int32_t right,
        int32_t bottom))
{
    LOS_Check(&g_Camera.target, target);

    const struct ROOM_INFO *r = &g_Rooms[g_Camera.target.room_num];
    int32_t x_floor = (g_Camera.target.z - r->z) >> WALL_SHIFT;
    int32_t y_floor = (g_Camera.target.x - r->x) >> WALL_SHIFT;
    int16_t item_box = r->floor[x_floor + y_floor * r->x_size].box;
    const struct BOX_INFO *box = &g_Boxes[item_box];

    int32_t left = (int32_t)box->left << WALL_SHIFT;
    int32_t top = (int32_t)box->top << WALL_SHIFT;
    int32_t right = ((int32_t)box->right << WALL_SHIFT) - 1;
    int32_t bottom = ((int32_t)box->bottom << WALL_SHIFT) - 1;

    r = &g_Rooms[target->room_num];
    x_floor = (target->z - r->z) >> WALL_SHIFT;
    y_floor = (target->x - r->x) >> WALL_SHIFT;
    int16_t camera_box = r->floor[x_floor + y_floor * r->x_size].box;

    if (camera_box != NO_BOX
        && (target->z < left || target->z > right || target->x < top
            || target->x > bottom)) {
        box = &g_Boxes[camera_box];
        left = (int32_t)box->left << WALL_SHIFT;
        top = (int32_t)box->top << WALL_SHIFT;
        right = ((int32_t)box->right << WALL_SHIFT) - 1;
        bottom = ((int32_t)box->bottom << WALL_SHIFT) - 1;
    }

    int32_t test;
    int32_t edge;

    test = (target->z - WALL_L) | (WALL_L - 1);
    const struct FLOOR_INFO *good_left =
        Camera_GoodPosition(target->x, target->y, test, target->room_num);
    if (good_left) {
        camera_box = good_left->box;
        edge = (int32_t)g_Boxes[camera_box].left << WALL_SHIFT;
        if (camera_box != NO_ITEM && edge < left) {
            left = edge;
        }
    } else {
        left = test;
    }

    test = (target->z + WALL_L) & (~(WALL_L - 1));
    const struct FLOOR_INFO *good_right =
        Camera_GoodPosition(target->x, target->y, test, target->room_num);
    if (good_right) {
        camera_box = good_right->box;
        edge = ((int32_t)g_Boxes[camera_box].right << WALL_SHIFT) - 1;
        if (camera_box != NO_ITEM && edge > right) {
            right = edge;
        }
    } else {
        right = test;
    }

    test = (target->x - WALL_L) | (WALL_L - 1);
    const struct FLOOR_INFO *good_top =
        Camera_GoodPosition(test, target->y, target->z, target->room_num);
    if (good_top) {
        camera_box = good_top->box;
        edge = (int32_t)g_Boxes[camera_box].top << WALL_SHIFT;
        if (camera_box != NO_ITEM && edge < top) {
            top = edge;
        }
    } else {
        top = test;
    }

    test = (target->x + WALL_L) & (~(WALL_L - 1));
    const struct FLOOR_INFO *good_bottom =
        Camera_GoodPosition(test, target->y, target->z, target->room_num);
    if (good_bottom) {
        camera_box = good_bottom->box;
        edge = ((int32_t)g_Boxes[camera_box].bottom << WALL_SHIFT) - 1;
        if (camera_box != NO_ITEM && edge > bottom) {
            bottom = edge;
        }
    } else {
        bottom = test;
    }

    left += STEP_L;
    right -= STEP_L;
    top += STEP_L;
    bottom -= STEP_L;

    struct GAME_VECTOR a = {
        .x = target->x,
        .y = target->y,
        .z = target->z,
        .room_num = target->room_num,
    };

    struct GAME_VECTOR b = {
        .x = target->x,
        .y = target->y,
        .z = target->z,
        .room_num = target->room_num,
    };

    int32_t noclip = 1;
    int32_t prefer_a;

    if (ABS(target->z - g_Camera.target.z)
        > ABS(target->x - g_Camera.target.x)) {
        if (target->z < left && !good_left) {
            noclip = 0;
            prefer_a = g_Camera.pos.x < g_Camera.target.x;
            shift(
                &a.z, &a.x, &a.y, g_Camera.target.z, g_Camera.target.x,
                g_Camera.target.y, left, top, right, bottom);
            shift(
                &b.z, &b.x, &b.y, g_Camera.target.z, g_Camera.target.x,
                g_Camera.target.y, left, bottom, right, top);
        } else if (target->z > right && !good_right) {
            noclip = 0;
            prefer_a = g_Camera.pos.x < g_Camera.target.x;
            shift(
                &a.z, &a.x, &a.y, g_Camera.target.z, g_Camera.target.x,
                g_Camera.target.y, right, top, left, bottom);
            shift(
                &b.z, &b.x, &b.y, g_Camera.target.z, g_Camera.target.x,
                g_Camera.target.y, right, bottom, left, top);
        }

        if (noclip) {
            if (target->x < top && !good_top) {
                noclip = 0;
                prefer_a = target->z < g_Camera.target.z;
                shift(
                    &a.x, &a.z, &a.y, g_Camera.target.x, g_Camera.target.z,
                    g_Camera.target.y, top, left, bottom, right);
                shift(
                    &b.x, &b.z, &b.y, g_Camera.target.x, g_Camera.target.z,
                    g_Camera.target.y, top, right, bottom, left);
            } else if (target->x > bottom && !good_bottom) {
                noclip = 0;
                prefer_a = target->z < g_Camera.target.z;
                shift(
                    &a.x, &a.z, &a.y, g_Camera.target.x, g_Camera.target.z,
                    g_Camera.target.y, bottom, left, top, right);
                shift(
                    &b.x, &b.z, &b.y, g_Camera.target.x, g_Camera.target.z,
                    g_Camera.target.y, bottom, right, top, left);
            }
        }
    } else {
        if (target->x < top && !good_top) {
            noclip = 0;
            prefer_a = g_Camera.pos.z < g_Camera.target.z;
            shift(
                &a.x, &a.z, &a.y, g_Camera.target.x, g_Camera.target.z,
                g_Camera.target.y, top, left, bottom, right);
            shift(
                &b.x, &b.z, &b.y, g_Camera.target.x, g_Camera.target.z,
                g_Camera.target.y, top, right, bottom, left);
        } else if (target->x > bottom && !good_bottom) {
            noclip = 0;
            prefer_a = g_Camera.pos.z < g_Camera.target.z;
            shift(
                &a.x, &a.z, &a.y, g_Camera.target.x, g_Camera.target.z,
                g_Camera.target.y, bottom, left, top, right);
            shift(
                &b.x, &b.z, &b.y, g_Camera.target.x, g_Camera.target.z,
                g_Camera.target.y, bottom, right, top, left);
        }

        if (noclip) {
            if (target->z < left && !good_left) {
                noclip = 0;
                prefer_a = target->x < g_Camera.target.x;
                shift(
                    &a.z, &a.x, &a.y, g_Camera.target.z, g_Camera.target.x,
                    g_Camera.target.y, left, top, right, bottom);
                shift(
                    &b.z, &b.x, &b.y, g_Camera.target.z, g_Camera.target.x,
                    g_Camera.target.y, left, bottom, right, top);
            } else if (target->z > right && !good_right) {
                noclip = 0;
                prefer_a = target->x < g_Camera.target.x;
                shift(
                    &a.z, &a.x, &a.y, g_Camera.target.z, g_Camera.target.x,
                    g_Camera.target.y, right, top, left, bottom);
                shift(
                    &b.z, &b.x, &b.y, g_Camera.target.z, g_Camera.target.x,
                    g_Camera.target.y, right, bottom, left, top);
            }
        }
    }

    if (noclip) {
        return;
    }

    if (prefer_a) {
        prefer_a = LOS_Check(&g_Camera.target, &a) != 0;
    } else {
        prefer_a = LOS_Check(&g_Camera.target, &b) == 0;
    }

    if (prefer_a) {
        target->x = a.x;
        target->y = a.y;
        target->z = a.z;
    } else {
        target->x = b.x;
        target->y = b.y;
        target->z = b.z;
    }

    Room_GetFloor(target->x, target->y, target->z, &target->room_num);
}

void __cdecl Camera_Chase(const struct ITEM_INFO *item)
{
    g_Camera.target_elevation += item->pos.x_rot;
    g_Camera.target_elevation = MIN(g_Camera.target_elevation, MAX_ELEVATION);
    g_Camera.target_elevation = MAX(g_Camera.target_elevation, -MAX_ELEVATION);

    int32_t distance =
        (g_Camera.target_distance * Math_Cos(g_Camera.target_elevation))
        >> W2V_SHIFT;
    int16_t angle = g_Camera.target_angle + item->pos.y_rot;

    g_Camera.target_square = SQUARE(distance);

    const struct PHD_VECTOR offset = {
        .y = (g_Camera.target_distance * Math_Sin(g_Camera.target_elevation))
            >> W2V_SHIFT,
        .x = -((distance * Math_Sin(angle)) >> W2V_SHIFT),
        .z = -((distance * Math_Cos(angle)) >> W2V_SHIFT),
    };

    struct GAME_VECTOR target = {
        .x = g_Camera.target.x + offset.x,
        .y = g_Camera.target.y + offset.y,
        .z = g_Camera.target.z + offset.z,
        .room_num = g_Camera.pos.room_num,
    };

    Camera_SmartShift(&target, Camera_Shift);
    Camera_Move(&target, g_Camera.speed);
}

int32_t __cdecl Camera_ShiftClamp(struct GAME_VECTOR *pos, int32_t clamp)
{
    int32_t x = pos->x;
    int32_t y = pos->y;
    int32_t z = pos->z;
    const struct FLOOR_INFO *floor = Room_GetFloor(x, y, z, &pos->room_num);
    const struct BOX_INFO *box = &g_Boxes[floor->box];

    int32_t left = ((int32_t)box->left << WALL_SHIFT) + clamp;
    int32_t right = ((int32_t)box->right << WALL_SHIFT) - clamp - 1;
    if (z < left && !Camera_GoodPosition(x, y, z - clamp, pos->room_num)) {
        pos->z = left;
    } else if (
        z > right && !Camera_GoodPosition(x, y, z + clamp, pos->room_num)) {
        pos->z = right;
    }

    int32_t top = ((int32_t)box->top << WALL_SHIFT) + clamp;
    int32_t bottom = ((int32_t)box->bottom << WALL_SHIFT) - clamp - 1;
    if (x < top && !Camera_GoodPosition(x - clamp, y, z, pos->room_num)) {
        pos->x = top;
    } else if (
        x > bottom && !Camera_GoodPosition(x + clamp, y, z, pos->room_num)) {
        pos->x = bottom;
    }

    int32_t height = Room_GetHeight(floor, x, y, z) - clamp;
    int32_t ceiling = Room_GetCeiling(floor, x, y, z) + clamp;
    if (height < ceiling) {
        ceiling = (height + ceiling) >> 1;
        height = ceiling;
    }

    if (y > height) {
        return height - y;
    }
    if (y < ceiling) {
        return ceiling - y;
    }
    return 0;
}

void __cdecl Camera_Combat(const struct ITEM_INFO *item)
{
    g_Camera.target.z = item->pos.z;
    g_Camera.target.x = item->pos.x;

    g_Camera.target_distance = COMBAT_DISTANCE;
    if (g_Lara.target) {
        g_Camera.target_angle = g_Lara.target_angles[0] + item->pos.y_rot;
        g_Camera.target_elevation = g_Lara.target_angles[1] + item->pos.x_rot;
    } else {
        g_Camera.target_angle =
            g_Lara.torso_y_rot + g_Lara.head_y_rot + item->pos.y_rot;
        g_Camera.target_elevation =
            g_Lara.torso_x_rot + g_Lara.head_x_rot + item->pos.x_rot;
    }

    int32_t distance =
        (COMBAT_DISTANCE * Math_Cos(g_Camera.target_elevation)) >> W2V_SHIFT;
    int16_t angle = g_Camera.target_angle;

    const struct PHD_VECTOR offset = {
        .y =
            +((g_Camera.target_distance * Math_Sin(g_Camera.target_elevation))
              >> W2V_SHIFT),
        .x = -((distance * Math_Sin(angle)) >> W2V_SHIFT),
        .z = -((distance * Math_Cos(angle)) >> W2V_SHIFT),
    };

    struct GAME_VECTOR target = {
        .x = g_Camera.target.x + offset.x,
        .y = g_Camera.target.y + offset.y,
        .z = g_Camera.target.z + offset.z,
        .room_num = g_Camera.pos.room_num,
    };

    if (g_Lara.water_status == LWS_UNDERWATER) {
        int32_t water_height = g_Lara.water_surface_dist + g_LaraItem->pos.y;
        if (g_Camera.target.y > water_height && water_height > target.y) {
            target.y = g_Lara.water_surface_dist + g_LaraItem->pos.y;
            target.z = g_Camera.target.z
                + (water_height - g_Camera.target.y)
                    * (target.z - g_Camera.target.z)
                    / (target.y - g_Camera.target.y);
            target.x = g_Camera.target.x
                + (water_height - g_Camera.target.y)
                    * (target.x - g_Camera.target.x)
                    / (target.y - g_Camera.target.y);
        }
    }

    Camera_SmartShift(&target, Camera_Shift);
    Camera_Move(&target, g_Camera.speed);
}

void __cdecl Camera_Look(const struct ITEM_INFO *item)
{
    struct PHD_VECTOR old = {
        .x = g_Camera.target.x,
        .y = g_Camera.target.y,
        .z = g_Camera.target.z,
    };

    g_Camera.target.z = item->pos.z;
    g_Camera.target.x = item->pos.x;
    g_Camera.target_angle =
        item->pos.y_rot + g_Lara.torso_y_rot + g_Lara.head_y_rot;
    g_Camera.target_distance = LOOK_DISTANCE;
    g_Camera.target_elevation =
        g_Lara.torso_x_rot + g_Lara.head_x_rot + item->pos.x_rot;

    int32_t distance =
        (LOOK_DISTANCE * Math_Cos(g_Camera.target_elevation)) >> W2V_SHIFT;

    g_Camera.shift =
        (-STEP_L * 2 * Math_Sin(g_Camera.target_elevation)) >> W2V_SHIFT;
    g_Camera.target.z +=
        (g_Camera.shift * Math_Cos(item->pos.y_rot)) >> W2V_SHIFT;
    g_Camera.target.x +=
        (g_Camera.shift * Math_Sin(item->pos.y_rot)) >> W2V_SHIFT;

    if (!Camera_GoodPosition(
            g_Camera.target.x, g_Camera.target.y, g_Camera.target.z,
            g_Camera.target.room_num)) {
        g_Camera.target.x = item->pos.x;
        g_Camera.target.z = item->pos.z;
    }

    g_Camera.target.y += Camera_ShiftClamp(&g_Camera.target, LOOK_CLAMP);

    const struct PHD_VECTOR offset = {
        .y =
            +((g_Camera.target_distance * Math_Sin(g_Camera.target_elevation))
              >> W2V_SHIFT),
        .x = -((distance * Math_Sin(g_Camera.target_angle)) >> W2V_SHIFT),
        .z = -((distance * Math_Cos(g_Camera.target_angle)) >> W2V_SHIFT),
    };

    struct GAME_VECTOR target = {
        .x = g_Camera.target.x + offset.x,
        .y = g_Camera.target.y + offset.y,
        .z = g_Camera.target.z + offset.z,
        .room_num = g_Camera.pos.room_num,
    };

    Camera_SmartShift(&target, Camera_Clip);
    g_Camera.target.z = old.z + (g_Camera.target.z - old.z) / g_Camera.speed;
    g_Camera.target.x = old.x + (g_Camera.target.x - old.x) / g_Camera.speed;
    Camera_Move(&target, g_Camera.speed);
}

void __cdecl Camera_Fixed(void)
{
    const struct OBJECT_VECTOR *fixed = &g_Camera.fixed[g_Camera.num];
    struct GAME_VECTOR target = {
        .x = fixed->x,
        .y = fixed->y,
        .z = fixed->z,
        .room_num = fixed->data,
    };
    if (!LOS_Check(&g_Camera.target, &target)) {
        Camera_ShiftClamp(&target, STEP_L);
    }

    g_Camera.fixed_camera = 1;
    Camera_Move(&target, g_Camera.speed);

    if (g_Camera.timer) {
        g_Camera.timer--;
        if (!g_Camera.timer) {
            g_Camera.timer = -1;
        }
    }
}

void __cdecl Camera_Update(void)
{
    if (g_Rooms[g_Camera.pos.room_num].flags & RF_UNDERWATER) {
        Sound_Effect(SFX_UNDERWATER, NULL, SPM_ALWAYS);
        if (!g_Camera.underwater) {
            Music_SetVolume(0);
            g_Camera.underwater = 1;
        }
    } else if (g_Camera.underwater) {
        if (g_OptionMusicVolume) {
            Music_SetVolume(25 * g_OptionMusicVolume + 5);
        }
        g_Camera.underwater = 0;
    }

    if (g_Camera.type == CAM_CINEMATIC) {
        Camera_LoadCutsceneFrame();
        return;
    }

    if (g_Camera.flags != CF_NO_CHUNKY) {
        g_IsChunkyCamera = 1;
    }

    const int32_t fixed_camera = g_Camera.item != NULL
        && (g_Camera.type == CAM_FIXED || g_Camera.type == CAM_HEAVY);
    const struct ITEM_INFO *const item =
        fixed_camera ? g_Camera.item : g_LaraItem;

    const int16_t *bounds = Item_GetBoundsAccurate(item);

    int32_t y = item->pos.y;
    if (fixed_camera) {
        y += (bounds[FBBOX_MIN_Y] + bounds[FBBOX_MAX_Y]) / 2;
    } else {
        y += bounds[FBBOX_MAX_Y]
            + (((int32_t)(bounds[FBBOX_MIN_Y] - bounds[FBBOX_MAX_Y])) * 3 >> 2);
    }

    if (g_Camera.item && !fixed_camera) {
        bounds = Item_GetBoundsAccurate(g_Camera.item);

        const int32_t dx = g_Camera.item->pos.x - item->pos.x;
        const int32_t dz = g_Camera.item->pos.z - item->pos.z;
        const int32_t shift = Math_Sqrt(SQUARE(dx) + SQUARE(dz));
        int16_t angle = Math_Atan(dz, dx) - item->pos.y_rot;

        int16_t tilt = Math_Atan(
            shift,
            y - (bounds[FBBOX_MIN_Y] + bounds[FBBOX_MAX_Y]) / 2
                - g_Camera.item->pos.y);
        angle >>= 1;
        tilt >>= 1;

        if (angle > MIN_HEAD_ROTATION && angle < MAX_HEAD_ROTATION
            && tilt > MIN_HEAD_TILT_CAM && tilt < MAX_HEAD_TILT_CAM) {
            int16_t change = angle - g_Lara.head_y_rot;
            if (change > HEAD_TURN) {
                g_Lara.head_y_rot += HEAD_TURN;
            } else if (change < -HEAD_TURN) {
                g_Lara.head_y_rot -= HEAD_TURN;
            } else {
                g_Lara.head_y_rot = angle;
            }

            change = tilt - g_Lara.head_x_rot;
            if (change > HEAD_TURN) {
                g_Lara.head_x_rot += HEAD_TURN;
            } else if (change < -HEAD_TURN) {
                g_Lara.head_x_rot -= HEAD_TURN;
            } else {
                g_Lara.head_x_rot += change;
            }
            g_Lara.torso_x_rot = g_Lara.head_x_rot;
            g_Lara.torso_y_rot = g_Lara.head_y_rot;
            g_Camera.type = CAM_LOOK;
            g_Camera.item->looked_at = 1;
        }
    }

    if (g_Camera.type == CAM_LOOK || g_Camera.type == CAM_COMBAT) {
        y -= STEP_L;
        g_Camera.target.room_num = item->room_num;
        if (g_Camera.fixed_camera) {
            g_Camera.target.y = y;
            g_Camera.speed = 1;
        } else {
            g_Camera.target.y += (y - g_Camera.target.y) >> 2;
            g_Camera.speed =
                g_Camera.type == CAM_LOOK ? LOOK_SPEED : COMBAT_SPEED;
        }
        g_Camera.fixed_camera = 0;
        if (g_Camera.type == CAM_LOOK) {
            Camera_Look(item);
        } else {
            Camera_Combat(item);
        }
    } else {
        g_Camera.target.x = item->pos.x;
        g_Camera.target.z = item->pos.z;

        if (g_Camera.flags == CF_FOLLOW_CENTRE) {
            const int32_t shift =
                (bounds[FBBOX_MIN_Z] + bounds[FBBOX_MAX_Z]) / 2;
            g_Camera.target.z +=
                (shift * Math_Cos(item->pos.y_rot)) >> W2V_SHIFT;
            g_Camera.target.x +=
                (shift * Math_Sin(item->pos.y_rot)) >> W2V_SHIFT;
        }

        g_Camera.target.room_num = item->room_num;
        if (g_Camera.fixed_camera != fixed_camera) {
            g_Camera.target.y = y;
            g_Camera.fixed_camera = 1;
            g_Camera.speed = 1;
        } else {
            g_Camera.fixed_camera = 0;
            g_Camera.target.y += (y - g_Camera.target.y) / 4;
        }

        const struct FLOOR_INFO *const floor = Room_GetFloor(
            g_Camera.target.x, y, g_Camera.target.z, &g_Camera.target.room_num);
        const int32_t height = Room_GetHeight(
            floor, g_Camera.target.x, g_Camera.target.y, g_Camera.target.z);
        if (g_Camera.target.y > height) {
            g_IsChunkyCamera = 0;
        }

        if (g_Camera.type == CAM_CHASE || g_Camera.flags == CF_CHASE_OBJECT) {
            Camera_Chase(item);
        } else {
            Camera_Fixed();
        }
    }

    g_Camera.last = g_Camera.num;
    g_Camera.fixed_camera = fixed_camera;
    if (g_Camera.type != CAM_HEAVY || g_Camera.timer == -1) {
        g_Camera.type = CAM_CHASE;
        g_Camera.speed = CHASE_SPEED;
        g_Camera.num = NO_CAMERA;
        g_Camera.last_item = g_Camera.item;
        g_Camera.item = NULL;
        g_Camera.target_elevation = 0;
        g_Camera.target_angle = 0;
        g_Camera.target_distance = CHASE_ELEVATION;
        g_Camera.flags = CF_NORMAL;
    }
    g_IsChunkyCamera = 0;
}

void __cdecl Camera_LoadCutsceneFrame(void)
{
    g_CineFrameIdx++;
    if (g_CineFrameIdx >= g_NumCineFrames) {
        g_CineFrameIdx = g_NumCineFrames - 1;
    }

    const struct CINE_FRAME *frame = &g_CineData[g_CineFrameIdx];
    int32_t tx = frame->tx;
    int32_t ty = frame->ty;
    int32_t tz = frame->tz;
    int32_t cx = frame->cx;
    int32_t cy = frame->cy;
    int32_t cz = frame->cz;
    int32_t fov = frame->fov;
    int32_t roll = frame->roll;
    int32_t c = Math_Cos(g_CinePos.y_rot);
    int32_t s = Math_Sin(g_CinePos.y_rot);

    g_Camera.target.x = g_CinePos.x + ((tx * c + tz * s) >> W2V_SHIFT);
    g_Camera.target.y = g_CinePos.y + ty;
    g_Camera.target.z = g_CinePos.z + ((tz * c - tx * s) >> W2V_SHIFT);
    g_Camera.pos.x = g_CinePos.x + ((cx * c + cz * s) >> W2V_SHIFT);
    g_Camera.pos.y = g_CinePos.y + cy;
    g_Camera.pos.z = g_CinePos.z + ((cz * c - cx * s) >> W2V_SHIFT);

    Output_AlterFOV(fov);
    Matrix_LookAt(
        g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z, g_Camera.target.x,
        g_Camera.target.y, g_Camera.target.z, roll);

    Room_GetFloor(
        g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z, &g_Camera.pos.room_num);

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

void __cdecl Camera_UpdateCutscene(void)
{
    const struct CINE_FRAME *frame = &g_CineData[g_CineFrameIdx];
    int32_t tx = frame->tx;
    int32_t ty = frame->ty;
    int32_t tz = frame->tz;
    int32_t cx = frame->cx;
    int32_t cy = frame->cy;
    int32_t cz = frame->cz;
    int32_t fov = frame->fov;
    int32_t roll = frame->roll;
    int32_t c = Math_Cos(g_Camera.target_angle);
    int32_t s = Math_Sin(g_Camera.target_angle);
    const struct PHD_VECTOR camtar = {
        .x = g_LaraItem->pos.x + ((tx * c + tz * s) >> W2V_SHIFT),
        .y = g_LaraItem->pos.y + ty,
        .z = g_LaraItem->pos.z + ((tz * c - tx * s) >> W2V_SHIFT),
    };
    const struct PHD_VECTOR campos = {
        .x = g_LaraItem->pos.x + ((cx * c + cz * s) >> W2V_SHIFT),
        .y = g_LaraItem->pos.y + cy,
        .z = g_LaraItem->pos.z + ((cz * c - cx * s) >> W2V_SHIFT),
    };
    int16_t room_num = Room_FindByPos(campos.x, campos.y, campos.z);
    if (room_num >= 0) {
        g_Camera.pos.room_num = room_num;
    }
    Output_AlterFOV(fov);
    Matrix_LookAt(
        campos.x, campos.y, campos.z, camtar.x, camtar.y, camtar.z, roll);
}
