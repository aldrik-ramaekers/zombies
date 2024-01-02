#include "../include/guns.h"
#include "../include/asset_defs.h"

gun get_gun_by_type(gun_type type) {
	return guns[type];
}

float get_gun_size(gun_type type) {
	return 0.5f;
}

image* get_image_of_gun(gun_type type) {
	switch (type)
	{
	case GUN_NOVA: return img_gun_mp5; break;
	case GUN_MP5: return img_gun_mp5; break;
	
	default:
		break;
	}

	return 0;
}

image* get_icon_of_gun(gun_type type) {
	switch (type)
	{
	case GUN_NOVA: return img_icon_nova; break;
	case GUN_MP5: return img_icon_nova; break;
	
	default:
		break;
	}

	return 0;
}