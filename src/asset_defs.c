#include "../include/asset_defs.h"

void load_assets() {
	fnt_24 = assets_load_font(noto_regular_ttf, noto_regular_ttf+noto_regular_ttf_len, 24);
	fnt_20 = assets_load_font(noto_regular_ttf, noto_regular_ttf+noto_regular_ttf_len, 20);

	// Icons
	img_icon_bullets = assets_load_image_from_file("data/imgs/bullets.png");
	img_icon_nova = assets_load_image_from_file("data/imgs/nova.png");
	img_drop = assets_load_image_from_file("data/imgs/drop.png");
	img_arrow_up = assets_load_image_from_file("data/imgs/icons/arrow_up.png");
	img_arrow_down = assets_load_image_from_file("data/imgs/icons/arrow_down.png");
	img_cancel = assets_load_image_from_file("data/imgs/icons/cross.png");

	// Objects
	img_spawner = assets_load_image_from_file("data/imgs/spawner.png");
	img_obj_plants = assets_load_image_from_file("data/imgs/plants.png");
	img_obj_wall1 = assets_load_image_from_file("data/imgs/wall1.png");

	// Players
	img_player = assets_load_image_from_file("data/imgs/player.png");

	// Other
	img_zombie_chunk_hand = assets_load_image_from_file("data/imgs/zombie_chunk_hand.png");
	img_zombie_chunk_foot = assets_load_image_from_file("data/imgs/zombie_chunk_foot.png");
	img_zombie_chunk_blood = assets_load_image_from_file("data/imgs/zombie_chunk_blood.png");

	// Tiles
	img_tile_cobblestone = assets_load_image_from_file("data/imgs/tiles/tile_cobblestone.png");
	img_tile_grass1 = assets_load_image_from_file("data/imgs/tiles/tile_grass1.png");
}