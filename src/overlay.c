#include "../include/overlay.h"
#include "../include/guns.h"
#include "../include/players.h"
#include "../include/asset_defs.h"

#define EDGE_PADDING 10

static void draw_gun_info(platform_window* window) {
	player *p = get_player_by_id(player_id);
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
	int pad_y = (h-fnt_20->px_h)/2;
	int pad_x = pad_y;
	renderer->render_text(fnt_20, x+pad_x, y+pad_y, name, rgb(255,255,255));
	renderer->render_text(fnt_20, x+pad_x+width_for_name+width_per_entry*0, y+pad_y, kills, rgb(255,255,255));
	renderer->render_text(fnt_20, x+pad_x+width_for_name+width_per_entry*1, y+pad_y, deaths, rgb(255,255,255));
	renderer->render_text(fnt_20, x+pad_x+width_for_name+width_per_entry*2, y+pad_y, ping, rgb(255,255,255));
}

static void draw_leaderboard(platform_window* window) {
	if (keyboard_is_key_down(KEY_TAB)) {
		int minimum_width = 300;
		int maximum_width = 900;
		int actual_width = window->width * 0.6f;
		if (actual_width > maximum_width) actual_width = maximum_width;
		if (actual_width < minimum_width) actual_width = minimum_width;

		int height_per_row = 24;
		int height_total = height_per_row * (get_player_count() + 1);

		int x = (int)_global_camera.x + (window->width - actual_width) / 2;
		int y = (int)_global_camera.y + (window->height - height_total) / 4;

		renderer->render_rectangle(x, y, actual_width, height_total, rgba(0,0,0,120));

		draw_leaderboard_entry(x, y, actual_width, height_per_row, "Player", "Kills", "Deaths", "Ping");
		for (int i = 0; i < MAX_PLAYERS; i++) {
			if (!players[i].active) continue;

			char kills[30]; snprintf(kills, 30, "%d", players[i].kills);
			char deaths[30]; snprintf(deaths, 30, "%d", 0);
			char ping[30]; snprintf(ping, 30, "%d", players[i].ping);
			draw_leaderboard_entry(x, y + ((i+1)*height_per_row), actual_width, height_per_row, players[i].client.ip, kills, deaths, ping);
		}
	}
}

void draw_debug_stats(platform_window* window) {
	float fps = 1.0f / update_delta;
	float usage = (update_delta / (1.0f / 60.0f) * 100);
	int count = 0;
	for (int i = 0; i < OUTGOING_QUEUE_SIZE; i++)
	{
		if (!messages_to_send_queue[i].active) continue;
		count++;
	}

	char fps_text[50];
	snprintf(fps_text, 50, "FPS: %d, MS: %.4f, USAGE: %.0f%%", (int)fps, update_delta*1000.0f, usage);

	char update_text[50];
	snprintf(update_text, 50, "update: %.2fms, queue: %d", logic_update_time/1000000.0f, count);

	renderer->render_text(fnt_20, _global_camera.x, _global_camera.y + fnt_20->px_h*0, fps_text, rgb(0,0,0));
	renderer->render_text(fnt_20, _global_camera.x, _global_camera.y + fnt_20->px_h*1, update_text, rgb(0,0,0));
}

void draw_overlay(platform_window* window) {
	OVERLAY_RENDER_DEPTH();
	draw_gun_info(window);
	draw_leaderboard(window);
	draw_debug_stats(window);
}