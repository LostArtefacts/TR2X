#define PHD_ONE 0x10000
#define PHD_DEGREE (PHD_ONE / 360) // = 182
#define PHD_360 (PHD_ONE) // = 65536 = 0x10000
#define PHD_180 (PHD_ONE / 2) // = 32768 = 0x8000
#define PHD_90 (PHD_ONE / 4) // = 16384 = 0x4000
#define PHD_45 (PHD_ONE / 8) // = 8192 = 0x2000
#define PHD_135 (PHD_45 * 3) // = 24576 = 0x6000

#define W2V_SHIFT 14
#define WALL_L 1024
#define WALL_SHIFT 10
#define STEP_L (WALL_L / 4)
#define NO_HEIGHT (-32512)
#define NO_BAD_POS (-NO_HEIGHT) // = 32512
#define BAD_JUMP_CEILING ((STEP_L * 3) / 4) // = 192
#define STEPUP_HEIGHT ((STEP_L * 3) / 2) // = 384

#define GAME_FOV 80
#define MIN_SQUARE SQUARE(WALL_L / 3)
#define NO_BOX (-1)
#define NO_ITEM (-1)
#define NO_CAMERA (-1)

#define MAX_AUDIO_SAMPLE_BUFFERS 256
#define MAX_AUDIO_SAMPLE_TRACKS 32

#define DAMAGE_START 140
#define DAMAGE_LENGTH 14

#define HEAD_TURN (4 * PHD_DEGREE) // = 728
#define MAX_HEAD_ROTATION (50 * PHD_DEGREE) // = 9100
#define MIN_HEAD_ROTATION (-MAX_HEAD_ROTATION) // = -9100
#define MAX_HEAD_TILT_CAM (85 * PHD_DEGREE) // = 15470
#define MIN_HEAD_TILT_CAM (-MAX_HEAD_TILT_CAM) // = -15470
#define FAST_FALL_SPEED 128
#define LARA_TURN_UNDO (2 * PHD_DEGREE) // = 364
#define LARA_TURN_RATE ((PHD_DEGREE / 4) + LARA_TURN_UNDO) // = 409
#define LARA_MED_TURN ((PHD_DEGREE * 4) + LARA_TURN_UNDO) // = 1092
#define LARA_SLOW_TURN ((PHD_DEGREE * 2) + LARA_TURN_UNDO) // = 728
#define LARA_FAST_TURN ((PHD_DEGREE * 6) + LARA_TURN_UNDO) // = 1456
#define LARA_LEAN_UNDO PHD_DEGREE
#define LARA_LEAN_RATE 273
#define LARA_LEAN_MAX ((10 * PHD_DEGREE) + LARA_LEAN_UNDO) // = 2002
#define LARA_JUMP_TURN ((PHD_DEGREE * 1) + LARA_TURN_UNDO) // = 546
#define LARA_FAST_FALL_SPEED (FAST_FALL_SPEED + 3) // = 131

#define CAM_HANG_ANGLE 0
#define CAM_HANG_ELEVATION (-60 * PHD_DEGREE) // = -10920
#define CAM_REACH_ANGLE (85 * PHD_DEGREE) // = 15470
#define CAM_SLIDE_ELEVATION (45 * PHD_DEGREE) // = -8190
#define CAM_BACK_JUMP_ANGLE (135 * PHD_DEGREE) // = 24570
#define CAM_PUSH_BLOCK_ANGLE (35 * PHD_DEGREE) // = 6370
#define CAM_PUSH_BLOCK_ELEVATION (-25 * PHD_DEGREE) // = -4550
#define CAM_PP_READY_ANGLE (75 * PHD_DEGREE) // = 13650
#define CAM_PICKUP_ANGLE (-130 * PHD_DEGREE) // = -23660
#define CAM_PICKUP_ELEVATION (-15 * PHD_DEGREE) // = -2730
#define CAM_PICKUP_DISTANCE WALL_L // = 1024
#define CAM_SWITCH_ON_ANGLE (80 * PHD_DEGREE) // = 14560
#define CAM_SWITCH_ON_ELEVATION (-25 * PHD_DEGREE) // = -4550
#define CAM_SWITCH_ON_DISTANCE WALL_L // = 1024
#define CAM_SWITCH_ON_SPEED 6
#define CAM_USE_KEY_ANGLE (-CAM_SWITCH_ON_ANGLE) // = -14560
#define CAM_USE_KEY_ELEVATION CAM_SWITCH_ON_ELEVATION // = -4550
#define CAM_USE_KEY_DISTANCE WALL_L // = 1024
#define CAM_SPECIAL_ANGLE (170 * PHD_DEGREE) // = 30940
#define CAM_SPECIAL_ELEVATION (-25 * PHD_DEGREE) // = -4550
#define CAM_WADE_ELEVATION (-22 * PHD_DEGREE) // = -4004
#define CAM_DEATH_SLIDE_ANGLE (70 * PHD_DEGREE) // = 12740
#define CAM_YETI_KILL_ANGLE (160 * PHD_DEGREE) // = 29120
#define CAM_YETI_KILL_DISTANCE (3 * WALL_L) // = 3072
#define CAM_SHARK_KILL_ANGLE (160 * PHD_DEGREE) // = 29120
#define CAM_SHARK_KILL_DISTANCE (3 * WALL_L) // = 3072
#define CAM_AIRLOCK_ANGLE (80 * PHD_DEGREE) // = 14560
#define CAM_AIRLOCK_ELEVATION (-25 * PHD_DEGREE) // = -4550
#define CAM_GONG_BONG_ANGLE (-25 * PHD_DEGREE) // = -4550
#define CAM_GONG_BONG_ELEVATION (-20 * PHD_DEGREE) // = -3640
#define CAM_GONG_BONG_DISTANCE (3 * WALL_L) // = 3072
#define CAM_DINO_KILL_ANGLE (170 * PHD_DEGREE) // = 30940
#define CAM_DINO_KILL_ELEVATION (-25 * PHD_DEGREE) // = -4550
#define CAM_CLIMB_LEFT_ANGLE (-30 * PHD_DEGREE) // = -5460
#define CAM_CLIMB_LEFT_ELEVATION (-15 * PHD_DEGREE) // = -2730
#define CAM_CLIMB_RIGHT_ANGLE (-CAM_CLIMB_LEFT_ANGLE) // = 5460
#define CAM_CLIMB_RIGHT_ELEVATION CAM_CLIMB_LEFT_ELEVATION // = -2730
#define CAM_CLIMB_STANCE_ELEVATION (-20 * PHD_DEGREE) // = -3640
#define CAM_CLIMBING_ELEVATION (30 * PHD_DEGREE) // = 5460
#define CAM_CLIMB_END_ELEVATION (45 * PHD_DEGREE) // = -8190
