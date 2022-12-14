#include "../include/asset_defs.h"

void load_assets() {
	fnt_24 = assets_load_font(mono_ttf, mono_ttf+mono_ttf_len, 24);
	fnt_20 = assets_load_font(mono_ttf, mono_ttf+mono_ttf_len, 20);
}