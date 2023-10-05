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
#define GAME_FOV 80
#define MIN_SQUARE SQUARE(WALL_L / 3)
#define NO_BOX (-1)
#define NO_ITEM (-1)
#define NO_CAMERA (-1)
#define HEAD_TURN (4 * PHD_DEGREE) // = 728
#define MAX_HEAD_ROTATION (50 * PHD_DEGREE) // = 9100
#define MIN_HEAD_ROTATION (-MAX_HEAD_ROTATION) // = -9100
#define MAX_HEAD_TILT_CAM (85 * PHD_DEGREE) // = 15470
#define MIN_HEAD_TILT_CAM (-MAX_HEAD_TILT_CAM) // = -15470

#define MAX_AUDIO_SAMPLE_BUFFERS 256
#define MAX_AUDIO_SAMPLE_TRACKS 32
