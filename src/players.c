#include "../include/players.h"
#include "../include/game.h"

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
		players[i].guntype = GUN_NOVA;
		return;
	}
}


void move_user(platform_window* window, u32 id, protocol_move_type move) {
	float speed = 0.1f;
	float pad_between_player_and_obj = 0.01f;

	player* p = get_player_by_id(id);
	if (p == 0) {
		log_info("Unknown user moved");
		return;
	}

	if (move == MOVE_UP) {
		float newy = p->playery - speed;
		if (is_in_bounds(p->playerx, newy)) {
			p->playery = newy;
			object o = check_if_player_collided_with_object(window, *p);
			if (o.active) p->playery = o.position.y+o.size.y - get_player_size_in_tile() + pad_between_player_and_obj;
		}
	}

	if (move == MOVE_DOWN) {
		float newy = p->playery + speed;
		if (is_in_bounds(p->playerx, newy)) {
			p->playery = newy;
			object o = check_if_player_collided_with_object(window, *p);
			if (o.active) p->playery = o.position.y - get_player_size_in_tile() - pad_between_player_and_obj;
		}
	}

	if (move == MOVE_LEFT) {
		float newx = p->playerx - speed;
		if (is_in_bounds(newx, p->playery)) {
			p->playerx = newx;
			object o = check_if_player_collided_with_object(window, *p);
			if (o.active) p->playerx = o.position.x+o.size.x + pad_between_player_and_obj;
		}
	}

	if (move == MOVE_RIGHT) {
		float newx = p->playerx + speed;
		if (is_in_bounds(newx, p->playery)) {
			p->playerx = newx;
			object o = check_if_player_collided_with_object(window, *p);
			if (o.active) p->playerx = o.position.x-get_player_size_in_tile() - pad_between_player_and_obj;
		}
	}
}

player* get_player_by_id(u32 id) {
	for (int i = 0; i < max_players; i++) {
		if (!players[i].active) continue;
		if (players[i].id == id) return &players[i];
	}
	return 0;
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
	player* p = get_player_by_id(my_id);
	if (!p) return;

	if (keyboard_is_key_down(KEY_W)) {
		if (!global_state.server) {
			network_message message = create_protocol_user_moved(MOVE_UP, my_id);
			network_client_send(global_state.client, message);
		}
		move_user(window, my_id, MOVE_UP);
	}
	if (keyboard_is_key_down(KEY_S)) {
		if (!global_state.server) {
			network_message message = create_protocol_user_moved(MOVE_DOWN, my_id);
			network_client_send(global_state.client, message);
		}
		move_user(window, my_id, MOVE_DOWN);
	}
	if (keyboard_is_key_down(KEY_A)) {
		if (!global_state.server) {
			network_message message = create_protocol_user_moved(MOVE_LEFT, my_id);
			network_client_send(global_state.client, message);
		}
		move_user(window, my_id, MOVE_LEFT);
	}
	if (keyboard_is_key_down(KEY_D)) {
		if (!global_state.server) {
			network_message message = create_protocol_user_moved(MOVE_RIGHT, my_id);
			network_client_send(global_state.client, message);
		}
		move_user(window, my_id, MOVE_RIGHT);
	}

	// Send gun position
	{
		float dirx = (_global_mouse.x - (window->width/2));
		float diry = (_global_mouse.y - (window->height/2));
		double length = sqrt(dirx * dirx + diry * diry);
		dirx /= length;
		diry /= length;

		float gun_offset_x = (get_player_size_in_tile()/2) + dirx/2;
		float gun_offset_y = (get_player_size_in_tile()/2) + diry/2;

		p->gunx = p->playerx + gun_offset_x;
		p->guny = p->playery + gun_offset_y;
		
		network_client_send(global_state.client, create_protocol_user_look(my_id, gun_offset_x, gun_offset_y));
	}

	// shoot
	{
		if (is_left_down()) {
			float dirx = (_global_mouse.x - (window->width/2));
			float diry = (_global_mouse.y - (window->height/2));
			double length = sqrt(dirx * dirx + diry * diry);
			dirx /= length;
			diry /= length;

			network_message message = create_protocol_user_shoot(my_id, dirx, diry);
			network_client_send(global_state.client, message);
		}
	}
}

void update_players_server() {
	for (int i = 0; i < max_players; i++) {
		if (!players[i].active) continue;
		players[i].sec_since_last_shot += update_delta;
	}
}

void draw_players_at_tile(platform_window* window, int x, int y) {
	for (int i = 0; i < max_players; i++) {
		if (!players[i].active) continue;
		if ((int)players[i].playerx != x || (int)(players[i].playery+get_player_size_in_tile()) != y) continue;

		int size = get_tile_width(window) / 2;
		map_info info = get_map_info(window);
		float height = get_height_of_tile_under_coords(window, players[i].playerx, players[i].playery);

		float player_render_x = players[i].playerx*info.tile_width + (players[i].playery*info.px_incline);
		float player_render_y = players[i].playery*info.tile_height - (height*info.px_raised_per_h);
		renderer->render_rectangle(player_render_x, player_render_y, size, size, rgb(200,150,120));

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
