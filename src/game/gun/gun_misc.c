#include "game/gun/gun_misc.h"

#include "game/items.h"
#include "game/los.h"
#include "game/math.h"
#include "game/math_misc.h"
#include "game/matrix.h"
#include "game/random.h"
#include "game/room.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

#define NEAR_ANGLE (PHD_DEGREE * 15) // = 2730

void __cdecl Gun_TargetInfo(const WEAPON_INFO *const winfo)
{
    if (!g_Lara.target) {
        g_Lara.left_arm.lock = 0;
        g_Lara.right_arm.lock = 0;
        g_Lara.target_angles[0] = 0;
        g_Lara.target_angles[1] = 0;
        return;
    }

    GAME_VECTOR start;
    start.pos.x = g_LaraItem->pos.x;
    start.pos.y = g_LaraItem->pos.y - 650;
    start.pos.z = g_LaraItem->pos.z;
    start.room_num = g_LaraItem->room_num;

    GAME_VECTOR target;
    Gun_FindTargetPoint(g_Lara.target, &target);

    int16_t angles[2];
    // clang-format off
    Math_GetVectorAngles(
        target.pos.x - start.pos.x,
        target.pos.y - start.pos.y,
        target.pos.z - start.pos.z,
        angles);
    // clang-format on

    angles[0] -= g_LaraItem->rot.y;
    angles[1] -= g_LaraItem->rot.x;

    if (!LOS_Check(&start, &target)) {
        g_Lara.right_arm.lock = 0;
        g_Lara.left_arm.lock = 0;
    } else if (
        angles[0] >= winfo->lock_angles[0] && angles[0] <= winfo->lock_angles[1]
        && angles[1] >= winfo->lock_angles[2]
        && angles[1] <= winfo->lock_angles[3]) {
        g_Lara.right_arm.lock = 1;
        g_Lara.left_arm.lock = 1;
    } else {
        if (g_Lara.left_arm.lock
            && (angles[0] < winfo->left_angles[0]
                || angles[0] > winfo->left_angles[1]
                || angles[1] < winfo->left_angles[2]
                || angles[1] > winfo->left_angles[3])) {
            g_Lara.left_arm.lock = 0;
        }
        if (g_Lara.right_arm.lock
            && (angles[0] < winfo->right_angles[0]
                || angles[0] > winfo->right_angles[1]
                || angles[1] < winfo->right_angles[2]
                || angles[1] > winfo->right_angles[3])) {
            g_Lara.right_arm.lock = 0;
        }
    }

    g_Lara.target_angles[0] = angles[0];
    g_Lara.target_angles[1] = angles[1];
}

void __cdecl Gun_GetNewTarget(const WEAPON_INFO *const winfo)
{
    GAME_VECTOR start;
    start.pos.x = g_LaraItem->pos.x;
    start.pos.y = g_LaraItem->pos.y - 650;
    start.pos.z = g_LaraItem->pos.z;
    start.room_num = g_LaraItem->room_num;

    int16_t best_y_rot = 0x7FFF;
    int32_t best_dist = 0x7FFFFFFF;
    ITEM_INFO *best_target = NULL;

    const int16_t max_dist = winfo->target_dist;
    for (int32_t i = 0; i < NUM_SLOTS; i++) {
        const int16_t item_num = g_BaddieSlots[i].item_num;
        if (item_num == NO_ITEM || item_num == g_Lara.item_num) {
            continue;
        }

        ITEM_INFO *const item = &g_Items[item_num];
        if (item->hit_points <= 0) {
            continue;
        }

        const int32_t dx = item->pos.x - start.pos.x;
        const int32_t dy = item->pos.y - start.pos.y;
        const int32_t dz = item->pos.z - start.pos.z;
        if (ABS(dx) > max_dist || ABS(dy) > max_dist || ABS(dz) > max_dist) {
            continue;
        }

        const int32_t dist = SQUARE(dz) + SQUARE(dy) + SQUARE(dx);
        if (dist >= SQUARE(max_dist)) {
            continue;
        }

        GAME_VECTOR target;
        Gun_FindTargetPoint(item, &target);
        if (!LOS_Check(&start, &target)) {
            continue;
        }

        int16_t angles[2];
        Math_GetVectorAngles(
            target.pos.x - start.pos.x, target.pos.y - start.pos.y,
            target.pos.z - start.pos.z, angles);
        angles[0] -= g_Lara.torso_y_rot + g_LaraItem->rot.y;
        angles[1] -= g_Lara.torso_x_rot + g_LaraItem->rot.x;

        if (angles[0] >= winfo->lock_angles[0]
            && angles[0] <= winfo->lock_angles[1]
            && angles[1] >= winfo->lock_angles[2]
            && angles[1] <= winfo->lock_angles[3]) {
            const int16_t y_rot = ABS(angles[0]);
            if (y_rot < best_y_rot + NEAR_ANGLE && dist < best_dist) {
                best_dist = dist;
                best_y_rot = y_rot;
                best_target = item;
            }
        }
    }

    g_Lara.target = best_target;
    Gun_TargetInfo(winfo);
}

int32_t __cdecl Gun_FireWeapon(
    const LARA_GUN_TYPE weapon_type, ITEM_INFO *const target,
    const ITEM_INFO *const src, const PHD_ANGLE *const angles)
{
    const WEAPON_INFO *const winfo = &g_Weapons[weapon_type];

    AMMO_INFO *ammo;
    switch (weapon_type) {
    case LGT_MAGNUMS:
        ammo = &g_Lara.magnum_ammo;
        if (g_SaveGame.bonus_flag) {
            ammo->ammo = 1000;
        }
        break;

    case LGT_UZIS:
        ammo = &g_Lara.uzi_ammo;
        if (g_SaveGame.bonus_flag) {
            ammo->ammo = 1000;
        }
        break;

    case LGT_SHOTGUN:
        ammo = &g_Lara.shotgun_ammo;
        if (g_SaveGame.bonus_flag) {
            ammo->ammo = 1000;
        }
        break;

    case LGT_M16:
        ammo = &g_Lara.m16_ammo;
        if (g_SaveGame.bonus_flag) {
            ammo->ammo = 1000;
        }
        break;

    default:
        ammo = &g_Lara.pistol_ammo;
        ammo->ammo = 1000;
        break;
    }

    if (ammo->ammo <= 0) {
        ammo->ammo = 0;
        return 0;
    }

    ammo->ammo--;

    const PHD_3DPOS view = {
        .pos = {
            .x = src->pos.x,
            .y = src->pos.y - winfo->gun_height,
            .z = src->pos.z,
        },
        .rot = {
            .x = angles[1] + winfo->shot_accuracy * (Random_GetControl() - PHD_90) / PHD_ONE,
            .y = angles[0] + winfo->shot_accuracy * (Random_GetControl() - PHD_90) / PHD_ONE,
            .z = 0,
        },
    };
    Matrix_GenerateW2V(&view);

    SPHERE spheres[33];
    int32_t sphere_count = Collide_GetSpheres(target, spheres, false);
    int32_t best_sphere = -1;
    int32_t best_dist = 0x7FFFFFFF;

    for (int32_t i = 0; i < sphere_count; i++) {
        const SPHERE *const sphere = &spheres[i];
        const int32_t r = sphere->r;
        if (ABS(sphere->x) < r && ABS(sphere->y) < r && sphere->z > r
            && SQUARE(sphere->x) + SQUARE(sphere->y) <= SQUARE(r)) {
            const int32_t dist = sphere->z - r;
            if (dist < best_dist) {
                best_dist = dist;
                best_sphere = i;
            }
        }
    }

    g_SaveGame.statistics.shots++;

    GAME_VECTOR start;
    start.pos.x = view.pos.x;
    start.pos.y = view.pos.y;
    start.pos.z = view.pos.z;
    start.room_num = src->room_num;

    if (best_sphere < 0) {
        const int32_t dist = winfo->target_dist;
        GAME_VECTOR hit_pos;
        hit_pos.pos.x = view.pos.x + ((dist * g_MatrixPtr->_20) >> W2V_SHIFT);
        hit_pos.pos.y = view.pos.y + ((dist * g_MatrixPtr->_21) >> W2V_SHIFT);
        hit_pos.pos.z = view.pos.z + ((dist * g_MatrixPtr->_22) >> W2V_SHIFT);
        hit_pos.room_num =
            Room_GetIndexFromPos(hit_pos.pos.x, hit_pos.pos.y, hit_pos.pos.z);
        const bool object_on_los = LOS_Check(&start, &hit_pos);
        const int16_t item_to_smash = LOS_CheckSmashable(&start, &hit_pos);
        if (item_to_smash == NO_ITEM) {
            if (!object_on_los) {
                Richochet(&hit_pos);
            }
            return -1;
        } else {
            Gun_SmashItem(item_to_smash, weapon_type);
            return -1;
        }
    } else {
        g_SaveGame.statistics.hits++;
        GAME_VECTOR hit_pos;
        hit_pos.pos.x =
            view.pos.x + ((best_dist * g_MatrixPtr->_20) >> W2V_SHIFT);
        hit_pos.pos.y =
            view.pos.y + ((best_dist * g_MatrixPtr->_21) >> W2V_SHIFT);
        hit_pos.pos.z =
            view.pos.z + ((best_dist * g_MatrixPtr->_22) >> W2V_SHIFT);
        hit_pos.room_num =
            Room_GetIndexFromPos(hit_pos.pos.x, hit_pos.pos.y, hit_pos.pos.z);
        const int16_t item_to_smash = LOS_CheckSmashable(&start, &hit_pos);
        if (item_to_smash != NO_ITEM) {
            Gun_SmashItem(item_to_smash, weapon_type);
        }
        Gun_HitTarget(target, &hit_pos, winfo->damage);
        return 1;
    }
}

void __cdecl Gun_FindTargetPoint(
    const ITEM_INFO *const item, GAME_VECTOR *const target)
{
    const BOUNDS_16 *const bounds = &Item_GetBestFrame(item)->bounds;
    const int32_t x = bounds->min_x + (bounds->max_x - bounds->min_x) / 2;
    const int32_t y = bounds->min_y + (bounds->max_y - bounds->min_y) / 3;
    const int32_t z = bounds->min_z + (bounds->max_z - bounds->min_z) / 2;
    const int32_t cy = Math_Cos(item->rot.y);
    const int32_t sy = Math_Sin(item->rot.y);
    target->pos.x = item->pos.x + ((cy * x + sy * z) >> W2V_SHIFT);
    target->pos.y = item->pos.y + y;
    target->pos.z = item->pos.z + ((cy * z - sy * x) >> W2V_SHIFT);
    target->room_num = item->room_num;
}
