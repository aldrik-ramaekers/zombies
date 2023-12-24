#include "../include/asset_defs.h"

//#include "../data/fonts/oxanium.h"
#include "../data/fonts/aleo.h"

void load_assets() {

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

	// Objects
	img_spawner = assets_load_image_from_file("data/imgs/spawner.png");
	img_obj_plants = assets_load_image_from_file("data/imgs/plants.png");
	img_obj_wall1 = assets_load_image_from_file("data/imgs/wall1.png");

	// Players
	img_player_running = assets_load_image_from_file("data/imgs/players/player1_running.png");

	// Other
	img_zombie_chunk_hand = assets_load_image_from_file("data/imgs/zombie_chunk_hand.png");
	img_zombie_chunk_foot = assets_load_image_from_file("data/imgs/zombie_chunk_foot.png");
	img_zombie_chunk_blood = assets_load_image_from_file("data/imgs/zombie_chunk_blood.png");

	// Tiles
	img_tile_cobblestone = assets_load_image_from_file("data/imgs/tiles/tile_cobblestone.png");
	img_tile_grass1 = assets_load_image_from_file("data/imgs/tiles/tile_grass1.png");

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
	wav_collect = Mix_LoadWAV("data/sounds/collect.wav");
	wav_round_change = Mix_LoadWAV("data/sounds/round_change.wav");
	wav_character = Mix_LoadWAV("data/sounds/character.wav");
	wav_step = Mix_LoadWAV("data/sounds/step.wav");
}

font* get_font(platform_window* window, float scale) {
	log_assert(scale >= 0.2f && scale <= 2.2f, "scale must be a multiple of 0.2");
	float window_scale = window->width / 800.0f;
	float diff = window_scale - 1.0f;

	int index_of_original = (int)(scale / 0.2f);
	int index_to_return = index_of_original + (int)(diff/0.2f);
	
	if (index_to_return < 0) index_to_return = 0;
	if (index_to_return > 10) index_to_return = 10;

	font* arr[] = {
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