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

image* img_test;

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

// Objects
image* img_spawner;
image* img_obj_plants;
image* img_obj_wall1;
image* img_metal_wall_front;

// Throwables
image* img_grenade;
image* img_grenade_explode;
image* img_molotov_explode;

// Players
image* img_helmet;
image* img_body;

// Other
image* img_zombie_chunk_hand;
image* img_zombie_chunk_foot;
image* img_zombie_chunk_blood;

// Tiles
image* img_tile_cobblestone;
image* img_tile_grass1;

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
Mix_Chunk* wav_error;
Mix_Chunk* wav_collect;

Mix_Chunk* wav_character;
Mix_Chunk* wav_round_change;
Mix_Chunk* wav_step;

#define NUM_SCREECHES 11
Mix_Chunk* wav_screech[NUM_SCREECHES];

Mix_Music* music_inside1;

void load_assets();
font* get_font(platform_window* window, float scale);

#endif