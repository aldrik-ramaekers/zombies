#include "../include/players.h"
#include "../include/game.h"

float get_bullet_size_in_tile(platform_window* window) {
	return 1 / 8.0f;
}

float get_bullet_size(platform_window* window) {
	return get_tile_width(window) * get_bullet_size_in_tile(window);
}

float get_player_size_in_tile() {
	return 0.8f;
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

bool player_has_old_session(u32 id) {
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (players[i].id == id && players[i].connection_state == DISCONNECTED) {
			return true;
		}
	}
	return false;
}

void rejoin_player(u32 id, network_client client) {
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (players[i].id == id) {
			players[i].connection_state = CONNECTED;
			players[i].client = client;
		}
	}
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
		players[i].sprite = create_sprite(img_player_running, 22, 108, 136, 0.02f);
		players[i].direction = DIRECTION_DOWN;
		players[i].connection_state = CONNECTED;
		players[i].throwables.grenades = 3; 
		players[i].throwables.molotovs = 1;

		gun g = get_gun_by_type(players[i].guntype);
		players[i].total_ammo = g.max_ammunition;
		players[i].ammo_in_mag = g.magazine_size;
		return;
	}
}


void move_user(platform_window* window, u32 id, protocol_move_type move, float delta) {
	float speed_straight = 5.5f;
	float speed = speed_straight * delta;
	float pad_between_player_and_obj = 0.01f;

	player* p = get_player_by_id(id);
	if (p == 0) {
		log_info("Unknown user moved");
		return;
	}

	if (move == MOVE_UPLEFT || move == MOVE_UPRIGHT || move == MOVE_DOWNLEFT || move == MOVE_DOWNRIGHT) {
		float diag = (speed_straight * sqrt(2));
		speed = (speed_straight * (speed_straight / diag)) * delta;
	}

	if (move == MOVE_UP || move == MOVE_UPRIGHT || move == MOVE_UPLEFT) {
		float newy = p->playery - speed;
		p->direction = DIRECTION_UP;
		if (is_in_bounds(p->playerx, newy)) {
			p->playery = newy;
			object o = check_if_player_collided_with_object(*p);
			if (o.active) p->playery = o.position.y+o.size.y + pad_between_player_and_obj;
		}
	}

	if (move == MOVE_DOWN || move == MOVE_DOWNLEFT || move == MOVE_DOWNRIGHT) {
		float newy = p->playery + speed;
		p->direction = DIRECTION_DOWN;
		if (is_in_bounds(p->playerx, newy)) {
			p->playery = newy;
			object o = check_if_player_collided_with_object(*p);
			if (o.active) p->playery = o.position.y - get_player_size_in_tile() - pad_between_player_and_obj;
		}
	}

	if (move == MOVE_LEFT || move == MOVE_UPLEFT || move == MOVE_DOWNLEFT) {
		float newx = p->playerx - speed;	
		p->direction = DIRECTION_LEFT;
		if (is_in_bounds(newx, p->playery)) {
			p->playerx = newx;
			object o = check_if_player_collided_with_object(*p);
			if (o.active) p->playerx = o.position.x+o.size.x + pad_between_player_and_obj;
		}
	}

	if (move == MOVE_RIGHT || move == MOVE_UPRIGHT || move == MOVE_DOWNRIGHT) {
		float newx = p->playerx + speed;
		p->direction = DIRECTION_RIGHT;
		if (is_in_bounds(newx, p->playery)) {
			p->playerx = newx;
			object o = check_if_player_collided_with_object(*p);
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

object check_if_player_collided_with_object(player p) {
	for (int i = 0; i < MAX_OBJECTS; i++) {
		object o = loaded_map.objects[i];
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

#ifdef MODE_DEBUG
	update_editor(window);
#endif
	
	if (keyboard_is_key_down(KEY_W) && keyboard_is_key_down(KEY_D)) {
		network_message message = create_protocol_user_moved(MOVE_UPRIGHT, player_id);
		add_message_to_outgoing_queuex(message, *global_state.client);
		move_user(window, player_id,MOVE_UPRIGHT, update_delta);
	}
	else if (keyboard_is_key_down(KEY_W) && keyboard_is_key_down(KEY_A)) {
		network_message message = create_protocol_user_moved(MOVE_UPLEFT, player_id);
		add_message_to_outgoing_queuex(message, *global_state.client);
		move_user(window, player_id,MOVE_UPLEFT, update_delta);
	}
	else if (keyboard_is_key_down(KEY_S) && keyboard_is_key_down(KEY_A)) {
		network_message message = create_protocol_user_moved(MOVE_DOWNLEFT, player_id);
		add_message_to_outgoing_queuex(message, *global_state.client);
		move_user(window, player_id,MOVE_DOWNLEFT, update_delta);
	}
	else if (keyboard_is_key_down(KEY_S) && keyboard_is_key_down(KEY_D)) {
		network_message message = create_protocol_user_moved(MOVE_DOWNRIGHT, player_id);
		add_message_to_outgoing_queuex(message, *global_state.client);
		move_user(window, player_id,MOVE_DOWNRIGHT, update_delta);
	}
	else if (keyboard_is_key_down(KEY_A)) {
		network_message message = create_protocol_user_moved(MOVE_LEFT, player_id);
		add_message_to_outgoing_queuex(message, *global_state.client);
		move_user(window, player_id,MOVE_LEFT, update_delta);
	}
	else if (keyboard_is_key_down(KEY_D)) {
		network_message message = create_protocol_user_moved(MOVE_RIGHT, player_id);
		add_message_to_outgoing_queuex(message, *global_state.client);
		move_user(window, player_id,MOVE_RIGHT, update_delta);
	}
	else if (keyboard_is_key_down(KEY_W)) {
		network_message message = create_protocol_user_moved(MOVE_UP, player_id);
		add_message_to_outgoing_queuex(message, *global_state.client);
		move_user(window, player_id,MOVE_UP, update_delta);
	}
	else if (keyboard_is_key_down(KEY_S)) {
		network_message message = create_protocol_user_moved(MOVE_DOWN, player_id);
		add_message_to_outgoing_queuex(message, *global_state.client);
		move_user(window, player_id,MOVE_DOWN, update_delta);
	}


#ifdef MODE_DEBUG
	if (is_editing_map) return;
#endif

	// Send gun position
	{
		float dirx = (_global_mouse.x - (window->width/2));
		float diry = (_global_mouse.y - (window->height/2));
		double length = sqrt(dirx * dirx + diry * diry);
		dirx /= length;
		diry /= length;

		float gun_offset_x = (get_player_size_in_tile()/2) + (dirx/4);
		float gun_offset_y = (get_player_size_in_tile()/2) + (diry/4);

		add_message_to_outgoing_queuex(create_protocol_user_look(player_id, gun_offset_x, gun_offset_y), *global_state.client);
	}

	{
		box box = get_render_box_of_square(window, (vec3f){p->playerx, p->playery, p->height}, (vec3f){1.0,1.0,1.0f});
		float dirx = (_global_mouse.x - (box.tl_u.x - _global_camera.x));
		float diry = (_global_mouse.y - (box.tl_u.y - _global_camera.y));
		double length = sqrt(dirx * dirx + diry * diry);
		dirx /= length;
		diry /= length;

		// shooting
		if (is_left_down()) {
			network_message message = create_protocol_user_shoot(player_id, dirx, diry);
			add_message_to_outgoing_queuex(message, *global_state.client);
		}

		// throwing
		if (is_right_clicked()) {
			network_message message = create_protocol_user_throw(player_id, dirx, diry, THROWABLE_GRENADE);
			add_message_to_outgoing_queuex(message, *global_state.client);
		}
	}	
}

void update_players_client() {
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (!players[i].active) continue;

		if (players[i].direction == DIRECTION_DOWN) {
			players[i].sprite.frame_start = 374;
		}
		if (players[i].direction == DIRECTION_UP) {
			players[i].sprite.frame_start = 0;
		}
		if (players[i].direction == DIRECTION_LEFT) {
			players[i].sprite.frame_start = 660;
		}
		if (players[i].direction == DIRECTION_RIGHT) {
			players[i].sprite.frame_start = 198;
		}

		if (players[i].direction == DIRECTION_TOPRIGHT) {
			players[i].sprite.frame_start = 110;
		}
		if (players[i].direction == DIRECTION_TOPLEFT) {
			players[i].sprite.frame_start = 506;
		}
		if (players[i].direction == DIRECTION_BOTTOMRIGHT) {
			players[i].sprite.frame_start = 286;
		}
		if (players[i].direction == DIRECTION_BOTTOMLEFT) {
			players[i].sprite.frame_start = 682;
		}
		

		update_sprite(&players[i].sprite);
	}
}

void update_players_server() {
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (!players[i].active) continue;
		players[i].sec_since_last_shot += SERVER_TICK_RATE;
		players[i].sec_since_interact_state_change += SERVER_TICK_RATE;

		gun g = get_gun_by_type(players[i].guntype);
		if (players[i].interact_state == INTERACT_RELOADING && players[i].sec_since_interact_state_change >= g.reload_time) {
			int amount_to_reload = g.magazine_size;
			if (amount_to_reload > players[i].total_ammo) amount_to_reload = players[i].total_ammo;
			players[i].total_ammo -= amount_to_reload;
			players[i].ammo_in_mag = amount_to_reload;

			players[i].interact_state = INTERACT_IDLE;
		}
	}

	update_players_client();
}

void draw_players(platform_window* window) {
	float size = get_player_size_in_tile();
	map_info info = get_map_info(window);

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (!players[i].active) continue;

		OBJECT_RENDER_DEPTH((int)(players[i].playery+size));

		float height = get_height_of_tile_under_coords(players[i].playerx, players[i].playery);
		players[i].height = height;

		box box = get_render_box_of_square(window, (vec3f){players[i].playerx, players[i].playery, height}, (vec3f){size,size,1.0f});
		
		/*
		render_quad_with_outline(box.tl_d, box.tr_d, box.bl_d, box.br_d, rgb(200,150,120));
		render_quad_with_outline(box.tl_u, box.tr_u, box.bl_u, box.br_u, rgb(200,150,120));
		render_quad_with_outline(box.tl_u, box.tl_d, box.bl_u, box.bl_d, rgb(200,150,120));
		render_quad_with_outline(box.bl_u, box.br_u, box.bl_d, box.br_d, rgb(200,150,120));
		*/
		sprite_frame frame = sprite_get_frame(&players[i].sprite);
		renderer->render_image_quad_partial(img_player_running, 
			box.tl_u.x, box.tl_u.y,
			box.bl_d.x, box.bl_d.y, 
			box.br_d.x, box.br_d.y, 
			box.tr_u.x, box.tr_u.y, 
			frame.tl, frame.tr, frame.bl, frame.br);
			
		float player_render_x = box.tl_u.x;
		float player_render_y = box.tl_u.y;
		
		players[i].gun_height = height+0.5;

		if (players[i].connection_state == DISCONNECTED) {
			float icon_h = (box.tr_u.x - box.tl_u.x)/2;
			renderer->render_image(img_disconnected, box.tl_u.x + (icon_h/3), box.tr_u.y - icon_h, icon_h, icon_h);
		}

		if (players[i].id == player_id) {		
			_next_camera_pos.x = -(window->width / 2) + player_render_x;
			_next_camera_pos.y = -(window->height / 2) + player_render_y;
		}
	}
}
