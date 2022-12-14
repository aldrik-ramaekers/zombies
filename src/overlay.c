#include "../include/overlay.h"
#include "../include/guns.h"

#define EDGE_PADDING 10

static void draw_gun_info(platform_window* window) {
	player *p = get_player_by_id(my_id);
	if (!p) return;

	gun g = get_gun_by_type(p->guntype);


	int y = window->height - fnt_24->px_h - EDGE_PADDING;
	int x = EDGE_PADDING;

	renderer->render_text(fnt_24, (int)_global_camera.x + x, (int)_global_camera.y + y, g.name, rgb(255,255,255));
	y -= fnt_24->px_h;

	char ammo_txt[50];
	sprintf(ammo_txt, "%d / %d", p->ammo_in_mag, p->total_ammo);
	renderer->render_text(fnt_20, (int)_global_camera.x + x, (int)_global_camera.y + y, ammo_txt, rgb(255,255,255));
}

void draw_overlay(platform_window* window) {
	OVERLAY_RENDER_DEPTH();
	draw_gun_info(window);
}