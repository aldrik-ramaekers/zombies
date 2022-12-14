#include "../include/overlay.h"
#include "../include/guns.h"
#include "../include/players.h"
#include "../include/asset_defs.h"

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

static void draw_leaderboard_entry(int x, int y, int w, int h, char* name, char* kills, char* deaths, char* ping) {
	int width_for_name = w / 2;
	int width_per_entry = (w-width_for_name)/3;
	renderer->render_text(fnt_20, x, y, name, rgb(0,0,0));
	renderer->render_text(fnt_20, x+width_for_name+width_per_entry*0, y, kills, rgb(0,0,0));
	renderer->render_text(fnt_20, x+width_for_name+width_per_entry*1, y, deaths, rgb(0,0,0));
	renderer->render_text(fnt_20, x+width_for_name+width_per_entry*2, y, ping, rgb(0,0,0));
}

static void draw_leaderboard(platform_window* window) {
	if (keyboard_is_key_down(KEY_TAB)) {
		int minimum_width = 300;
		int maximum_width = 900;
		int actual_width = window->width * 0.6f;
		if (actual_width > maximum_width) actual_width = maximum_width;
		if (actual_width < minimum_width) actual_width = minimum_width;

		int height_per_row = 20;
		int height_total = height_per_row * (get_player_count() + 1);

		int x = (int)_global_camera.x + (window->width - actual_width) / 2;
		int y = (int)_global_camera.y + (window->height - height_total) / 4;

		renderer->render_rectangle(x, y, actual_width, height_total, rgba(255,255,255, 200));

		draw_leaderboard_entry(x, y, actual_width, height_per_row, "Player", "Kills", "Deaths", "Ping");
		for (int i = 0; i < max_players; i++) {
			if (!players[i].active) continue;

			char kills[30]; snprintf(kills, 30, "%d", 0);
			char deaths[30]; snprintf(deaths, 30, "%d", 0);
			char ping[30]; snprintf(ping, 30, "%d", 0);
			draw_leaderboard_entry(x, y + ((i+1)*height_per_row), actual_width, height_per_row, players[i].client.ip, kills, deaths, ping);
		}
	}
}

void draw_overlay(platform_window* window) {
	OVERLAY_RENDER_DEPTH();
	draw_gun_info(window);
	draw_leaderboard(window);
}