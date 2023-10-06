#pragma once

#include <ddraw.h>
#include <ddrawi.h>
#include <d3dhal.h>
#include <dsound.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

#pragma pack(push, 1)

typedef DDSURFACEDESC DDSDESC, *LPDDSDESC;
typedef LPDIRECTDRAWSURFACE3 LPDDS;
typedef D3DTEXTUREHANDLE HWR_TEXHANDLE;

typedef int16_t PHD_ANGLE;

typedef struct MATRIX {
    int32_t _00;
    int32_t _01;
    int32_t _02;
    int32_t _03;
    int32_t _10;
    int32_t _11;
    int32_t _12;
    int32_t _13;
    int32_t _20;
    int32_t _21;
    int32_t _22;
    int32_t _23;
} MATRIX;

typedef struct PHD_3DPOS {
    int32_t x;
    int32_t y;
    int32_t z;
    int16_t x_rot;
    int16_t y_rot;
    int16_t z_rot;
} PHD_3DPOS;

typedef enum VGA_MODE {
    VGA_NoVga,
    VGA_256Color,
    VGA_ModeX,
    VGA_Standard,
} VGA_MODE;

typedef struct BITMAP_RESOURCE {
    LPBITMAPINFO bmp_info;
    void *bmp_data;
    HPALETTE hPalette;
    DWORD flags;
} BITMAP_RESOURCE;

typedef struct POS_3D {
    int16_t x;
    int16_t y;
    int16_t z;
} POS_3D;

typedef struct DISPLAY_MODE {
    int32_t width;
    int32_t height;
    int32_t bpp;
    VGA_MODE vga;
} DISPLAY_MODE;

typedef struct DISPLAY_MODE_NODE {
    struct DISPLAY_MODE_NODE *next;
    struct DISPLAY_MODE_NODE *previous;
    struct DISPLAY_MODE body;
} DISPLAY_MODE_NODE;

typedef struct DISPLAY_MODE_LIST {
    struct DISPLAY_MODE_NODE *head;
    struct DISPLAY_MODE_NODE *tail;
    DWORD count;
} DISPLAY_MODE_LIST;

typedef struct STRING_FLAGGED {
    char *content;
    bool is_valid;
} STRING_FLAGGED;

typedef struct DISPLAY_ADAPTER {
    LPGUID adapter_guid_ptr;
    GUID adapter_guid;
    STRING_FLAGGED driver_description;
    STRING_FLAGGED driver_name;
    DDCAPS driver_caps;
    DDCAPS hel_caps;
    GUID device_guid;
    D3DDEVICEDESC device_desc;
    DISPLAY_MODE_LIST hw_disp_mode_list;
    DISPLAY_MODE_LIST sw_disp_mode_list;
    DISPLAY_MODE vga_mode1;
    DISPLAY_MODE vga_mode2;
    DWORD screen_width;
    bool hw_render_supported;
    bool sw_windowed_supported;
    bool hw_windowed_supported;
    bool is_vga_mode1_presented;
    bool is_vga_mode2_presented;
    bool perspective_correct_supported;
    bool dither_supported;
    bool zbuffer_supported;
    bool linear_filter_supported;
    bool shade_restricted;
} DISPLAY_ADAPTER;

typedef struct DISPLAY_ADAPTER_NODE {
    struct DISPLAY_ADAPTER_NODE *next;
    struct DISPLAY_ADAPTER_NODE *previous;
    struct DISPLAY_ADAPTER body;
} DISPLAY_ADAPTER_NODE;

typedef struct DISPLAY_ADAPTER_LIST {
    struct DISPLAY_ADAPTER_NODE *head;
    struct DISPLAY_ADAPTER_NODE *tail;
    DWORD count;
} DISPLAY_ADAPTER_LIST;

typedef struct SOUND_ADAPTER {
    GUID *adapter_guid_ptr;
    GUID adapter_guid;
    STRING_FLAGGED description;
    STRING_FLAGGED module;
} SOUND_ADAPTER;

typedef struct SOUND_ADAPTER_NODE {
    struct SOUND_ADAPTER_NODE *next;
    struct SOUND_ADAPTER_NODE *previous;
    SOUND_ADAPTER body;
} SOUND_ADAPTER_NODE;

typedef struct SOUND_ADAPTER_LIST {
    struct SOUND_ADAPTER_NODE *head;
    struct SOUND_ADAPTER_NODE *tail;
    DWORD count;
} SOUND_ADAPTER_LIST;

typedef struct JOYSTICK {
    GUID *lpJoystickGuid;
    GUID joystickGuid;
    STRING_FLAGGED productName;
    STRING_FLAGGED instanceName;
} JOYSTICK;

typedef struct JOYSTICK_NODE {
    struct JOYSTICK_NODE *next;
    struct JOYSTICK_NODE *previous;
    JOYSTICK body;
} JOYSTICK_NODE;

typedef struct JOYSTICK_LIST {
    struct JOYSTICK_LIST *head;
    struct JOYSTICK_LIST *tail;
    DWORD count;
} JOYSTICK_LIST;

typedef enum RENDER_MODE {
    RM_Unknown,
    RM_Software,
    RM_Hardware,
} RENDER_MODE;

typedef enum ASPECT_MODE {
    AM_4_3,
    AM_16_9,
    AM_Any,
} ASPECT_MODE;

typedef enum TEX_ADJUST_MODE {
    TAM_Disabled,
    TAM_BilinearOnly,
    TAM_Always,
} TEX_ADJUST_MODE;

typedef struct APP_SETTINGS {
    struct DISPLAY_ADAPTER_NODE *PreferredDisplayAdapter;
    struct SOUND_ADAPTER_NODE *PreferredSoundAdapter;
    struct JOYSTICK_NODE *PreferredJoystick;
    struct DISPLAY_MODE_NODE *VideoMode;
    enum RENDER_MODE RenderMode;
    int32_t WindowWidth;
    int32_t WindowHeight;
    enum ASPECT_MODE AspectMode;
    bool PerspectiveCorrect;
    bool Dither;
    bool ZBuffer;
    bool BilinearFiltering;
    bool TripleBuffering;
    bool FullScreen;
    bool SoundEnabled;
    bool LaraMic;
    bool JoystickEnabled;
    bool Disable16BitTextures;
    bool DontSortPrimitives;
    bool FlipBroken;
    bool DisableFMV;
    TEX_ADJUST_MODE TexelAdjustMode;
    int32_t NearestAdjustment;
    int32_t LinearAdjustment;
} APP_SETTINGS;

struct TEXPAGE_DESC {
    LPDDS sysMemSurface;
    LPDDS vidMemSurface;
    LPDIRECTDRAWPALETTE palette;
    LPDIRECT3DTEXTURE2 texture3d;
    HWR_TEXHANDLE texHandle;
    int32_t width;
    int32_t height;
    int32_t status;
};

typedef struct RGB888 {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RGB888;

typedef struct ColorBitMasks_t {
    DWORD dwRBitMask;
    DWORD dwGBitMask;
    DWORD dwBBitMask;
    DWORD dwRGBAlphaBitMask;
    DWORD dwRBitDepth;
    DWORD dwGBitDepth;
    DWORD dwBBitDepth;
    DWORD dwRGBAlphaBitDepth;
    DWORD dwRBitOffset;
    DWORD dwGBitOffset;
    DWORD dwBBitOffset;
    DWORD dwRGBAlphaBitOffset;
} COLOR_BIT_MASKS;

typedef struct GOURAUD_FILL {
    D3DCOLOR clr[4][4];
} GOURAUD_FILL;

typedef struct GOURAUD_OUTLINE {
    D3DCOLOR clr[9];
} GOURAUD_OUTLINE;

typedef struct DEPTHQ_ENTRY {
    uint8_t index[256];
} DEPTHQ_ENTRY;

typedef struct GOURAUD_ENTRY {
    uint8_t index[32];
} GOURAUD_ENTRY;

typedef struct PHD_VECTOR {
    int32_t x;
    int32_t y;
    int32_t z;
} PHD_VECTOR;

typedef struct SPHERE {
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t r;
} SPHERE;

typedef struct TEXTSTRING {
    uint32_t flags;
    uint16_t text_flags;
    uint16_t bgnd_flags;
    uint16_t outl_flags;
    int16_t xpos;
    int16_t ypos;
    int16_t zpos;
    int16_t letter_spacing;
    int16_t word_spacing;
    int16_t flash_rate;
    int16_t flash_count;
    int16_t bgnd_colour;
    uint16_t *bgnd_gour;
    int16_t outl_colour;
    uint16_t *outl_gour;
    int16_t bgnd_size_x;
    int16_t bgnd_size_y;
    int16_t bgnd_off_x;
    int16_t bgnd_off_y;
    int16_t bgnd_off_z;
    int32_t scale_h;
    int32_t scale_v;
    char *string;
} TEXTSTRING;

typedef struct PHD_VBUF {
    float xv;
    float yv;
    float zv;
    float rhw;
    float xs;
    float ys;
    int16_t clip;
    int16_t g;
    int16_t u;
    int16_t v;
} PHD_VBUF;

typedef struct PHD_UV {
    uint16_t u;
    uint16_t v;
} PHD_UV;

typedef struct PHD_TEXTURE {
    uint16_t draw_type;
    uint16_t tex_page;
    PHD_UV uv[4];
} PHD_TEXTURE;

typedef struct POINT_INFO {
    float xv;
    float yv;
    float zv;
    float rhw;
    float xs;
    float ys;
    float u;
    float v;
    float g;
} POINT_INFO;

typedef struct VERTEX_INFO {
    float x;
    float y;
    float rhw;
    float u;
    float v;
    float g;
} VERTEX_INFO;

typedef enum KEYMAP {
    KM_Forward,
    KM_Back,
    KM_Left,
    KM_Right,
    KM_StepLeft,
    KM_StepRight,
    KM_Slow,
    KM_Jump,
    KM_Action,
    KM_WeaponDraw,
    KM_Flare,
    KM_Look,
    KM_Roll,
    KM_Option,
} KEYMAP;

typedef struct REQUEST_INFO {
    uint16_t req_flags;
    uint16_t items_count;
    uint16_t selected;
    uint16_t visible_count;
    uint16_t line_offset;
    uint16_t line_old_offset;
    uint16_t pix_width;
    uint16_t line_height;
    int16_t x_pos;
    int16_t y_pos;
    int16_t z_pos;
    uint16_t item_string_len;
    char *pitem_strings1;
    char *pitem_strings2;
    uint32_t *pitem_flags1;
    uint32_t *pitem_flags2;
    uint32_t heading_flags1;
    uint32_t heading_flags2;
    uint32_t background_flags;
    uint32_t moreup_flags;
    uint32_t moredown_flags;
    uint32_t item_flags1[24];
    uint32_t item_flags2[24];
    struct TEXTSTRING *heading_text1;
    struct TEXTSTRING *heading_text2;
    struct TEXTSTRING *background_text;
    struct TEXTSTRING *moreup_text;
    struct TEXTSTRING *moredown_text;
    struct TEXTSTRING *item_texts1[24];
    struct TEXTSTRING *item_texts2[24];
    char heading_string1[32];
    char heading_string2[32];
    uint32_t render_width;
    uint32_t render_height;
} REQUEST_INFO;

typedef enum SORT_TYPE {
    ST_AVG_Z,
    ST_MAX_Z,
    ST_FAR_Z,
} SORT_TYPE;

typedef struct COLL_SIDE {
    int32_t floor;
    int32_t ceiling;
    int32_t type;
} COLL_SIDE;

typedef struct COLL_INFO {
    struct COLL_SIDE side_mid;
    struct COLL_SIDE side_front;
    struct COLL_SIDE side_left;
    struct COLL_SIDE side_right;
    int32_t radius;
    int32_t bad_pos;
    int32_t bad_neg;
    int32_t bad_ceiling;
    struct PHD_VECTOR shift;
    struct PHD_VECTOR old;
    int16_t old_anim_state;
    int16_t old_anim_num;
    int16_t old_frame_num;
    int16_t facing;
    int16_t quadrant;
    int16_t coll_type;
    int16_t *trigger;
    int8_t x_tilt;
    int8_t z_tilt;
    int8_t hit_by_baddie;
    int8_t hit_static;
    uint16_t slopes_are_walls : 2;
    uint16_t slopes_are_pits : 1;
    uint16_t lava_is_pit : 1;
    uint16_t enable_baddie_push : 1;
    uint16_t enable_spaz : 1;
    uint16_t hit_ceiling : 1;
} COLL_INFO;

typedef struct ITEM_INFO {
    int32_t floor;
    uint32_t touch_bits;
    uint32_t mesh_bits;
    int16_t object_num;
    int16_t current_anim_state;
    int16_t goal_anim_state;
    int16_t required_anim_state;
    int16_t anim_num;
    int16_t frame_num;
    int16_t room_num;
    int16_t next_item;
    int16_t next_active;
    int16_t speed;
    int16_t fall_speed;
    int16_t hit_points;
    int16_t box_num;
    int16_t timer;
    uint16_t flags;
    int16_t shade1;
    int16_t shade2;
    int16_t carried_item;
    void *data;
    struct PHD_3DPOS pos;
    uint16_t active : 1;
    uint16_t status : 2;
    uint16_t gravity : 1;
    uint16_t hit_status : 1;
    uint16_t collidable : 1;
    uint16_t looked_at : 1;
    uint16_t dynamic_light : 1;
    uint16_t clear_body : 1;
    uint16_t pad : 7;
} ITEM_INFO;

typedef struct FLOOR_INFO {
    int16_t idx;
    int16_t box;
    int8_t pit_room;
    int8_t floor;
    int8_t sky_room;
    int8_t ceiling;
} FLOOR_INFO;

typedef struct WEAPON_INFO {
    int16_t lock_angles[4];
    int16_t left_angles[4];
    int16_t right_angles[4];
    int16_t aim_speed;
    int16_t shot_accuracy;
    int32_t gun_height;
    int32_t damage;
    int32_t target_dist;
    int16_t recoil_frame;
    int16_t flash_time;
    int16_t sample_num;
} WEAPON_INFO;

typedef struct FX_INFO {
    struct PHD_3DPOS pos;
    int16_t room_num;
    int16_t object_num;
    int16_t next_fx;
    int16_t next_active;
    int16_t speed;
    int16_t fallspeed;
    int16_t frame_num;
    int16_t counter;
    int16_t shade;
} FX_INFO;

typedef struct AI_INFO {
    int16_t zone_num;
    int16_t enemy_zone;
    int32_t distance;
    int32_t ahead;
    int32_t bite;
    int16_t angle;
    int16_t enemy_facing;
} AI_INFO;

typedef struct BOX_NODE {
    int16_t exit_box;
    uint16_t search_num;
    int16_t next_expansion;
    int16_t box_num;
} BOX_NODE;

typedef struct LOT_INFO {
    struct BOX_NODE *node;
    int16_t head;
    int16_t tail;
    uint16_t search_num;
    uint16_t block_mask;
    int16_t step;
    int16_t drop;
    int16_t fly;
    int16_t zone_count;
    int16_t target_box;
    int16_t required_box;
    struct PHD_VECTOR target;
} LOT_INFO;

typedef enum GF_LEVEL_TYPE {
    GFL_NOLEVEL = -1,
    GFL_TITLE,
    GFL_NORMAL,
    GFL_SAVED,
    GFL_DEMO,
    GFL_CUTSCENE,
    GFL_STORY,
    GFL_QUIET,
    GFL_MIDSTORY,
} GF_LEVEL_TYPE;

typedef struct INVENTORY_ITEM {
    char *string;
    int16_t obj_num;
    int16_t frames_total;
    int16_t current_frame;
    int16_t goal_frame;
    int16_t open_frame;
    int16_t anim_direction;
    int16_t anim_speed;
    int16_t anim_count;
    int16_t x_rot_pt_sel;
    int16_t x_rot_pt;
    int16_t x_rot_sel;
    int16_t x_rot;
    int16_t y_rot_sel;
    int16_t y_rot;
    int16_t z_rot;
    int32_t y_trans_sel;
    int32_t y_trans;
    int32_t z_trans_sel;
    int32_t z_trans;
    int32_t meshes_sel;
    int32_t meshes_drawn;
    int16_t inv_pos;
    void *sprites;
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t reserved3;
    uint32_t reserved4;
} INVENTORY_ITEM;

typedef enum GF_EVENTS {
    GFE_PICTURE,
    GFE_LIST_START,
    GFE_LIST_END,
    GFE_PLAYFMV,
    GFE_STARTLEVEL,
    GFE_CUTSCENE,
    GFE_LEVCOMPLETE,
    GFE_DEMOPLAY,
    GFE_JUMPTO_SEQ,
    GFE_END_SEQ,
    GFE_SETTRACK,
    GFE_SUNSET,
    GFE_LOADINGPIC,
    GFE_DEADLY_WATER,
    GFE_REMOVE_WEAPONS,
    GFE_GAMECOMPLETE,
    GFE_CUTANGLE,
    GFE_NOFLOOR,
    GFE_ADD2INV,
    GFE_STARTANIM,
    GFE_NUMSECRETS,
    GFE_KILL2COMPLETE,
    GFE_REMOVE_AMMO,
} GF_EVENTS;

typedef enum MOOD_TYPE {
    MOOD_BORED = 0,
    MOOD_ATTACK = 1,
    MOOD_ESCAPE = 2,
    MOOD_STALK = 3,
} MOOD_TYPE;

typedef enum TARGET_TYPE {
    TARGET_NONE = 0,
    TARGET_PRIMARY = 1,
    TARGET_SECONDARY = 2,
} TARGET_TYPE;

typedef struct BITE_INFO {
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t mesh_idx;
} BITE_INFO;

typedef struct ANIM_STRUCT {
    int16_t *frame_ptr;
    int16_t interpolation;
    int16_t current_anim_state;
    int32_t velocity;
    int32_t acceleration;
    int16_t frame_base;
    int16_t frame_end;
    int16_t jump_anim_num;
    int16_t jump_frame_num;
    int16_t num_changes;
    int16_t change_idx;
    int16_t num_commands;
    int16_t command_idx;
} ANIM_STRUCT;

typedef struct DOOR_INFO {
    int16_t room;
    int16_t x;
    int16_t y;
    int16_t z;
    struct POS_3D vertex[4];
} DOOR_INFO;

typedef struct DOOR_INFOS {
    int16_t count;
    struct DOOR_INFO door[];
} DOOR_INFOS;

typedef struct LIGHT_INFO {
    int32_t x;
    int32_t y;
    int32_t z;
    int16_t intensity1;
    int16_t intensity2;
    int32_t falloff1;
    int32_t falloff2;
} LIGHT_INFO;

typedef struct MESH_INFO {
    int32_t x;
    int32_t y;
    int32_t z;
    int16_t y_rot;
    int16_t shade1;
    int16_t shade2;
    int16_t static_num;
} MESH_INFO;

typedef enum ROOM_FLAG {
    RF_UNDERWATER = 0x01,
    RF_OUTSIDE = 0x08,
    RF_INSIDE = 0x40,
} ROOM_FLAG;

typedef struct ROOM_INFO {
    int16_t *data;
    struct DOOR_INFOS *doors;
    struct FLOOR_INFO *floor;
    struct LIGHT_INFO *light;
    struct MESH_INFO *mesh;
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t min_floor;
    int32_t max_ceiling;
    int16_t x_size;
    int16_t y_size;
    int16_t ambient1;
    int16_t ambient2;
    int16_t light_mode;
    int16_t num_lights;
    int16_t num_meshes;
    int16_t bound_left;
    int16_t bound_right;
    int16_t bound_top;
    int16_t bound_bottom;
    uint16_t bound_active;
    int16_t left;
    int16_t right;
    int16_t top;
    int16_t bottom;
    int16_t item_num;
    int16_t fx_num;
    int16_t flipped_room;
    uint16_t flags;
} ROOM_INFO;

typedef struct CREATURE_INFO {
    int16_t head_rotation;
    int16_t neck_rotation;
    int16_t maximum_turn;
    uint16_t flags;
    int16_t item_num;
    enum MOOD_TYPE mood;
    struct LOT_INFO lot;
    struct PHD_VECTOR target;
    struct ITEM_INFO *enemy;
} CREATURE_INFO;

typedef enum CAMERA_TYPE {
    CAM_CHASE = 0,
    CAM_FIXED = 1,
    CAM_LOOK = 2,
    CAM_COMBAT = 3,
    CAM_CINEMATIC = 4,
    CAM_HEAVY = 5,
} CAMERA_TYPE;

typedef struct GAME_VECTOR {
    int32_t x;
    int32_t y;
    int32_t z;
    int16_t room_num;
    int16_t box_num;
} GAME_VECTOR;

typedef struct OBJECT_VECTOR {
    int32_t x;
    int32_t y;
    int32_t z;
    int16_t data;
    int16_t flags;
} OBJECT_VECTOR;

typedef struct BOX_INFO {
    uint8_t left;
    uint8_t right;
    uint8_t top;
    uint8_t bottom;
    uint16_t height;
    uint16_t overlap_index;
} BOX_INFO;

typedef struct CAMERA_INFO {
    struct GAME_VECTOR pos;
    struct GAME_VECTOR target;
    enum CAMERA_TYPE type;
    int32_t shift;
    uint32_t flags;
    int32_t fixed_camera;
    int32_t num_frames;
    int32_t bounce;
    int32_t underwater;
    int32_t target_distance;
    int32_t target_square;
    int16_t target_angle;
    int16_t actual_angle;
    int16_t target_elevation;
    int16_t box;
    int16_t num;
    int16_t last;
    int16_t timer;
    int16_t speed;
    struct ITEM_INFO *item;
    struct ITEM_INFO *last_item;
    struct OBJECT_VECTOR *fixed;
    int32_t is_lara_mic;
    struct PHD_VECTOR mic_pos;
} CAMERA_INFO;

typedef struct LARA_ARM {
    int16_t *frame_base;
    int16_t frame_num;
    int16_t anim_num;
    int16_t lock;
    int16_t y_rot;
    int16_t x_rot;
    int16_t z_rot;
    int16_t flash_gun;
} LARA_ARM;

typedef struct AMMO_INFO {
    int32_t ammo;
} AMMO_INFO;

typedef enum LARA_WATER_STATE {
    LWS_ABOVE_WATER = 0,
    LWS_UNDERWATER = 1,
    LWS_SURFACE = 2,
    LWS_CHEAT = 3,
    LWS_WADE = 4,
} LARA_WATER_STATE;

typedef struct LARA_INFO {
    int16_t item_num;
    int16_t gun_status;
    int16_t gun_type;
    int16_t request_gun_type;
    int16_t last_gun_type;
    int16_t calc_fallspeed;
    int16_t water_status;
    int16_t climb_status;
    int16_t pose_count;
    int16_t hit_frame;
    int16_t hit_direction;
    int16_t air;
    int16_t dive_count;
    int16_t death_count;
    int16_t current_active;
    int16_t spaz_effect_count;
    int16_t flare_age;
    int16_t skidoo;
    int16_t weapon_item;
    int16_t back_gun;
    int16_t flare_frame;
    uint16_t flare_control_left : 1;
    uint16_t flare_control_right : 1;
    uint16_t extra_anim : 1;
    uint16_t look : 1;
    uint16_t burn : 1;
    uint16_t keep_ducked : 1;
    uint16_t can_monkey_swing : 1;
    uint16_t pad : 9;
    int32_t water_surface_dist;
    struct PHD_VECTOR last_pos;
    struct FX_INFO *spaz_effect;
    uint32_t mesh_effects;
    int16_t *mesh_ptrs[15];
    struct ITEM_INFO *target;
    int16_t target_angles[2];
    int16_t turn_rate;
    int16_t move_angle;
    int16_t head_y_rot;
    int16_t head_x_rot;
    int16_t head_z_rot;
    int16_t torso_y_rot;
    int16_t torso_x_rot;
    int16_t torso_z_rot;
    struct LARA_ARM left_arm;
    struct LARA_ARM right_arm;
    struct AMMO_INFO pistol_ammo;
    struct AMMO_INFO magnum_ammo;
    struct AMMO_INFO uzi_ammo;
    struct AMMO_INFO shotgun_ammo;
    struct AMMO_INFO harpoon_ammo;
    struct AMMO_INFO grenade_ammo;
    struct AMMO_INFO m16_ammo;
    struct CREATURE_INFO *creature;
} LARA_INFO;

typedef enum SOUND_EFFECT_ID {
    SFX_LARA_FALL = 30,
    SFX_UNDERWATER = 60,
} SOUND_EFFECT_ID;

typedef enum SOUND_PLAY_MODE {
    SPM_NORMAL = 0,
    SPM_UNDERWATER = 1,
    SPM_ALWAYS = 2,
} SOUND_PLAY_MODE;

typedef enum CAMERA_FLAGS {
    CF_NORMAL = 0,
    CF_FOLLOW_CENTRE = 1,
    CF_NO_CHUNKY = 2,
    CF_CHASE_OBJECT = 3,
} CAMERA_FLAGS;

typedef enum FRAME_BBOX_INFO {
    FBBOX_MIN_X = 0,
    FBBOX_MAX_X = 1,
    FBBOX_MIN_Y = 2,
    FBBOX_MAX_Y = 3,
    FBBOX_MIN_Z = 4,
    FBBOX_MAX_Z = 5,
    FBBOX_X = 6,
    FBBOX_Y = 7,
    FBBOX_Z = 8,
} FRAME_BBOX_INFO;

typedef struct CINE_FRAME {
    int16_t tx;
    int16_t ty;
    int16_t tz;
    int16_t cx;
    int16_t cy;
    int16_t cz;
    int16_t fov;
    int16_t roll;
} CINE_FRAME;

typedef enum INPUT_STATE {
    IN_FORWARD = 0x00000001,
    IN_BACK = 0x00000002,
    IN_LEFT = 0x00000004,
    IN_RIGHT = 0x00000008,
    IN_JUMP = 0x00000010,
    IN_DRAW = 0x00000020,
    IN_ACTION = 0x00000040,
    IN_SLOW = 0x00000080,
    IN_OPTION = 0x00000100,
    IN_LOOK = 0x00000200,
    IN_STEP_LEFT = 0x00000400,
    IN_STEP_RIGHT = 0x00000800,
    IN_ROLL = 0x00001000,
    IN_PAUSE = 0x00002000,
    IN_RESERVED1 = 0x00004000,
    IN_RESERVED2 = 0x00008000,
    IN_DOZY_CHEAT = 0x00010000,
    IN_STUFF_CHEAT = 0x00020000,
    IN_DEBUG_INFO = 0x00040000,
    IN_FLARE = 0x00080000,
    IN_SELECT = 0x00100000,
    IN_DESELECT = 0x00200000,
    IN_SAVE = 0x00400000,
    IN_LOAD = 0x00800000,
} INPUT_STATE;

typedef enum LARA_ANIMATION {
    LA_RUN = 0,
    LA_WALK_FORWARD = 1,
    LA_RUN_START = 6,
    LA_WALK_BACK = 40,
    LA_VAULT_12 = 50,
    LA_VAULT_34 = 42,
    LA_FAST_FALL = 32,
    LA_STOP = 11,
    LA_FALL_DOWN = 34,
    LA_STOP_LEFT = 2,
    LA_STOP_RIGHT = 3,
    LA_HIT_WALL_LEFT = 53,
    LA_HIT_WALL_RIGHT = 54,
    LA_RUN_STEP_UP_LEFT = 56,
    LA_RUN_STEP_UP_RIGHT = 55,
    LA_WALK_STEP_UP_LEFT = 57,
    LA_WALK_STEP_UP_RIGHT = 58,
    LA_WALK_STEP_DOWN_LEFT = 60,
    LA_WALK_STEP_DOWN_RIGHT = 59,
    LA_BACK_STEP_DOWN_LEFT = 61,
    LA_BACK_STEP_DOWN_RIGHT = 62,
    LA_WALL_SWITCH_DOWN = 63,
    LA_WALL_SWITCH_UP = 64,
    LA_SIDE_STEP_LEFT = 65,
    LA_SIDE_STEP_RIGHT = 67,
    LA_LAND_FAR = 24,
    LA_GRAB_LEDGE = 96,
    LA_SWIM_GLIDE = 87,
    LA_FALL_BACK = 93,
    LA_HANG = 96,
    LA_STOP_HANG = 28,
    LA_SLIDE = 70,
    LA_SLIDE_BACK = 104,
    LA_TREAD = 108,
    LA_SURF_TREAD = 114,
    LA_SURF_DIVE = 119,
    LA_SURF_CLIMB = 111,
    LA_JUMP_IN = 112,
    LA_ROLL = 146,
    LA_PICKUP_UW = 130,
    LA_PICKUP = 135,
    LA_ROLLING_BALL_DEATH = 139,
    LA_SPIKE_DEATH = 149,
    LA_GRAB_LEDGE_IN = 150,
    LA_SPAZ_FORWARD = 125,
    LA_SPAZ_BACK = 126,
    LA_SPAZ_RIGHT = 127,
    LA_SPAZ_LEFT = 128,
} LARA_ANIMATION;

typedef enum LARA_STATE {
    LS_WALK = 0,
    LS_RUN = 1,
    LS_STOP = 2,
    LS_FORWARD_JUMP = 3,
    LS_POSE = 4,
    LS_FAST_BACK = 5,
    LS_TURN_RIGHT = 6,
    LS_TURN_LEFT = 7,
    LS_DEATH = 8,
    LS_FAST_FALL = 9,
    LS_HANG = 10,
    LS_REACH = 11,
    LS_SPLAT = 12,
    LS_TREAD = 13,
    LS_LAND = 14,
    LS_COMPRESS = 15,
    LS_BACK = 16,
    LS_SWIM = 17,
    LS_GLIDE = 18,
    LS_NULL = 19,
    LS_FAST_TURN = 20,
    LS_STEP_RIGHT = 21,
    LS_STEP_LEFT = 22,
    LS_HIT = 23,
    LS_SLIDE = 24,
    LS_BACK_JUMP = 25,
    LS_RIGHT_JUMP = 26,
    LS_LEFT_JUMP = 27,
    LS_UP_JUMP = 28,
    LS_FALL_BACK = 29,
    LS_HANG_LEFT = 30,
    LS_HANG_RIGHT = 31,
    LS_SLIDE_BACK = 32,
    LS_SURF_TREAD = 33,
    LS_SURF_SWIM = 34,
    LS_DIVE = 35,
    LS_PUSH_BLOCK = 36,
    LS_PULL_BLOCK = 37,
    LS_PP_READY = 38,
    LS_PICKUP = 39,
    LS_SWITCH_ON = 40,
    LS_SWITCH_OFF = 41,
    LS_USE_KEY = 42,
    LS_USE_PUZZLE = 43,
    LS_UW_DEATH = 44,
    LS_ROLL = 45,
    LS_SPECIAL = 46,
    LS_SURF_BACK = 47,
    LS_SURF_LEFT = 48,
    LS_SURF_RIGHT = 49,
    LS_USE_MIDAS = 50,
    LS_DIE_MIDAS = 51,
    LS_SWAN_DIVE = 52,
    LS_FAST_DIVE = 53,
    LS_GYMNAST = 54,
    LS_WATER_OUT = 55,
    LS_CLIMB_STANCE = 56,
    LS_CLIMBING = 57,
    LS_CLIMB_LEFT = 58,
    LS_CLIMB_END = 59,
    LS_CLIMB_RIGHT = 60,
    LS_CLIMB_DOWN = 61,
    LS_LARA_TEST1 = 62,
    LS_LARA_TEST2 = 63,
    LS_LARA_TEST3 = 64,
    LS_WADE = 65,
    LS_WATER_ROLL = 66,
    LS_FLARE_PICKUP = 67,
    LS_TWIST = 68,
    LS_KICK = 69,
    LS_DEATH_SLIDE = 70,
    LS_DUCK = 71,
    LS_DUCK_ROLL = 72,
    LS_DASH = 73,
    LS_DASH_DIVE = 74,
    LS_MONKEY_SWING = 75,
    LS_MONKEYF = 76,
    LS_LAST = 77,
} LARA_STATE;

typedef enum LARA_GUN_STATE {
    LGS_ARMLESS = 0,
    LGS_HANDS_BUSY = 1,
    LGS_DRAW = 2,
    LGS_UNDRAW = 3,
    LGS_READY = 4,
} LARA_GUN_STATE;
