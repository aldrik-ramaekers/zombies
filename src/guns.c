#include "../include/guns.h"

gun get_gun_by_type(gun_type type) {
	return guns[type];
}