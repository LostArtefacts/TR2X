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

typedef struct OBJECT_INFO {
    int16_t mesh_count;
    int16_t mesh_idx;
    int32_t bone_idx;
    int16_t *frame_base;

    void (*initialise)(int16_t item_number);
    void (*control)(int16_t item_number);
    void (*floor)(
        struct ITEM_INFO *item, int32_t x, int32_t y, int32_t z,
        int32_t *height);
    void (*ceiling)(
        struct ITEM_INFO *item, int32_t x, int32_t y, int32_t z,
        int32_t *height);
    void (*draw_routine)(struct ITEM_INFO *item);
    void (*collision)(
        int16_t item_num, struct ITEM_INFO *lara_item, struct COLL_INFO *coll);

    int16_t anim_idx;
    int16_t hit_points;
    int16_t pivot_length;
    int16_t radius;
    int16_t shadow_size;

    uint16_t loaded : 1;
    uint16_t intelligent : 1;
    uint16_t non_lot : 1;
    uint16_t save_position : 1;
    uint16_t save_hitpoints : 1;
    uint16_t save_flags : 1;
    uint16_t save_anim : 1;
    uint16_t semi_transparent : 1;
    uint16_t water_creature : 1;
} OBJECT_INFO;

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
    LA_BREATH = 103,
    LA_HANG_UP = 187, // climb ladders up
    LA_HANG_DOWN = 188, // climb ladders down
    LA_CLIMB_STANCE = 164,
    LA_CLIMBING = 161,
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

typedef enum LARA_MESH {
    LM_HIPS = 0,
    LM_THIGH_L = 1,
    LM_CALF_L = 2,
    LM_FOOT_L = 3,
    LM_THIGH_R = 4,
    LM_CALF_R = 5,
    LM_FOOT_R = 6,
    LM_TORSO = 7,
    LM_UARM_R = 8,
    LM_LARM_R = 9,
    LM_HAND_R = 10,
    LM_UARM_L = 11,
    LM_LARM_L = 12,
    LM_HAND_L = 13,
    LM_HEAD = 14,
    LM_NUMBER_OF = 15,
} LARA_MESH;

// clang-format off
typedef enum {
    O_NONE                  = -1,
    O_LARA                  = 0,
    O_LARA_PISTOLS          = 1,
    O_LARA_HAIR             = 2,
    O_LARA_SHOTGUN          = 3,
    O_LARA_MAGNUMS          = 4,
    O_LARA_UZIS             = 5,
    O_LARA_M16              = 6,
    O_LARA_GRENADE          = 7,
    O_LARA_HARPOON          = 8,
    O_LARA_FLARE            = 9,
    O_LARA_SKIDOO           = 10,
    O_LARA_BOAT             = 11,
    O_LARA_EXTRA            = 12,
    O_SKIDOO_FAST           = 13,
    O_BOAT                  = 14,
    O_DOG                   = 15,
    O_CULT_1                = 16,
    O_CULT_1A               = 17,
    O_CULT_1B               = 18,
    O_CULT_2                = 19,
    O_CULT_3                = 20,
    O_MOUSE                 = 21,
    O_DRAGON_FRONT          = 22,
    O_DRAGON_BACK           = 23,
    O_GONDOLA               = 24,
    O_SHARK                 = 25,
    O_EEL                   = 26,
    O_BIG_EEL               = 27,
    O_BARRACUDA             = 28,
    O_DIVER                 = 29,
    O_WORKER_1              = 30,
    O_WORKER_2              = 31,
    O_WORKER_3              = 32,
    O_WORKER_4              = 33,
    O_WORKER_5              = 34,
    O_JELLY                 = 35,
    O_SPIDER_or_WOLF        = 36,
    O_BIG_SPIDER_or_BEAR    = 37,
    O_CROW                  = 38,
    O_TIGER                 = 39,
    O_BARTOLI               = 40,
    O_XIAN_LORD             = 41,
    O_CHINESE_2             = 42,
    O_WARRIOR               = 43,
    O_CHINESE_4             = 44,
    O_YETI                  = 45,
    O_GIANT_YETI            = 46,
    O_EAGLE                 = 47,
    O_BANDIT_1              = 48,
    O_BANDIT_2              = 49,
    O_BANDIT_2B             = 50,
    O_SKIDOO_ARMED          = 51,
    O_SKIDMAN               = 52,
    O_MONK_1                = 53,
    O_MONK_2                = 54,
    O_FALLING_BLOCK_1       = 55,
    O_FALLING_BLOCK_2       = 56,
    O_FALLING_BLOCK_3       = 57,
    O_PENDULUM_1            = 58,
    O_SPIKES                = 59,
    O_ROLLING_BALL_1        = 60,
    O_DARTS                 = 61,
    O_DART_EMITTER          = 62,
    O_DRAW_BRIDGE           = 63,
    O_TEETH_TRAP            = 64,
    O_LIFT                  = 65,
    O_GENERAL               = 66,
    O_MOVABLE_BLOCK_1       = 67,
    O_MOVABLE_BLOCK_2       = 68,
    O_MOVABLE_BLOCK_3       = 69,
    O_MOVABLE_BLOCK_4       = 70,
    O_BIG_BOWL              = 71,
    O_WINDOW_1              = 72,
    O_WINDOW_2              = 73,
    O_WINDOW_3              = 74,
    O_WINDOW_4              = 75,
    O_PROPELLER_1           = 76,
    O_PROPELLER_2           = 77,
    O_HOOK                  = 78,
    O_FALLING_CEILING       = 79,
    O_SPINNING_BLADE        = 80,
    O_BLADE                 = 81,
    O_KILLER_STATUE         = 82,
    O_ROLLING_BALL_2        = 83,
    O_ICICLE                = 84,
    O_SPIKE_WALL            = 85,
    O_SPRING_BOARD          = 86,
    O_CEILING_SPIKES        = 87,
    O_BELL                  = 88,
    O_WATER_SPRITE          = 89,
    O_SNOW_SPRITE           = 90,
    O_SKIDOO_LARA           = 91,
    O_SWITCH_TYPE_1         = 92,
    O_SWITCH_TYPE_2         = 93,
    O_PROPELLER_3           = 94,
    O_PROPELLER_4           = 95,
    O_PENDULUM_2            = 96,
    O_MESH_SWAP_1           = 97,
    O_MESH_SWAP_2           = 98,
    O_LARA_SWAP             = 99,
    O_TEXT_BOX              = 100,
    O_ROLLING_BALL_3        = 101,
    O_DEATH_SLIDE           = 102,
    O_SWITCH_TYPE_3         = 103,
    O_SWITCH_TYPE_4         = 104,
    O_SWITCH_TYPE_5         = 105,
    O_DOOR_TYPE_1           = 106,
    O_DOOR_TYPE_2           = 107,
    O_DOOR_TYPE_3           = 108,
    O_DOOR_TYPE_4           = 109,
    O_DOOR_TYPE_5           = 110,
    O_DOOR_TYPE_6           = 111,
    O_DOOR_TYPE_7           = 112,
    O_DOOR_TYPE_8           = 113,
    O_TRAPDOOR_TYPE_1       = 114,
    O_TRAPDOOR_TYPE_2       = 115,
    O_TRAPDOOR_TYPE_3       = 116,
    O_BRIDGE_FLAT           = 117,
    O_BRIDGE_TILT_1         = 118,
    O_BRIDGE_TILT_2         = 119,
    O_PASSPORT_OPTION       = 120,
    O_COMPASS_OPTION        = 121,
    O_PHOTO_OPTION          = 122,
    O_PLAYER_1              = 123,
    O_PLAYER_2              = 124,
    O_PLAYER_3              = 125,
    O_PLAYER_4              = 126,
    O_PLAYER_5              = 127,
    O_PLAYER_6              = 128,
    O_PLAYER_7              = 129,
    O_PLAYER_8              = 130,
    O_PLAYER_9              = 131,
    O_PLAYER_10             = 132,
    O_PASSPORT_CLOSED       = 133,
    O_COMPASS_ITEM          = 134,
    O_PISTOL_ITEM           = 135,
    O_SHOTGUN_ITEM          = 136,
    O_MAGNUM_ITEM           = 137,
    O_UZI_ITEM              = 138,
    O_HARPOON_ITEM          = 139,
    O_M16_ITEM              = 140,
    O_GRENADE_ITEM          = 141,
    O_PISTOL_AMMO_ITEM      = 142,
    O_SHOTGUN_AMMO_ITEM     = 143,
    O_MAGNUM_AMMO_ITEM      = 144,
    O_UZI_AMMO_ITEM         = 145,
    O_HARPOON_AMMO_ITEM     = 146,
    O_M16_AMMO_ITEM         = 147,
    O_GRENADE_AMMO_ITEM     = 148,
    O_SMALL_MEDIPACK_ITEM   = 149,
    O_LARGE_MEDIPACK_ITEM   = 150,
    O_FLARES_ITEM           = 151,
    O_FLARE_ITEM            = 152,
    O_DETAIL_OPTION         = 153,
    O_SOUND_OPTION          = 154,
    O_CONTROL_OPTION        = 155,
    O_GAMMA_OPTION          = 156,
    O_PISTOL_OPTION         = 157,
    O_SHOTGUN_OPTION        = 158,
    O_MAGNUM_OPTION         = 159,
    O_UZI_OPTION            = 160,
    O_HARPOON_OPTION        = 161,
    O_M16_OPTION            = 162,
    O_GRENADE_OPTION        = 163,
    O_PISTOL_AMMO_OPTION    = 164,
    O_SHOTGUN_AMMO_OPTION   = 165,
    O_MAGNUM_AMMO_OPTION    = 166,
    O_UZI_AMMO_OPTION       = 167,
    O_HARPOON_AMMO_OPTION   = 168,
    O_M16_AMMO_OPTION       = 169,
    O_GRENADE_AMMO_OPTION   = 170,
    O_SMALL_MEDIPACK_OPTION = 171,
    O_LARGE_MEDIPACK_OPTION = 172,
    O_FLARES_OPTION         = 173,
    O_PUZZLE_ITEM_1         = 174,
    O_PUZZLE_ITEM_2         = 175,
    O_PUZZLE_ITEM_3         = 176,
    O_PUZZLE_ITEM_4         = 177,
    O_PUZZLE_OPTION_1       = 178,
    O_PUZZLE_OPTION_2       = 179,
    O_PUZZLE_OPTION_3       = 180,
    O_PUZZLE_OPTION_4       = 181,
    O_PUZZLE_HOLE_1         = 182,
    O_PUZZLE_HOLE_2         = 183,
    O_PUZZLE_HOLE_3         = 184,
    O_PUZZLE_HOLE_4         = 185,
    O_PUZZLE_DONE_1         = 186,
    O_PUZZLE_DONE_2         = 187,
    O_PUZZLE_DONE_3         = 188,
    O_PUZZLE_DONE_4         = 189,
    O_SECRET_1              = 190,
    O_SECRET_2              = 191,
    O_SECRET_3              = 192,
    O_KEY_ITEM_1            = 193,
    O_KEY_ITEM_2            = 194,
    O_KEY_ITEM_3            = 195,
    O_KEY_ITEM_4            = 196,
    O_KEY_OPTION_1          = 197,
    O_KEY_OPTION_2          = 198,
    O_KEY_OPTION_3          = 199,
    O_KEY_OPTION_4          = 200,
    O_KEY_HOLE_1            = 201,
    O_KEY_HOLE_2            = 202,
    O_KEY_HOLE_3            = 203,
    O_KEY_HOLE_4            = 204,
    O_PICKUP_ITEM_1         = 205,
    O_PICKUP_ITEM_2         = 206,
    O_PICKUP_OPTION_1       = 207,
    O_PICKUP_OPTION_2       = 208,
    O_SPHERE_OF_DOOM_1      = 209,
    O_SPHERE_OF_DOOM_2      = 210,
    O_SPHERE_OF_DOOM_3      = 211,
    O_ALARM_SOUND           = 212,
    O_BIRD_TWEETER_1        = 213,
    O_DINO                  = 214,
    O_BIRD_TWEETER_2        = 215,
    O_CLOCK_CHIMES          = 216,
    O_DRAGON_BONES_1        = 217,
    O_DRAGON_BONES_2        = 218,
    O_DRAGON_BONES_3        = 219,
    O_HOT_LIQUID            = 220,
    O_BOAT_BITS             = 221,
    O_MINE                  = 222,
    O_INV_BACKGROUND        = 223,
    O_FX_RESERVED           = 224,
    O_GONG_BONGER           = 225,
    O_DETONATOR_1           = 226,
    O_DETONATOR_2           = 227,
    O_COPTER                = 228,
    O_EXPLOSION             = 229,
    O_SPLASH                = 230,
    O_BUBBLES               = 231,
    O_BUBBLE_EMITTER        = 232,
    O_BLOOD                 = 233,
    O_DART_EFFECT           = 234,
    O_FLARE_FIRE            = 235,
    O_GLOW                  = 236,
    O_GLOW_RESERVED         = 237,
    O_RICOCHET              = 238,
    O_TWINKLE               = 239,
    O_GUN_FLASH             = 240,
    O_M16_FLASH             = 241,
    O_BODY_PART             = 242,
    O_CAMERA_TARGET         = 243,
    O_WATERFALL             = 244,
    O_MISSILE_HARPOON       = 245,
    O_MISSILE_FLAME         = 246,
    O_MISSILE_KNIFE         = 247,
    O_ROCKET                = 248,
    O_HARPOON_BOLT          = 249,
    O_LAVA                  = 250,
    O_LAVA_EMITTER          = 251,
    O_FLAME                 = 252,
    O_FLAME_EMITTER         = 253,
    O_SKYBOX                = 254,
    O_ALPHABET              = 255,
    O_DYING_MONK            = 256,
    O_DING_DONG             = 257,
    O_LARA_ALARM            = 258,
    O_MINI_COPTER           = 259,
    O_WINSTON               = 260,
    O_ASSAULT_DIGITS        = 261,
    O_FINAL_LEVEL_COUNTER   = 262,
    O_CUT_SHOTGUN           = 263,
    O_EARTHQUAKE            = 264,
    O_NUMBER_OF             = 265,
} GAME_OBJECT_ID;
// clang-format on

// clang-format off
typedef enum MUSIC_TRACK_ID {
    MX_INACTIVE                = -1,
    MX_UNUSED_0                = 0, // 2.mp3
    MX_UNUSED_1                = 1, // 2.mp3
    MX_CUTSCENE_THE_GREAT_WALL = 2, // 2.mp3
    MX_UNUSED_2                = 3, // 2.mp3
    MX_CUTSCENE_OPERA_HOUSE    = 4, // 3.mp3
    MX_CUTSCENE_BROTHER_CHAN   = 5, // 4.mp3
    MX_GYM_HINT_1              = 6, // 5.mp3
    MX_GYM_HINT_2              = 7, // 6.mp3
    MX_GYM_HINT_3              = 8, // 7.mp3
    MX_GYM_HINT_4              = 9, // 8.mp3
    MX_GYM_HINT_5              = 10, // 9.mp3
    MX_GYM_HINT_6              = 11, // 10.mp3
    MX_GYM_HINT_7              = 12, // 11.mp3
    MX_GYM_HINT_8              = 13, // 12.mp3
    MX_GYM_HINT_9              = 14, // 13.mp3
    MX_GYM_HINT_10             = 15, // 14.mp3
    MX_GYM_HINT_11             = 16, // 15.mp3
    MX_GYM_HINT_12             = 17, // 16.mp3
    MX_GYM_HINT_13             = 18, // 17.mp3
    MX_GYM_HINT_14             = 19, // 18.mp3
    MX_UNUSED_3                = 20, // 18.mp3
    MX_UNUSED_4                = 21, // 18.mp3
    MX_GYM_HINT_15             = 22, // 19.mp3
    MX_GYM_HINT_16             = 23, // 20.mp3
    MX_GYM_HINT_17             = 24, // 21.mp3
    MX_GYM_HINT_18             = 25, // 22.mp3
    MX_UNUSED_5                = 26, // 23.mp3
    MX_CUTSCENE_BATH           = 27, // 23.mp3
    MX_DAGGER_PULL             = 28, // 24.mp3
    MX_GYM_HINT_20             = 29, // 25.mp3
    MX_CUTSCENE_XIAN           = 30, // 26.mp3
    MX_CAVES_AMBIENCE          = 31, // 27.mp3
    MX_SEWERS_AMBIENCE         = 32, // 28.mp3
    MX_WINDY_AMBIENCE          = 33, // 29.mp3
    MX_HEARTBEAT_AMBIENCE      = 34, // 30.mp3
    MX_SURPRISE_1              = 35, // 31.mp3
    MX_SURPRISE_2              = 36, // 32.mp3
    MX_SURPRISE_3              = 37, // 33.mp3
    MX_OOH_AAH_1               = 38, // 34.mp3
    MX_OOH_AAH_2               = 39, // 35.mp3
    MX_VENICE_VIOLINS          = 40, // 36.mp3
    MX_END_OF_LEVEL            = 41, // 37.mp3
    MX_SPOOKY_1                = 42, // 38.mp3
    MX_SPOOKY_2                = 43, // 39.mp3
    MX_SPOOKY_3                = 44, // 40.mp3
    MX_HARP_THEME              = 45, // 41.mp3
    MX_MYSTERY_1               = 46, // 42.mp3
    MX_SECRET                  = 47, // 43.mp3
    MX_AMBUSH_1                = 48, // 44.mp3
    MX_AMBUSH_2                = 49, // 45.mp3
    MX_AMBUSH_3                = 50, // 46.mp3
    MX_AMBUSH_4                = 51, // 47.mp3
    MX_SKIDOO_THEME            = 52, // 48.mp3
    MX_BATTLE_THEME            = 53, // 49.mp3
    MX_MYSTERY_2               = 54, // 50.mp3
    MX_MYSTERY_3               = 55, // 51.mp3
    MX_MYSTERY_4               = 56, // 52.mp3
    MX_MYSTERY_5               = 57, // 53.mp3
    MX_RIG_AMBIENCE            = 58, // 54.mp3
    MX_TOMB_AMBIENCE           = 59, // 55.mp3
    MX_OOH_AAH_3               = 60, // 56.mp3
    MX_REVEAL_1                = 61, // 57.mp3
    MX_CUTSCENE_RIG            = 62, // 58.mp3
    MX_REVEAL_2                = 63, // 59.mp3
    MX_TITLE_THEME             = 64, // 60.mp3
    MX_UNUSED_6                = 65, // 61.mp3
} MUSIC_TRACK_ID;
// clang-format on

// clang-format off
typedef enum COLL_TYPE {
    COLL_NONE     = 0x00,
    COLL_FRONT    = 0x01,
    COLL_LEFT     = 0x02,
    COLL_RIGHT    = 0x04,
    COLL_TOP      = 0x08,
    COLL_TOPFRONT = 0x10,
    COLL_CLAMP    = 0x20,
} COLL_TYPE;
// clang-format on

#pragma pack(pop)
