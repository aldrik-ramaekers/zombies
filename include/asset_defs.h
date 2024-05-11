#ifndef INCLUDE_ASSET_DEFS
#define INCLUDE_ASSET_DEFS

#include <projectbase/project_base.h>

font* fnt_52;
font* fnt_48;
font* fnt_44;
font* fnt_40;
font* fnt_36;
font* fnt_32;
font* fnt_28;
font* fnt_24;
font* fnt_20;
font* fnt_16;
font* fnt_12;
font* fnt_8;
font* fnt_4;

// UI
image* img_red_border;

// Icons
image* img_icon_bullets;
image* img_icon_nova;
image* img_drop;
image* img_arrow_up;
image* img_arrow_down;
image* img_cancel;
image* img_3d;
image* img_tiles;
image* img_sunny;
image* img_disconnected;
image* img_icon_grenade;
image* img_icon_molotov;

// Objects // @NEWOBJECT
image* img_spawner;
image* img_space_control_panel;
image* img_space_control_panel2;
image* img_space_window;
image* img_space_window_h;
image* img_metal_wall;
image* img_metal_wall2;
image* img_chair_up;
image* img_zombie_spawner;

// Throwables
image* img_grenade;
image* img_grenade_explode;
image* img_molotov_explode;

// Players
image* img_gunner_blue_run;
image* img_gunner_blue_idle;
image* img_gunner_blue_die;

image* img_gunner_black_run;
image* img_gunner_black_idle;
image* img_gunner_black_die;

image* img_gunner_green_run;
image* img_gunner_green_idle;
image* img_gunner_green_die;

image* img_gunner_yellow_run;
image* img_gunner_yellow_idle;
image* img_gunner_yellow_die;

image* img_gunner_red_run;
image* img_gunner_red_idle;
image* img_gunner_red_die;

image* img_bullet_stream;
image* img_muzzle_flash;
image* img_alien_run;

// Guns
image* img_gun_mp5;

// Other
image* img_zombie_chunk_hand;
image* img_zombie_chunk_foot;
image* img_zombie_chunk_blood;

// Tiles
image* img_mars_surface;
image* img_tile_floor1;
image* img_tile_floor2;

// Sounds
Mix_Chunk* wav_throwable_bounce;
Mix_Chunk* wav_grenade_explode;
Mix_Chunk* wav_molotov_explode;

Mix_Chunk* wav_fire;

Mix_Chunk* wav_shoot_mp5;
Mix_Chunk* wav_reload_mp5;

Mix_Chunk* wav_shoot_nova;
Mix_Chunk* wav_reload_nova;

Mix_Chunk* wav_impact_zombie;
Mix_Chunk* wav_impact_wood;
Mix_Chunk* wav_impact_metal;
Mix_Chunk* wav_error;
Mix_Chunk* wav_collect;

Mix_Chunk* wav_character;
Mix_Chunk* wav_roar_enraged;
Mix_Chunk* wav_round_change;
Mix_Chunk* wav_step;
Mix_Chunk* wav_step_enraged;

#define NUM_SCREECHES 11
Mix_Chunk* wav_screech[NUM_SCREECHES];

#define NUM_PLAYER_HURT 3
Mix_Chunk* wav_player_hurt[NUM_PLAYER_HURT];

Mix_Music* music_inside1;

void load_assets();
font* get_font(platform_window* window, float scale);

#endif