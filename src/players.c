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

int get_player_count() {
	int count = 0;
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (!players[i].active) continue;
		count++;
	}
	return count;
}

void spawn_player(u32 id, network_client client) {
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (players[i].active) continue;
		players[i].active = true;
		players[i].sec_since_last_shot = 10.0f;
		players[i].playerx = 3;
		players[i].playery = 3;
		players[i].gunx = 0.0f;
		players[i].guny = 0.0f;
		players[i].gun_height = 0.0f;
		players[i].id = id;
		players[i].guntype = GUN_MP5;
		players[i].height = 0.0f;
		players[i].client = client;

		gun g = get_gun_by_type(players[i].guntype);
		players[i].total_ammo = g.max_ammunition;
		players[i].ammo_in_mag = g.magazine_size;
		return;
	}
}


void move_user(platform_window* window, u32 id, protocol_move_type move, float delta) {
	float speed = 5.5f * delta;
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
			if (o.active) p->playery = o.position.y+o.size.y + pad_between_player_and_obj;
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
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (!players[i].active) continue;
		if (players[i].id == id) return &players[i];
	}
	return 0;
}

bool check_if_player_collided_with_box(player p, box o) {
	float player_size = get_player_size_in_tile();

	box pbox = get_box_of_square((vec3f){p.playerx, p.playery, p.height}, (vec3f){player_size,player_size,0.8f});

	// [x1, y1, x2, y2]
	bool b1 = min(pbox.br_u.x, o.br_d.x) > max(pbox.tl_u.x, o.tl_u.x);
	bool b2 = min(pbox.br_u.y, o.br_d.y) > max(pbox.tl_u.y, o.tl_u.y);
	return b1 && b2;
}

object check_if_player_collided_with_object(platform_window* window, player p) {
	map_info info = get_map_info(window);
	float player_size = get_player_size(window);

	for (int i = 0; i < max_objects; i++) {
		object o = objects[i];
		if (!o.active) continue;

		if (check_if_player_collided_with_box(p, get_box_of_square((vec3f){o.position.x, o.position.y, o.h}, o.size))) {
			return o;
		}
	}

	return (object){0};
}

int get_my_player_index() {
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (!players[i].active) continue;
		if (players[i].id == player_id) return i;
	}
	return -1;
}

void take_player_input(platform_window* window) {
	player* p = get_player_by_id(player_id);
	if (!p) return;

	if (keyboard_is_key_down(KEY_W)) {
		if (!global_state.server) {
			network_message message = create_protocol_user_moved(MOVE_UP, player_id);
			add_message_to_outgoing_queuex(message, *global_state.client);
		}
		move_user(window, player_id, MOVE_UP, update_delta);
	}
	if (keyboard_is_key_down(KEY_S)) {
		if (!global_state.server) {
			network_message message = create_protocol_user_moved(MOVE_DOWN, player_id);
			add_message_to_outgoing_queuex(message, *global_state.client);
		}
		move_user(window, player_id, MOVE_DOWN, update_delta);
	}
	if (keyboard_is_key_down(KEY_A)) {
		if (!global_state.server) {
			network_message message = create_protocol_user_moved(MOVE_LEFT, player_id);
			add_message_to_outgoing_queuex(message, *global_state.client);
		}
		move_user(window, player_id, MOVE_LEFT, update_delta);
	}
	if (keyboard_is_key_down(KEY_D)) {
		if (!global_state.server) {
			network_message message = create_protocol_user_moved(MOVE_RIGHT, player_id);
			add_message_to_outgoing_queuex(message, *global_state.client);
		}
		move_user(window, player_id, MOVE_RIGHT, update_delta);
	}

	// Send gun position
	{
		float dirx = (_global_mouse.x - (window->width/2));
		float diry = (_global_mouse.y - (window->height/2));
		double length = sqrt(dirx * dirx + diry * diry);
		dirx /= length;
		diry /= length;

		float gun_offset_x = (get_player_size_in_tile()/2) + dirx;
		float gun_offset_y = (get_player_size_in_tile()/2) + diry;

		p->gunx = p->playerx + gun_offset_x;
		p->guny = p->playery + gun_offset_y;
		
		add_message_to_outgoing_queuex(create_protocol_user_look(player_id, gun_offset_x, gun_offset_y), *global_state.client);
	}

	// shoot
	{
		if (is_left_down()) {
			float dirx = (_global_mouse.x - (window->width/2));
			float diry = (_global_mouse.y - (window->height/2));
			double length = sqrt(dirx * dirx + diry * diry);
			dirx /= length;
			diry /= length;

			network_message message = create_protocol_user_shoot(player_id, dirx, diry);
			add_message_to_outgoing_queuex(message, *global_state.client);
		}
	}
}

void update_players_server() {
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (!players[i].active) continue;
		players[i].sec_since_last_shot += SERVER_TICK_RATE;
	}
}

void draw_players(platform_window* window) {
	float size = get_player_size_in_tile();
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (!players[i].active) continue;

		OBJECT_RENDER_DEPTH((int)(players[i].playery+size));

		float height = get_height_of_tile_under_coords(window, players[i].playerx, players[i].playery);
		players[i].height = height;

		box box = get_render_box_of_square(window, (vec3f){players[i].playerx, players[i].playery, height}, (vec3f){size,size,0.8f});
		render_quad_with_outline(box.tl_d, box.tr_d, box.bl_d, box.br_d, rgb(200,150,120));
		render_quad_with_outline(box.tl_u, box.tr_u, box.bl_u, box.br_u, rgb(200,150,120));
		render_quad_with_outline(box.tl_u, box.tl_d, box.bl_u, box.bl_d, rgb(200,150,120));
		render_quad_with_outline(box.bl_u, box.br_u, box.bl_d, box.br_d, rgb(200,150,120));

		int size = get_tile_width(window) / 2;
		map_info info = get_map_info(window);


		float player_render_x = players[i].playerx*info.tile_width + (players[i].playery*info.px_incline);
		float player_render_y = players[i].playery*info.tile_height - (height*info.px_raised_per_h);

		players[i].gun_height = height+0.5;
		float gun_render_x = players[i].gunx*info.tile_width + (players[i].guny*info.px_incline);
		float gun_render_y = players[i].guny*info.tile_height - (players[i].gun_height*info.px_raised_per_h);
		
		renderer->render_rectangle(gun_render_x, gun_render_y, size/4, size/4, rgb(20,255,20));

		if (players[i].id == player_id) {
			_next_camera_pos.x = -(window->width / 2) + player_render_x;
			_next_camera_pos.y = -(window->height / 2) + player_render_y;
		}
	}
}
