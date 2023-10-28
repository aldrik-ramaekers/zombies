#ifndef INCLUDE_ASSET_DEFS
#define INCLUDE_ASSET_DEFS

#include <projectbase/project_base.h>

font* fnt_24;
font* fnt_20;

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

// Objects
image* img_spawner;
image* img_obj_plants;
image* img_obj_wall1;

// Players
image* img_player;

// Other
image* img_zombie_chunk_hand;
image* img_zombie_chunk_foot;
image* img_zombie_chunk_blood;

// Tiles
image* img_tile_cobblestone;
image* img_tile_grass1;

// Sounds
Mix_Chunk* wav_throwable_bounce;
Mix_Chunk* wav_shoot_mp5;

void load_assets();

#endif