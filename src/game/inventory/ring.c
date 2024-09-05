#include "game/inventory/ring.h"

#include "global/funcs.h"
#include "global/vars.h"

#define RING_OPEN_FRAMES 32
#define RING_OPEN_ROTATION PHD_180
#define RING_OPEN_ROTATION PHD_180
#define RING_RADIUS 688
#define RING_CAMERA_START_HEIGHT (-1536)
#define RING_CAMERA_HEIGHT (-256)

void __cdecl Inv_Ring_Init(
    RING_INFO *const ring, const RING_TYPE type, INVENTORY_ITEM **const list,
    const int16_t qty, const int16_t current, IMOTION_INFO *const imo)
{
    ring->type = type;
    ring->list = list;
    ring->number_of_objects = qty;
    ring->current_object = current;
    ring->radius = 0;
    ring->angle_adder = 0x10000 / qty;

    if (g_Inv_Mode == INV_TITLE_MODE) {
        ring->camera_pitch = 1024;
    } else {
        ring->camera_pitch = 0;
    }

    ring->rotating = 0;
    ring->rot_count = 0;
    ring->target_object = 0;
    ring->rot_adder = 0;
    ring->rot_adder_l = 0;
    ring->rot_adder_r = 0;

    ring->imo = imo;

    ring->camera.pos.x = 0;
    ring->camera.pos.y = RING_CAMERA_START_HEIGHT;
    ring->camera.pos.z = 896;
    ring->camera.rot.x = 0;
    ring->camera.rot.y = 0;
    ring->camera.rot.z = 0;

    Inv_Ring_MotionInit(ring, RING_OPEN_FRAMES, RNG_OPENING, RNG_OPEN);
    Inv_Ring_MotionRadius(ring, RING_RADIUS);
    Inv_Ring_MotionCameraPos(ring, RING_CAMERA_HEIGHT);
    Inv_Ring_MotionRotation(
        ring, RING_OPEN_ROTATION,
        -PHD_90 - ring->current_object * ring->angle_adder);

    ring->ring_pos.pos.x = 0;
    ring->ring_pos.pos.y = 0;
    ring->ring_pos.pos.z = 0;
    ring->ring_pos.rot.x = 0;
    ring->ring_pos.rot.y = imo->rotate_target + RING_OPEN_ROTATION;
    ring->ring_pos.rot.z = 0;

    ring->light.x = -1536;
    ring->light.y = 256;
    ring->light.z = 1024;
}
