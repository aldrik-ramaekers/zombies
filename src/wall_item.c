#include "../include/wall_item.h"

static image* get_wallitem_img(wall_item_type item, wall_item_data data) {
	switch(item) {
		case WALLITEM_GUN: return get_image_of_gun(data.gun);
	}

	return 0;
}

void create_wallitem(vec3f position, wall_item_type type, wall_item_data data) {
	wall_item item;
	item.active = true;
	item.position = position;
	item.item = type;
	item.data = data;
	item.start_h = position.z;
	item.time_active = 0.0f;
	item.img = get_wallitem_img(type, data);

	for (int i = 0; i < MAX_WALLITEMS; i++) {
		wall_item ei = wallitems[i];
		if (ei.active) continue;
		wallitems[i] = item;
		return;
	}
}

void update_wallitems() {
	#define MAX_HEIGHT_DIFF_FOR_WALLITEM (0.1f)

	for (int i = 0; i < MAX_WALLITEMS; i++) {
		wall_item item = wallitems[i];
		if (!item.active) continue;

		wallitems[i].time_active += update_delta;
		wallitems[i].position.z = MAX_HEIGHT_DIFF_FOR_WALLITEM * sin (2 * M_PI * 0.5f * (wallitems[i].time_active) + 0) + item.start_h;
	}
}

void draw_wallitems(platform_window* window) {
	for (int i = 0; i < MAX_WALLITEMS; i++) {
		wall_item item = wallitems[i];
		if (!item.active) continue;

		box box = get_render_box_of_square(window, item.position, (vec3f){1,1,2});
		renderer->render_image(item.img, box.tl_u.x, box.tl_u.y, box.tr_u.x - box.tl_u.x, box.br_u.y - box.tr_u.y);
	}
}