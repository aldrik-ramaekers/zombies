#include "../include/asset_defs.h"

//#include "../data/fonts/oxanium.h"
#include "../data/fonts/aleo.h"

void load_menu_assets() { // Assets loaded at game start
	img_menu_screen = assets_load_image_from_file("data/imgs/ui/menu_screen.png");
	img_splash_art2 = assets_load_image_from_file("data/imgs/ui/splash2.png");
	img_splash_art3 = assets_load_image_from_file("data/imgs/ui/splash3.png");

	wav_woosh = Mix_LoadWAV("data/sounds/woosh.wav");
	wav_menu_hover = Mix_LoadWAV("data/sounds/menu_hover.wav");
	music_menu = Mix_LoadMUS("data/sounds/menu_ambient.mp3");

	fnt_52 = assets_load_font(Aleo_Regular, Aleo_Regular + Aleo_Regular_Size, 52);
	fnt_48 = assets_load_font(Aleo_Regular, Aleo_Regular + Aleo_Regular_Size, 48);
	fnt_44 = assets_load_font(Aleo_Regular, Aleo_Regular + Aleo_Regular_Size, 44);
	fnt_40 = assets_load_font(Aleo_Regular, Aleo_Regular + Aleo_Regular_Size, 40);
	fnt_36 = assets_load_font(Aleo_Regular, Aleo_Regular + Aleo_Regular_Size, 36);
	fnt_32 = assets_load_font(Aleo_Regular, Aleo_Regular + Aleo_Regular_Size, 32);
	fnt_28 = assets_load_font(Aleo_Regular, Aleo_Regular + Aleo_Regular_Size, 28);
	fnt_24 = assets_load_font(Aleo_Regular, Aleo_Regular + Aleo_Regular_Size, 24);
	fnt_20 = assets_load_font(Aleo_Regular, Aleo_Regular + Aleo_Regular_Size, 20);
	fnt_16 = assets_load_font(Aleo_Regular, Aleo_Regular + Aleo_Regular_Size, 16);
	fnt_12 = assets_load_font(Aleo_Regular, Aleo_Regular + Aleo_Regular_Size, 12);
	fnt_8 = assets_load_font(Aleo_Regular, Aleo_Regular + Aleo_Regular_Size, 8);
	fnt_4 = assets_load_font(Aleo_Regular, Aleo_Regular + Aleo_Regular_Size, 4);

	img_splash_art1 = assets_load_image_from_file("data/imgs/ui/splash1.png");
}

void load_assets() { // Assets loaded at match start.
	// UI
	img_red_border = assets_load_image_from_file("data/imgs/ui/red_border.png");

	// Icons
	img_icon_bullets = assets_load_image_from_file("data/imgs/bullets.png");
	img_icon_nova = assets_load_image_from_file("data/imgs/nova.png");
	img_drop = assets_load_image_from_file("data/imgs/drop.png");
	img_arrow_up = assets_load_image_from_file("data/imgs/icons/arrow_up.png");
	img_arrow_down = assets_load_image_from_file("data/imgs/icons/arrow_down.png");
	img_cancel = assets_load_image_from_file("data/imgs/icons/cross.png");
	img_3d = assets_load_image_from_file("data/imgs/icons/3d.png");
	img_tiles = assets_load_image_from_file("data/imgs/icons/tiles.png");
	img_sunny = assets_load_image_from_file("data/imgs/icons/sunny.png");
	img_disconnected = assets_load_image_from_file("data/imgs/icons/disconnected.png");
	img_icon_grenade = assets_load_image_from_file("data/imgs/icons/grenade.png");
	img_icon_molotov = assets_load_image_from_file("data/imgs/icons/molotov.png");
	

	// Throwables
	img_grenade = assets_load_image_from_file("data/imgs/throwables/grenade.png");
	img_grenade_explode = assets_load_image_from_file("data/imgs/throwables/grenade_explode.png");
	img_molotov_explode = assets_load_image_from_file("data/imgs/throwables/molotov_explode.png");

	// Objects // @NEWOBJECT
	img_spawner = assets_load_image_from_file("data/imgs/spawner.png");
	img_space_control_panel = assets_load_image_from_file("data/imgs/objects/space_control_panel.png");
	img_space_control_panel2 = assets_load_image_from_file("data/imgs/objects/space_control_panel2.png");
	img_space_window = assets_load_image_from_file("data/imgs/objects/metal_wall_window.png");
	img_space_window_h = assets_load_image_from_file("data/imgs/objects/metal_wall_window_h.png");
	img_metal_wall = assets_load_image_from_file("data/imgs/objects/metal_wall.png");
	img_metal_wall2 = assets_load_image_from_file("data/imgs/objects/metal_wall2.png");
	img_chair_up = assets_load_image_from_file("data/imgs/objects/chair_up.png");
	img_chair_down = assets_load_image_from_file("data/imgs/objects/chair_down.png");
	img_zombie_spawner = assets_load_image_from_file("data/imgs/objects/zombie_spawner.png");
	img_lamp_east = assets_load_image_from_file("data/imgs/objects/lamp_east.png");
	img_lamp_south = assets_load_image_from_file("data/imgs/objects/lamp_south.png");
	img_metal_table_h = assets_load_image_from_file("data/imgs/objects/metal_table_h.png");
	img_cantine_glass = assets_load_image_from_file("data/imgs/objects/cantine_glass.png");
	img_glass_door_h_open = assets_load_image_from_file("data/imgs/objects/glass_door_h_open.png");
	img_glass_door_h_closed = assets_load_image_from_file("data/imgs/objects/glass_door_h_closed.png");
	img_big_control_panel = assets_load_image_from_file("data/imgs/objects/big_control_panel.png");
	img_computer_rack = assets_load_image_from_file("data/imgs/objects/computer_rack.png");
	img_generator = assets_load_image_from_file("data/imgs/objects/generator.png");
	img_school_bench = assets_load_image_from_file("data/imgs/objects/school_bench.png");
	img_school_table = assets_load_image_from_file("data/imgs/objects/school_table.png");
	img_locker = assets_load_image_from_file("data/imgs/objects/locker.png");
	img_bar_h = assets_load_image_from_file("data/imgs/objects/bar_h.png");
	img_bar_v = assets_load_image_from_file("data/imgs/objects/bar_v.png");
	img_bar_hv = assets_load_image_from_file("data/imgs/objects/bar_hv.png");
	img_club_seat1 = assets_load_image_from_file("data/imgs/objects/club_seat1.png");
	img_club_seat2 = assets_load_image_from_file("data/imgs/objects/club_seat2.png");
	img_club_seat3 = assets_load_image_from_file("data/imgs/objects/club_seat3.png");
	img_club_seat4 = assets_load_image_from_file("data/imgs/objects/club_seat4.png");
	img_club_wall1 = assets_load_image_from_file("data/imgs/objects/club_wall1.png");
	img_club_wall2 = assets_load_image_from_file("data/imgs/objects/club_wall2.png");
	img_club_wall3 = assets_load_image_from_file("data/imgs/objects/club_wall3.png");
	img_club_seat5 = assets_load_image_from_file("data/imgs/objects/club_seat5.png");
	img_club_seat6 = assets_load_image_from_file("data/imgs/objects/club_seat6.png");
	img_club_seat7 = assets_load_image_from_file("data/imgs/objects/club_seat7.png");
	img_bowling_lane = assets_load_image_from_file("data/imgs/objects/bowling_lane.png");
	img_bowling_lane_end = assets_load_image_from_file("data/imgs/objects/bowling_lane_end.png");

	// Players
	img_gunner_black_run = assets_load_image_from_file("data/imgs/players/Black/Gunner_Black_Run.png");
	img_gunner_black_idle = assets_load_image_from_file("data/imgs/players/Black/Gunner_Black_Idle.png");
	img_gunner_black_die = assets_load_image_from_file("data/imgs/players/Black/Gunner_Black_Death.png");

	img_gunner_blue_run = assets_load_image_from_file("data/imgs/players/Blue/Gunner_Blue_Run.png");
	img_gunner_blue_idle = assets_load_image_from_file("data/imgs/players/Blue/Gunner_Blue_Idle.png");
	img_gunner_blue_die = assets_load_image_from_file("data/imgs/players/Blue/Gunner_Blue_Death.png");

	img_gunner_green_run = assets_load_image_from_file("data/imgs/players/Green/Gunner_Green_Run.png");
	img_gunner_green_idle = assets_load_image_from_file("data/imgs/players/Green/Gunner_Green_Idle.png");
	img_gunner_green_die = assets_load_image_from_file("data/imgs/players/Green/Gunner_Green_Death.png");

	img_gunner_red_run = assets_load_image_from_file("data/imgs/players/Red/Gunner_Red_Run.png");
	img_gunner_red_idle = assets_load_image_from_file("data/imgs/players/Red/Gunner_Red_Idle.png");
	img_gunner_red_die = assets_load_image_from_file("data/imgs/players/Red/Gunner_Red_Death.png");

	img_gunner_yellow_run = assets_load_image_from_file("data/imgs/players/Yellow/Gunner_Yellow_Run.png");
	img_gunner_yellow_idle = assets_load_image_from_file("data/imgs/players/Yellow/Gunner_Yellow_Idle.png");
	img_gunner_yellow_die = assets_load_image_from_file("data/imgs/players/Yellow/Gunner_Yellow_Death.png");

	img_muzzle_flash = assets_load_image_from_file("data/imgs/players/MuzzleFlash.png");
	img_bullet_stream = assets_load_image_from_file("data/imgs/players/BulletStream.png");
	img_alien_run = assets_load_image_from_file("data/imgs/players/alien_run.png");

	// Guns
	img_gun_mp5 = assets_load_image_from_file("data/imgs/guns/mp5.png");

	// Other
	img_zombie_chunk_hand = assets_load_image_from_file("data/imgs/zombie_chunk_hand.png");
	img_zombie_chunk_foot = assets_load_image_from_file("data/imgs/zombie_chunk_foot.png");
	img_zombie_chunk_blood = assets_load_image_from_file("data/imgs/zombie_chunk_blood.png");

	// Tiles // @NEWTILE
	img_mars_surface = assets_load_image_from_file("data/imgs/tiles/mars_surface.png");
	img_tile_floor1 = assets_load_image_from_file("data/imgs/tiles/floor1.png");
	img_tile_floor2 = assets_load_image_from_file("data/imgs/tiles/floor2.png");
	img_space_parallax = assets_load_image_from_file("data/imgs/tiles/space_parallax.png");
	img_tile_bathroom = assets_load_image_from_file("data/imgs/tiles/bathroom.png");
	img_tile_funk = assets_load_image_from_file("data/imgs/tiles/tile_funk.png");
	img_tile_wood1 = assets_load_image_from_file("data/imgs/tiles/tile_wood1.png");
	img_tile_gym = assets_load_image_from_file("data/imgs/tiles/tile_gym.png");
	img_tile_flowers = assets_load_image_from_file("data/imgs/tiles/tile_flowers.png");
	img_tile_carpet1 = assets_load_image_from_file("data/imgs/tiles/tile_carpet1.png");
	img_tile_carpet2 = assets_load_image_from_file("data/imgs/tiles/tile_carpet2.png");
	img_tile_carpet3 = assets_load_image_from_file("data/imgs/tiles/tile_carpet3.png");
	img_tile_carpet4 = assets_load_image_from_file("data/imgs/tiles/tile_carpet4.png");
	img_tile_carpet5 = assets_load_image_from_file("data/imgs/tiles/tile_carpet5.png");
	img_tile_carpet_stairs_v = assets_load_image_from_file("data/imgs/tiles/tile_carpet_stairs_v.png");
	img_tile_club1 = assets_load_image_from_file("data/imgs/tiles/tile_club1.png");
	img_tile_club2 = assets_load_image_from_file("data/imgs/tiles/tile_club2.png");
	img_tile_club3 = assets_load_image_from_file("data/imgs/tiles/tile_club3.png");
	img_tile_club4 = assets_load_image_from_file("data/imgs/tiles/tile_club4.png");
	img_tile_club_stairs1 = assets_load_image_from_file("data/imgs/tiles/club_stairs1.png");
	img_tile_club_stairs2 = assets_load_image_from_file("data/imgs/tiles/club_stairs2.png");
	img_tile_club_stairs3 = assets_load_image_from_file("data/imgs/tiles/club_stairs3.png");
	img_tile_club_stairs4 = assets_load_image_from_file("data/imgs/tiles/club_stairs4.png");
	img_tile_club_stairs5 = assets_load_image_from_file("data/imgs/tiles/club_stairs5.png");
	img_tile_club_stairs6 = assets_load_image_from_file("data/imgs/tiles/club_stairs6.png");
	img_tile_club_stairs7 = assets_load_image_from_file("data/imgs/tiles/club_stairs7.png");
	img_tile_club_stairs8 = assets_load_image_from_file("data/imgs/tiles/club_stairs8.png");
	img_tile_club_stairs9 = assets_load_image_from_file("data/imgs/tiles/club_stairs9.png");
	img_tile_club_stairs10 = assets_load_image_from_file("data/imgs/tiles/club_stairs10.png");
	img_tile_club_stairs11 = assets_load_image_from_file("data/imgs/tiles/club_stairs11.png");
	img_tile_club_stairs12 = assets_load_image_from_file("data/imgs/tiles/club_stairs12.png");
	img_tile_club_stairs13 = assets_load_image_from_file("data/imgs/tiles/club_stairs13.png");

	// sounds
	wav_throwable_bounce = Mix_LoadWAV("data/sounds/throwable_bounce.wav");
	wav_grenade_explode = Mix_LoadWAV("data/sounds/grenade_explode.wav");
	wav_molotov_explode = Mix_LoadWAV("data/sounds/molotov_explode.wav");
	wav_fire = Mix_LoadWAV("data/sounds/fire.wav");
	wav_shoot_mp5 = Mix_LoadWAV("data/sounds/shoot_mp5.wav");
	wav_reload_mp5 = Mix_LoadWAV("data/sounds/reload_mp5.wav");
	wav_shoot_nova = Mix_LoadWAV("data/sounds/shoot_nova.wav");
	wav_reload_nova = Mix_LoadWAV("data/sounds/reload_nova.wav");
	wav_impact_wood = Mix_LoadWAV("data/sounds/impact_wood.wav");
	wav_error = Mix_LoadWAV("data/sounds/error.wav");
	wav_impact_zombie = Mix_LoadWAV("data/sounds/impact_zombie.wav");
	wav_impact_metal = Mix_LoadWAV("data/sounds/impact_metal.wav"); 
	wav_collect = Mix_LoadWAV("data/sounds/collect.wav");
	wav_round_change = Mix_LoadWAV("data/sounds/round_change.wav");
	wav_character = Mix_LoadWAV("data/sounds/character.wav");
	wav_step = Mix_LoadWAV("data/sounds/step.wav");
	wav_step_enraged = Mix_LoadWAV("data/sounds/step_enraged.wav");
	wav_roar_enraged = Mix_LoadWAV("data/sounds/roar_enraged.wav");
	wav_door_open = Mix_LoadWAV("data/sounds/door_open.wav");
	wav_door_close = Mix_LoadWAV("data/sounds/door_close.wav");

	for (int i = 1; i <= NUM_SCREECHES; i++) {
		char path[100];
		sprintf(path, "data/sounds/screech%d.wav", i);
		wav_screech[i-1] = Mix_LoadWAV(path);
	}

	for (int i = 1; i <= NUM_PLAYER_HURT; i++) {
		char path[100];
		sprintf(path, "data/sounds/player_hurt%d.wav", i);
		wav_player_hurt[i-1] = Mix_LoadWAV(path);
	}

	// music
	music_inside1 = Mix_LoadMUS("data/sounds/music_inside1.mp3");

}

font* get_font(platform_window* window, float scale) {
	log_assert(scale >= 0.2f && scale <= 2.2f, "scale must be a multiple of 0.2");
	float window_scale = window->width / 800.0f;
	float diff = window_scale - 1.0f;

	float step = 1/3.0f;

	int index_of_original = (int)(scale / step);
	int index_to_return = index_of_original + (int)(diff/step);
	
	if (index_to_return < 0) index_to_return = 0;
	if (index_to_return > 12) index_to_return = 12;

	font* arr[] = {
		fnt_4,
		fnt_8,
		fnt_12, // scale = 0.2
		fnt_16,
		fnt_20,
		fnt_24,
		fnt_28, // scale = 1.0
		fnt_32,
		fnt_36,
		fnt_40,
		fnt_44,
		fnt_48, // scale = 2.0
		fnt_52,
	};

	return arr[index_to_return];
}