#include "../include/asset_defs.h"

void load_assets() {
	fnt_32 = assets_load_font(noto_regular_ttf, noto_regular_ttf+noto_regular_ttf_len, 32);
	fnt_24 = assets_load_font(noto_regular_ttf, noto_regular_ttf+noto_regular_ttf_len, 24);
	fnt_20 = assets_load_font(noto_regular_ttf, noto_regular_ttf+noto_regular_ttf_len, 20);

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
}