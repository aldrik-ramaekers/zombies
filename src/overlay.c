#include "../include/overlay.h"
#include "../include/guns.h"
#include "../include/players.h"
#include "../include/asset_defs.h"

#define EDGE_PADDING 10

static void draw_gun_info(platform_window* window) {
	player *p = get_player_by_id(player_id);
	if (!p) return;

	// Overlay background.
	int overlay_w = window->width / 4;
	int overlay_h = overlay_w*(img_stats_overlay->height/(float)img_stats_overlay->width);

	int overlay_x = _global_camera.x + window->width/2 - overlay_w/2;
	int overlay_y = _global_camera.y + window->height - overlay_h;

	renderer->render_image(img_stats_overlay, overlay_x, overlay_y, overlay_w, overlay_h);

	// Round text.
	{
		font* round_fnt = get_font(window, 2.0f);
		char round_text[30];
		sprintf(round_text, "%d", _current_round.round_nr);
		int round_text_x = _global_camera.x + window->width / 2 - renderer->calculate_text_width(round_fnt, round_text)/2;
		int round_text_y = overlay_y + overlay_h/1.35f - round_fnt->px_h/2;

		renderer->render_text(round_fnt, round_text_x+1, round_text_y-1, round_text, rgb(0,0,0));
		renderer->render_text(round_fnt, round_text_x, round_text_y, round_text, rgb(255,255,255));

		round_fnt = get_font(window, 1.5f);
		sprintf(round_text, "%s", "Round");
		round_text_x = _global_camera.x + window->width / 2 - renderer->calculate_text_width(round_fnt, round_text)/2;
		round_text_y = overlay_y + overlay_h/3.0f - round_fnt->px_h/2;

		renderer->render_text(round_fnt, round_text_x+1, round_text_y-1, round_text, rgb(0,0,0));
		renderer->render_text(round_fnt, round_text_x, round_text_y, round_text, rgb(255,255,255));
	}

	// Ammo.
	{
		gun g = get_gun_by_type(p->guntype);
		font* round_fnt = get_font(window, 1.5f);
		char ammo_txt[50];
		sprintf(ammo_txt, "%d/%d", p->ammo_in_mag, p->total_ammo);
		int ammo_text_x = _global_camera.x + window->width / 2 - renderer->calculate_text_width(round_fnt, ammo_txt)/2 - overlay_w/3.2f;
		int ammo_text_y = overlay_y + overlay_h/1.35f - round_fnt->px_h/2;

		renderer->render_text(round_fnt, ammo_text_x+1, ammo_text_y-1, ammo_txt, rgb(0,0,0));
		renderer->render_text(round_fnt, ammo_text_x, ammo_text_y, ammo_txt, rgb(255,255,255));
	}
}

static void draw_leaderboard_entry(int x, int y, int w, int h, char* name, char* kills, char* deaths, char* ping, bool highlighted, bool disconnected) {

	if (highlighted) {
		renderer->render_rectangle(x, y, w, h, rgba(255,0,0,50));
	}

	int width_for_name = w / 2;
	int width_per_entry = (w-width_for_name)/3;
	int pad_y = (h-fnt_20->px_h)/2;
	int pad_x = pad_y;
	renderer->render_text(fnt_20, x+pad_x, y+pad_y, name, rgb(255,255,255));
	renderer->render_text(fnt_20, x+pad_x+width_for_name+width_per_entry*0, y+pad_y, kills, rgb(255,255,255));
	renderer->render_text(fnt_20, x+pad_x+width_for_name+width_per_entry*1, y+pad_y, deaths, rgb(255,255,255));

	if (disconnected) {
		int icon_h = h*0.8f;
		int iconpad = ((h-icon_h)/2);
		renderer->render_image(img_disconnected, x+pad_x+width_for_name+width_per_entry*2.5f - iconpad, y + iconpad, icon_h, icon_h);
	}
	else {
		renderer->render_text(fnt_20, x+pad_x+width_for_name+width_per_entry*2, y+pad_y, ping, rgb(255,255,255));
	}
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

		draw_leaderboard_entry(x, y, actual_width, height_per_row, "Player", "Kills", "Deaths", "Ping", false, false);
		for (int i = 0; i < MAX_PLAYERS; i++) {
			if (!players[i].active) continue;

			char kills[30]; snprintf(kills, 30, "%d", players[i].kills);
			char deaths[30]; snprintf(deaths, 30, "%d", 0);
			char ping[30]; snprintf(ping, 30, "%d", players[i].ping);
			draw_leaderboard_entry(x, y + ((i+1)*height_per_row), actual_width, height_per_row, 
				get_player_name_by_player_index(i), kills, deaths, ping, players[i].id == player_id, players[i].connection_state == DISCONNECTED);
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

	char fps_text[100];
	snprintf(fps_text, 100, "FPS: %d, MS: %.4f, USAGE: %.0f%%", (int)fps, update_delta*1000.0f, usage);

	char update_text[100];
	snprintf(update_text, 100, "update: %.2fms, tcp queue: %d, pathfinding queue: %d", logic_update_time/1000000.0f, count, global_pathfinding_queue.length);

	renderer->render_text(fnt_20, _global_camera.x, _global_camera.y + fnt_20->px_h*0, fps_text, rgb(0,0,0));
	renderer->render_text(fnt_20, _global_camera.x, _global_camera.y + fnt_20->px_h*1, update_text, rgb(0,0,0));
}

void draw_points(platform_window* window) {
	int x = (int)_global_camera.x + window->width - EDGE_PADDING;
	int y = (int)_global_camera.y + window->height - EDGE_PADDING;
	char points_txt[20];

	font* score_font = get_font(window, 1.0f);
	font* sub_score_font = get_font(window, 0.2f);
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (!players[i].active) continue;

		sprintf(points_txt, "%d", players[i].points);

		y -= score_font->px_h;
		int x1 = renderer->calculate_text_width(score_font, points_txt);
		renderer->render_text(score_font, x - x1+1, y+1, points_txt, rgba(0,0,0,120));
		renderer->render_text(score_font, x - x1, y, points_txt, get_color_tint_by_player_index(i));

		for (int t = 0; t < MAX_POINT_ANIMATIONS; t++) {
			if (!players[i].point_animations[t].active) continue;
			point_animation animation = players[i].point_animations[t];
			sprintf(points_txt, "%d", animation.points);
			int textw = renderer->calculate_text_width(sub_score_font, points_txt);
			int py = y - animation.position.y;
			int px = x1 - animation.position.x;
			px += textw+2;
			color c = get_color_tint_by_player_index(i);
			c.a = 255*animation.opacity;
			renderer->render_text(sub_score_font, x - px+1, py+1, points_txt, rgba(0,0,0,120*animation.opacity));
			renderer->render_text(sub_score_font, x - px, py, points_txt, c);
		}

		y -= 8;
	}
}

static void draw_hurt_borders(platform_window* window) {
	player* p = get_player_by_id(player_id);
	if (!p) return;

	if (p->max_health == p->health) return;
	if (p->health == 0)
	{
		int skull_size = window->width / 7;
		renderer->render_image(img_skull, 
			_global_camera.x + (window->width/2)-(skull_size/2), 
			_global_camera.y + (window->height/2)-(skull_size/2), skull_size, skull_size);
	}
	else {
		float freq = 1.0f * (p->health/(float)p->max_health);
		static float heatbeat_timestamp = 0.0f;
		heatbeat_timestamp += update_delta;

		float opacity = 1.0f - (heatbeat_timestamp / freq);
		int heart_size = window->width / 7;
		color c = rgba(255,255,255,255*opacity);

		renderer->render_image_tint(img_heart, 
			_global_camera.x + (window->width/2)-(heart_size/2), 
			_global_camera.y + (window->height/2)-(heart_size/2), heart_size, heart_size, c);

		if (heatbeat_timestamp >= freq) {
			heatbeat_timestamp = 0.0f;
			play_sound(-1, wav_heartbeat);
		}
	}

	float border_opacity = 1.0f - (p->health/(float)p->max_health);
	int hurt_border_width = window->width / (6 - (border_opacity*3));
	color border_color = rgba(255,255,255,255*border_opacity);

	renderer->render_image_tint(img_hurt_overlay_left, _global_camera.x, 
		_global_camera.y, hurt_border_width, window->height, border_color);
	renderer->render_image_tint(img_hurt_overlay_right, _global_camera.x + window->width - hurt_border_width, 
		_global_camera.y, hurt_border_width, window->height, border_color);
}

static void draw_global_message(platform_window* window) {
	if (!game_is_paused) return;
	char* text = "Game is paused";
	int text_w = renderer->calculate_text_width(fnt_52, text);

	int x = _global_camera.x;
	int y = _global_camera.y + 80;

	renderer->render_text(fnt_52, x + (window->width/2)-(text_w/2)+1, y-1, text, rgb(0,0,0));
	renderer->render_text(fnt_52, x + (window->width/2)-(text_w/2), y, text, rgb(255,255,255));
}

static void draw_fadein_overlay(platform_window* window)
{
	float percentage = _current_round.fade_in_timer / FADE_IN_DURATION;
	if (percentage <= 0.0f) return;
	renderer->render_rectangle(_global_camera.x, _global_camera.y, window->width, window->height, rgba(0,0,0,255*percentage));
}

static bool draw_ingame_menu_button(platform_window* window, int y, int h, char* text)
{
	bool result = false;
	int btn_w = 300;
	font* fnt = get_font(window, 1.5);
	int x = _global_camera.x + window->width/2 - btn_w/2;
	color bg_c = rgba(0,0,0,0);

	static int was_hovered = 0;
	if (_global_mouse.x + _global_camera.x > x && 
			_global_mouse.x + _global_camera.x < x + btn_w && 
			_global_mouse.y + _global_camera.y > y && 
			_global_mouse.y + _global_camera.y < y + h) 
	{
		if (was_hovered == 0) {
			play_sound(-1, wav_menu_hover);
		}

		was_hovered = x+y;

		bg_c = rgb(200,100,100);
		if (is_left_clicked()) {
			result = true;
		}
	}
	else if (was_hovered == x+y) {
		was_hovered = 0;
	}

	int text_w = renderer->calculate_text_width(fnt, text);	
	renderer->render_rectangle(x, y, btn_w, h, bg_c);
	renderer->render_text(fnt, x + (btn_w/2)-(text_w/2), y+(h/2)-(fnt->px_h/2), text, rgb(255,255,255));

	return result;
}

static bool ingame_menu_open = 0;
static void draw_ingame_menu(platform_window* window)
{
	if (keyboard_is_key_pressed(KEY_ESCAPE)) {
		ingame_menu_open = !ingame_menu_open;
	}

	if (global_state.server) {
		game_is_paused = ingame_menu_open;
	}

	if (!ingame_menu_open) return;

	renderer->render_rectangle(_global_camera.x, _global_camera.y, window->width, window->height, rgba(0,0,0,220));

	int btn_h = 50;
	if (draw_ingame_menu_button(window, _global_camera.y + 200, btn_h, "Continue")) {
		ingame_menu_open = false;
	}

	if (draw_ingame_menu_button(window, _global_camera.y + 200 + 60, btn_h, "Restart")) {
		ingame_menu_open = false;
		restart_game();
	}

	if (draw_ingame_menu_button(window, _global_camera.y + 200 + 120, btn_h, "Quit")) {
		ingame_menu_open = false;
		global_scene_state = SCENE_MAIN_MENU;
		global_state.state = GAMESTATE_PLAYING;
		current_menu_state = MENU_STATE_MAIN;

		if (global_state.server) {
			_current_round.round_nr = 0;
			clear_players();
			clear_zombies();
			network_client_close(global_state.client);
			networking_destroy_server(global_state.server);
		}
		else {
			network_client_close(global_state.client);
		}

		global_state.server = 0;
		global_state.client = 0;
	}
}

void draw_overlay(platform_window* window) {
	draw_hurt_borders(window);
	draw_gun_info(window);
	draw_points(window);
	draw_leaderboard(window);
	draw_debug_stats(window);
	draw_fadein_overlay(window);
	draw_ingame_menu(window);
	draw_global_message(window);
}