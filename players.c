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

object check_if_player_collided_with_object(platform_window* window) {
	map_info info = get_map_info(window);
	float player_size = get_player_size(window);

	for (int i = 0; i < max_objects; i++) {
		object o = objects[i];
		if (!o.active) continue;

		box box = get_box_of_object(window, o);
		float x_to_check = playerx;
		float player_size_in_tile_px = player_size / (float)info.tile_width;
		float y_to_check = playery + player_size_in_tile_px;

		if (x_to_check+player_size_in_tile_px >= o.position.x && x_to_check <= o.position.x+o.size.x
		 && y_to_check >= o.position.y && y_to_check <= o.position.y+o.size.y) {
			return o;
		}
	}

	return (object){0};
}

void take_player_input(platform_window* window) {
	float speed = 0.1f;
	float pad_between_player_and_obj = 0.01f;

	float old_x = playerx;
	float old_y = playery;

	if (keyboard_is_key_down(KEY_W)) {
		float newy = playery - speed;
		if (is_in_bounds(playerx, newy)) {
			playery = newy;
			object o = check_if_player_collided_with_object(window);
			if (o.active) playery = o.position.y+o.size.y - get_player_size_in_tile() + pad_between_player_and_obj;
		}
	}

	if (keyboard_is_key_down(KEY_S)) {
		float newy = playery + speed;
		if (is_in_bounds(playerx, newy)) {
			playery = newy;
			object o = check_if_player_collided_with_object(window);
			if (o.active) playery = o.position.y - get_player_size_in_tile() - pad_between_player_and_obj;
		}
	}

	if (keyboard_is_key_down(KEY_A)) {
		float newx = playerx - speed;
		if (is_in_bounds(newx, playery)) {
			playerx = newx;
			object o = check_if_player_collided_with_object(window);
			if (o.active) playerx = o.position.x+o.size.x + pad_between_player_and_obj;
		}
	}

	if (keyboard_is_key_down(KEY_D)) {
		float newx = playerx + speed;
		if (is_in_bounds(newx, playery)) {
			playerx = newx;
			object o = check_if_player_collided_with_object(window);
			if (o.active) playerx = o.position.x-get_player_size_in_tile() - pad_between_player_and_obj;
		}
	}
}

void draw_player(platform_window* window) {
	int size = get_tile_width(window) / 2;
	map_info info = get_map_info(window);
	float height = get_height_of_tile_under_coords(window, playerx, playery);

	take_player_input(window);

	sec_since_last_shot += update_delta;
	float bullets_per_sec = 10;
	float time_between_bullets = 1.0f/bullets_per_sec;
	if (is_left_down()) {
		if (sec_since_last_shot > time_between_bullets) {
			for (int i = 0; i < 3; i++) shoot(window);
			sec_since_last_shot = 0.0f;
		}
	}

	float player_render_x = playerx*info.tile_width + (playery*info.px_incline);
	float player_render_y = playery*info.tile_height - (height*info.px_raised_per_h);
	renderer->render_rectangle(player_render_x, player_render_y, size, size, rgb(200,150,120));

	float dirx = (_global_mouse.x - (window->width/2));
	float diry = (_global_mouse.y - (window->height/2));
	double length = sqrt(dirx * dirx + diry * diry);
	dirx /= length;
	diry /= length;

	gunx = playerx + (get_player_size_in_tile()/2) + dirx/2;
	guny = playery + (get_player_size_in_tile()/2) + diry/2;
	gun_height = height+0.5;
	float gun_render_x = gunx*info.tile_width + (guny*info.px_incline);
	float gun_render_y = guny*info.tile_height - (gun_height*info.px_raised_per_h);

	renderer->render_rectangle(gun_render_x, gun_render_y, size/4, size/4, rgb(20,255,20));

	_global_camera.x = -(window->width / 2) + player_render_x;
	_global_camera.y = -(window->height / 2) + player_render_y;
}
