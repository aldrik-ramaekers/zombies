#include "../include/guns.h"
#include "../include/asset_defs.h"

gun get_gun_by_type(gun_type type) {
	return guns[type];
}

image* get_image_of_gun(gun_type type) {
	switch (type)
	{
	case GUN_NOVA: return img_icon_nova; break;
	
	default:
		break;
	}

	return 0;
}