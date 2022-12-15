#include "../include/drops.h"
#include "../include/objects.h"
#include "../include/asset_defs.h"

void handle_drop_pickup(player* p, drop* d) {
	if (!d->active) return;

	switch(d->type) {
		case DROP_AMMO: p->total_ammo += d->data.ammo_count; break;

		default: log_infox("Unhandled drop type: %d", d->type); break;
	}
	d->active = false;
}

void update_drops() {
	#define MAX_HEIGHT_DIFF (0.3f)
	#define FREQUENCY (0.15)

	for (int i = 0; i < MAX_DROPS; i++) {
		drop b = drops[i];
		if (!b.active) continue;

		drops[i].time_active += update_delta;
		drops[i].position.z = MAX_HEIGHT_DIFF * sin (2 * M_PI * 0.5f * (drops[i].time_active) + 0) + b.start_h;

		for (int x = 0; x < max_players; x++) {
			player *p = &players[x];
			if (!p->active) continue;

			if (check_if_player_collided_with_box(*p, get_box_of_square(drops[i].position, drops[i].size))) {
				handle_drop_pickup(p, &drops[i]);
			}
		}
	}
}

void draw_drops(platform_window* window) {
	for (int i = 0; i < MAX_DROPS; i++) {
		drop b = drops[i];
		if (!b.active) continue;

		DROP_RENDER_DEPTH((int)(b.position.y));

		b.position.z = b.start_h;
		b.size.z = 0.0f;
		b.position.y += 0.2f;
		b.position.x -= 0.07f;
		box shadow_box = get_render_box_of_square(window, b.position, b.size);
		render_quad_with_outline(shadow_box.tl_d, shadow_box.tr_d, shadow_box.bl_d, shadow_box.br_d, rgba(0,0,0,120));
		render_quad_with_outline(shadow_box.tl_u, shadow_box.tr_u, shadow_box.bl_u, shadow_box.br_u, rgba(0,0,0,120));
		render_quad_with_outline(shadow_box.tl_u, shadow_box.tl_d, shadow_box.bl_u, shadow_box.bl_d, rgba(0,0,0,120));
		render_quad_with_outline(shadow_box.bl_u, shadow_box.br_u, shadow_box.bl_d, shadow_box.br_d, rgba(0,0,0,120));

		b = drops[i];
		box full_box = get_render_box_of_square(window, b.position, b.size);
		render_quad_with_outline(full_box.tl_d, full_box.tr_d, full_box.bl_d, full_box.br_d, rgb(218,112,214));
		render_quad_with_outline(full_box.tl_u, full_box.tr_u, full_box.bl_u, full_box.br_u, rgb(218,112,214));
		render_quad_with_outline(full_box.tl_u, full_box.tl_d, full_box.bl_u, full_box.bl_d, rgb(218,112,214));
		render_quad_with_outline(full_box.bl_u, full_box.br_u, full_box.bl_d, full_box.br_d, rgb(218,112,214));

		int drop_h = full_box.br_d.y - full_box.tr_d.y;

		int icon_size = full_box.tr_d.x - full_box.tl_d.x;
		int icon_x = full_box.tl_u.x;
		int icon_y = full_box.tl_u.y - icon_size + (drop_h/2);
		switch(b.type) {
			case DROP_AMMO: {
				renderer->render_image(img_icon_bullets, icon_x, icon_y, icon_size, icon_size);
			} break;

			default: break;
		}
	}
}

void spawn_drop(vec3f pos) {
	static int drop_percentage = 15;
	int val = rand() % (100 + 1);
	if (val > drop_percentage) {
		drop_percentage += 2;
		return;
	}
	drop_percentage = 15;

	for (int i = 0; i < MAX_DROPS; i++) {
		drop b = drops[i];
		if (b.active) continue;

		drop new_drop;
		new_drop.active = true;
		new_drop.time_active = 0.0f;
		new_drop.position = pos;
		new_drop.size = (vec3f){0.3f, 0.3f, 0.4f};
		new_drop.start_h = pos.z;
		new_drop.type = DROP_AMMO;
		new_drop.data.ammo_count = 20;

		drops[i] = new_drop;
		break;
	}
}