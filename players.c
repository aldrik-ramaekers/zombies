#include "include/players.h"

float get_bullet_size_in_tile(platform_window* window) {
	return 1 / 8.0f;
}

float get_bullet_size(platform_window* window) {
	return get_tile_width(window) * get_bullet_size_in_tile(window);
}

float get_player_size_in_tile() {
	return 0.5;
}

float get_player_size(platform_window* window) {
	int player_size = get_tile_width(window) * get_player_size_in_tile();
}

void spawn_player(int id) {
	for (int i = 0; i < max_players; i++) {
		if (players[i].active) continue;
		players[i].active = true;
		players[i].sec_since_last_shot = 10.0f;
		players[i].playerx = 3;
		players[i].playery = 3;
		players[i].gunx = 0.0f;
		players[i].guny = 0.0f;
		players[i].gun_height = 0.0f;
		players[i].id = id;
		return;
	}
}

player get_player_by_id(int id) {
	for (int i = 0; i < max_players; i++) {
		if (!players[i].active) continue;
		if (players[i].id == id) return players[i];
	}
	return (player){-1};
}

object check_if_player_collided_with_object(platform_window* window, player p) {
	map_info info = get_map_info(window);
	float player_size = get_player_size(window);

	for (int i = 0; i < max_objects; i++) {
		object o = objects[i];
		if (!o.active) continue;

		box box = get_box_of_object(window, o);
		float x_to_check = p.playerx;
		float player_size_in_tile_px = player_size / (float)info.tile_width;
		float y_to_check = p.playery + player_size_in_tile_px;

		if (x_to_check+player_size_in_tile_px >= o.position.x && x_to_check <= o.position.x+o.size.x
		 && y_to_check >= o.position.y && y_to_check <= o.position.y+o.size.y) {
			return o;
		}
	}

	return (object){0};
}

int get_my_player_index() {
	for (int i = 0; i < max_players; i++) {
		if (!players[i].active) continue;
		if (players[i].id == my_id) return i;
	}
	return -1;
}

void take_player_input(platform_window* window) {
	float speed = 0.1f;
	float pad_between_player_and_obj = 0.01f;

	int my_index = get_my_player_index();
	if (my_index == -1) return;

	float old_x = players[my_index].playerx;
	float old_y = players[my_index].playery;

	if (keyboard_is_key_down(KEY_W)) {
		float newy = players[my_index].playery - speed;
		if (is_in_bounds(players[my_index].playerx, newy)) {
			players[my_index].playery = newy;
			object o = check_if_player_collided_with_object(window, players[my_index]);
			if (o.active) players[my_index].playery = o.position.y+o.size.y - get_player_size_in_tile() + pad_between_player_and_obj;
		}
	}

	if (keyboard_is_key_down(KEY_S)) {
		float newy = players[my_index].playery + speed;
		if (is_in_bounds(players[my_index].playerx, newy)) {
			players[my_index].playery = newy;
			object o = check_if_player_collided_with_object(window, players[my_index]);
			if (o.active) players[my_index].playery = o.position.y - get_player_size_in_tile() - pad_between_player_and_obj;
		}
	}

	if (keyboard_is_key_down(KEY_A)) {
		float newx = players[my_index].playerx - speed;
		if (is_in_bounds(newx, players[my_index].playery)) {
			players[my_index].playerx = newx;
			object o = check_if_player_collided_with_object(window, players[my_index]);
			if (o.active) players[my_index].playerx = o.position.x+o.size.x + pad_between_player_and_obj;
		}
	}

	if (keyboard_is_key_down(KEY_D)) {
		float newx = players[my_index].playerx + speed;
		if (is_in_bounds(newx, players[my_index].playery)) {
			players[my_index].playerx = newx;
			object o = check_if_player_collided_with_object(window, players[my_index]);
			if (o.active) players[my_index].playerx = o.position.x-get_player_size_in_tile() - pad_between_player_and_obj;
		}
	}
}

void draw_players_at_tile(platform_window* window, int x, int y) {
	for (int i = 0; i < max_players; i++) {
		if (!players[i].active) continue;
		if ((int)players[i].playerx != x || (int)(players[i].playery+get_player_size_in_tile()) != y) continue;

		int size = get_tile_width(window) / 2;
		map_info info = get_map_info(window);
		float height = get_height_of_tile_under_coords(window, players[i].playerx, players[i].playery);

		take_player_input(window);

		players[i].sec_since_last_shot += update_delta;
		float bullets_per_sec = 10;
		float time_between_bullets = 1.0f/bullets_per_sec;
		if (is_left_down()) {
			if (players[i].sec_since_last_shot > time_between_bullets) {
				int ix = get_my_player_index();
				if (ix != -1) {
					for (int i = 0; i < 3; i++) {
						shoot(window, players[ix]);
						players[i].sec_since_last_shot = 0.0f;
					}
				}
			}
		}

		float player_render_x = players[i].playerx*info.tile_width + (players[i].playery*info.px_incline);
		float player_render_y = players[i].playery*info.tile_height - (height*info.px_raised_per_h);
		renderer->render_rectangle(player_render_x, player_render_y, size, size, rgb(200,150,120));

		float dirx = (_global_mouse.x - (window->width/2));
		float diry = (_global_mouse.y - (window->height/2));
		double length = sqrt(dirx * dirx + diry * diry);
		dirx /= length;
		diry /= length;

		players[i].gunx = players[i].playerx + (get_player_size_in_tile()/2) + dirx/2;
		players[i].guny = players[i].playery + (get_player_size_in_tile()/2) + diry/2;
		players[i].gun_height = height+0.5;
		float gun_render_x = players[i].gunx*info.tile_width + (players[i].guny*info.px_incline);
		float gun_render_y = players[i].guny*info.tile_height - (players[i].gun_height*info.px_raised_per_h);

		renderer->render_rectangle(gun_render_x, gun_render_y, size/4, size/4, rgb(20,255,20));

		if (players[i].id == my_id) {
			_global_camera.x = -(window->width / 2) + player_render_x;
			_global_camera.y = -(window->height / 2) + player_render_y;
		}
	}
}
