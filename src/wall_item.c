#include "../include/wall_item.h"
#include "../include/players.h"
#include "../include/keybindings.h"
#include "../include/asset_defs.h"

static image* get_wallitem_img(wall_item_type item, wall_item_data data) {
	switch(item) {
		case WALLITEM_GUN: return get_image_of_gun(data.gun);
	}

	return 0;
}

static char* get_wallitem_name(wall_item_type item, wall_item_data data) {
	switch(item) {
		case WALLITEM_GUN: return get_gun_by_type(data.gun).name;
	}

	return 0;
}

static void apply_wallitem_to_player(wall_item item, player* p) {
	if (item.item == WALLITEM_GUN) {
		gun g = get_gun_by_type(item.data.gun);
		p->guntype = g.type;
		p->total_ammo = g.max_ammunition;
		p->ammo_in_mag = g.magazine_size;
	}
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

void update_wallitems_server() {
	#define MAX_HEIGHT_DIFF_FOR_WALLITEM (0.1f)

	for (int i = 0; i < MAX_WALLITEMS; i++) {
		wall_item item = wallitems[i];
		if (!item.active) continue;

		wallitems[i].time_active += SERVER_TICK_RATE;
		wallitems[i].position.z = MAX_HEIGHT_DIFF_FOR_WALLITEM * sin (2 * M_PI * 0.5f * (wallitems[i].time_active) + 0) + item.start_h;
	}
}

static float wallspace_entry_time = 0.0f;
void draw_keybinding_for_wallitem_purchase(platform_window* window, wall_item item) {
	OVERLAY_RENDER_DEPTH();

	map_info info = get_map_info(window);

	char purchase_text[50];
	snprintf(purchase_text, 50, "Press %s to purchase %s", keybind_wall_purchase.text, get_wallitem_name(item.item, item.data));
	int purchase_text_length = renderer->calculate_text_width(fnt_24, purchase_text);

	float center_tilex = item.position.x;
	float center_tiley = item.position.y - 2.5f;
	#define KEY_DRAW_W (0.8f)
	#define KEY_DRAW_H (0.6f)
	float height = 0.5;
	if (((int)wallspace_entry_time % 2) == 1) height = 0.1f;
	box key_box = get_render_box_of_square_without_incline(window, (vec3f){center_tilex - (purchase_text_length/info.tile_width)/2, 
		center_tiley, 0}, (vec3f){KEY_DRAW_W, KEY_DRAW_H, height});

	#define KEY_INSET ((0.12f)*info.tile_width)
	key_box.tl_u.x += KEY_INSET; key_box.bl_u.x += KEY_INSET;
	key_box.tl_u.y -= KEY_INSET; key_box.bl_u.y -= KEY_INSET;
	key_box.tr_u.x -= KEY_INSET; key_box.br_u.x -= KEY_INSET;
	key_box.tr_u.y -= KEY_INSET; key_box.br_u.y -= KEY_INSET;
	render_box_with_outline(key_box, rgb(255,255,255));

	int surface_w = key_box.tr_u.x - key_box.tl_u.x;
	int surface_h = key_box.br_u.y - key_box.tr_u.y;

	int text_w = renderer->calculate_text_width(fnt_24, keybind_wall_purchase.text);
	renderer->render_text(fnt_24, key_box.tl_u.x + surface_w/2 - text_w/2, 
		key_box.tl_u.y + surface_h/2 - fnt_24->px_h/2, keybind_wall_purchase.text, rgb(0,0,0));

	renderer->render_text(fnt_24, key_box.tr_d.x + 10, key_box.tr_d.y + (surface_h/2) - (fnt_24->px_h/2), purchase_text, rgb(255,255,255));

	wallspace_entry_time += update_delta;

	if (keyboard_is_key_pressed(keybind_wall_purchase.key)) {
		player* p = get_player_by_id(player_id);
		if (!p) return;
		apply_wallitem_to_player(item, p);
	}
}

void draw_wallitems(platform_window* window) {
	for (int i = 0; i < MAX_WALLITEMS; i++) {
		wall_item item = wallitems[i];
		if (!item.active) continue;

		{
			OBJECT_RENDER_DEPTH((int)item.position.y);
			box box = get_render_box_of_square(window, item.position, (vec3f){1,1,2});
			renderer->render_image(item.img, box.tl_u.x, box.tl_u.y, box.tr_u.x - box.tl_u.x, box.br_u.y - box.tr_u.y);
		}

		player* p = get_player_by_id(player_id);
		if (!p) continue;
		float dirx = ((item.position.x + (0.5)) - (p->playerx + (get_player_size_in_tile()/2)));
		float diry = ((item.position.y) - (p->playery + (get_player_size_in_tile()/2)));
		float length = sqrt(dirx * dirx + diry * diry);
		
		if (length < 1.0f) {
			draw_keybinding_for_wallitem_purchase(window, item);
		}
		else {
			wallspace_entry_time = 0.0f;
		}
	}
}