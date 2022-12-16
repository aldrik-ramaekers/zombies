#include "../include/asset_defs.h"

void load_assets() {
	fnt_24 = assets_load_font(mono_ttf, mono_ttf+mono_ttf_len, 24);
	fnt_20 = assets_load_font(mono_ttf, mono_ttf+mono_ttf_len, 20);

	img_icon_bullets = assets_load_image_from_file("data/imgs/bullets.png");

	img_icon_nova = assets_load_image_from_file("data/imgs/nova.png");
}