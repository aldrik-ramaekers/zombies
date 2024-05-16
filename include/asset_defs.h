#ifndef INCLUDE_ASSET_DEFS
#define INCLUDE_ASSET_DEFS

#include <projectbase/project_base.h>

/////// Loaded at game start.
image* img_splash_art1;
image* img_splash_art2;
image* img_splash_art3;
image* img_menu_screen;

Mix_Music* music_menu;

Mix_Chunk* wav_woosh;
Mix_Chunk* wav_menu_hover;

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

/////// Loaded at match start.

// UI
image* img_red_border;
image* img_heart;
image* img_hurt_overlay_left;
image* img_hurt_overlay_right;

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
image* img_lamp_east;
image* img_lamp_south;
image* img_metal_table_h;
image* img_cantine_glass;
image* img_glass_door_h_closed;
image* img_glass_door_h_open;
image* img_glass_door_v_closed;
image* img_glass_door_v_open;
image* img_big_control_panel;
image* img_chair_down;
image* img_computer_rack;
image* img_generator;
image* img_school_table;
image* img_school_bench;
image* img_locker;
image* img_bar_h;
image* img_bar_v;
image* img_bar_hv;
image* img_club_seat1;
image* img_club_seat2;
image* img_club_seat3;
image* img_club_seat4;
image* img_club_seat5;
image* img_club_seat6;
image* img_club_seat7;
image* img_club_wall1;
image* img_club_wall2;
image* img_club_wall3;
image* img_bowling_lane;
image* img_bowling_lane_end;
image* img_bowling_machine;
image* img_painting1;
image* img_painting2;
image* img_painting3;
image* img_painting4;
image* img_painting5;
image* img_painting6;
image* img_painting7;
image* img_painting8;
image* img_painting9;
image* img_painting10;

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

// Tiles @NEWTILE
image* img_mars_surface;
image* img_space_parallax;
image* img_tile_floor1;
image* img_tile_floor2;
image* img_tile_bathroom;
image* img_tile_funk;
image* img_tile_wood1;
image* img_tile_gym;
image* img_tile_flowers;
image* img_tile_carpet1;
image* img_tile_carpet2;
image* img_tile_carpet3;
image* img_tile_carpet4;
image* img_tile_carpet5;
image* img_tile_carpet_stairs_v;
image* img_tile_club1;
image* img_tile_club2;
image* img_tile_club3;
image* img_tile_club4;
image* img_tile_club_stairs1;
image* img_tile_club_stairs2;
image* img_tile_club_stairs3;
image* img_tile_club_stairs4;
image* img_tile_club_stairs5;
image* img_tile_club_stairs6;
image* img_tile_club_stairs7;
image* img_tile_club_stairs8;
image* img_tile_club_stairs9;
image* img_tile_club_stairs10;
image* img_tile_club_stairs11;
image* img_tile_club_stairs12;
image* img_tile_club_stairs13;
image* img_tile_laminate;
image* img_tile_concrete;

// Sounds
Mix_Chunk* wav_throwable_bounce;
Mix_Chunk* wav_grenade_explode;
Mix_Chunk* wav_molotov_explode;
Mix_Chunk* wav_heartbeat;
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

Mix_Chunk* wav_door_open;
Mix_Chunk* wav_door_close;

#define NUM_SCREECHES 11
Mix_Chunk* wav_screech[NUM_SCREECHES];

#define NUM_PLAYER_HURT 3
Mix_Chunk* wav_player_hurt[NUM_PLAYER_HURT];

Mix_Music* music_inside1;

void load_assets();
font* get_font(platform_window* window, float scale);

#endif