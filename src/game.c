#include "../include/game.h"
#include "../include/pathfinding.h"

static void server_on_client_disconnect(network_client c) {
	for (int i = 0; i < MAX_PLAYERS; i++) {
		player p = players[i];
		if (p.client.ConnectSocket == c.ConnectSocket) players[i].active = false;
	}
}

void start_server(char* port) {
	server_incomming_allocator = create_allocator(MAX_NETWORK_BUFFER_SIZE);

	messages_received_on_server = array_create(sizeof(protocol_generic_message*));
	array_reserve(&messages_received_on_server, 100);

	global_state.server = networking_create_server();
	global_state.server->on_message = server_on_message_received;
	global_state.server->on_client_disconnect = server_on_client_disconnect;
}

static u32 get_session_id() {
	u64 time = platform_get_time(TIME_FULL, TIME_NS);
	return (((time * 2654435789U) + time) * 2654435789U) + platform_get_processid();
	
}

void connect_to_server(char* ip, char* port) {
	client_incomming_allocator = create_allocator(MAX_NETWORK_BUFFER_SIZE);

	player_id = get_session_id();
	messages_received_on_client = array_create(sizeof(protocol_generic_message*));
	array_reserve(&messages_received_on_client, 100);

	global_state.network_state = CONNECTING;
	global_state.client = network_connect_to_server(ip, port);
	global_state.client->on_message = client_on_message_received;

	log_infox("Session id: %u", player_id);

	if (global_state.server) {
		player_id = player_id;
		spawn_player(player_id, (network_client){0, false, 0, "Host"});
		global_state.network_state = CONNECTED;
	}
	else {
		if (global_state.client->is_connected) {
			global_state.network_state = WAITING_FOR_ID;

			network_message message = create_protocol_get_id_up(player_id);
			add_message_to_outgoing_queuex(message, *global_state.client);
		}
	}
}

void load_map() {
	global_state.state = GAMESTATE_LOADING_MAP;

	outgoing_allocator = create_allocator(MAX_NETWORK_BUFFER_SIZE);
	messages_to_send_queue_mutex = mutex_create();

	for (int i = 0; i < 3; i++) {
		thread send_thread = thread_start(network_send_thread, 0);
		thread_detach(&send_thread);
	}

	load_map_from_data();
	create_objects();

	pathfinding_init();
	
	thread t = thread_start(pathfinding_thread, 0);
	thread_detach(&t);

	global_state.state = GAMESTATE_PLAYING;
	log_info("Done loading map");
}

void init_game() {
	global_state.state = IDLE;
	global_state.network_state = DISCONNECTED;
	
	load_assets();
	load_map();
}

void destroy_game() {
	if (global_state.client) network_client_close(global_state.client);
	if (global_state.server) networking_destroy_server(global_state.server);
}

static void broadcast_to_clients(network_message message) {
	if (!global_state.server || !global_state.server->is_open) return;

	send_queue_entry entry = {0};
	entry.message = message;
	for (int i = 0; i < MAX_PLAYERS; i++) {
		player p = players[i];
		if (!p.client.is_connected) continue;
		if (!p.active) continue;
		entry.recipients[i] = p.client;
	}
	add_message_to_outgoing_queue(entry);
}

static void rotate_user(platform_window* window, protocol_user_look *message) {
	player* p = get_player_by_id(message->id);
	if (p == 0) {
		log_info("Unknown user rotated");
		return;
	}

	p->gunx = p->playerx + message->gunx;
	p->guny = p->playery + message->guny;
}

static void set_ping_for_player(protocol_generic_message* msg) {
	u64 diff = platform_get_time(TIME_FULL, TIME_MILI_S) - msg->send_timestamp;

	for (int i = 0; i < MAX_PLAYERS; i++) {
		player p = players[i];
		if (!p.client.is_connected) continue;
		if (!p.active) continue;
		if (p.client.ConnectSocket == msg->client.ConnectSocket) {
			players[i].ping = diff;
			return;
		}
	}
}

float update_timer = 0.0f;
void update_server(platform_window* window) {
	logic_update_time = platform_get_time(TIME_FULL, TIME_NS);

	mutex_lock(&messages_received_on_server.mutex);

	for (int i = 0; i < messages_received_on_server.length; i++) {
		protocol_generic_message* msg = *(protocol_generic_message**)array_at(&messages_received_on_server, i);
		set_ping_for_player(msg);

		switch (msg->message->type)
		{
			case MESSAGE_GET_ID_UPSTREAM: {
				protocol_get_id_upstream* m = (protocol_get_id_upstream*)msg->message;
				add_message_to_outgoing_queuex(create_protocol_get_id_down(m->id), msg->client);
				spawn_player(m->id, msg->client);
				log_info("Player connected to server");
			} break;

			case MESSAGE_USER_MOVED: {
				protocol_move* move_msg = (protocol_move*)msg->message;
				move_user(window,  move_msg->id, move_msg->move, move_msg->delta);
			} break;

			case MESSAGE_USER_LOOK: {
				rotate_user(window, (protocol_user_look*)msg->message);
			} break;

			case MESSAGE_USER_SHOOT: {
				protocol_user_shoot* shoot_msg = (protocol_user_shoot*)msg->message;
				shoot(window, shoot_msg->id, shoot_msg->dirx, shoot_msg->diry);
			} break;
			
			default:
				log_info("Unhandled message received");
				break;
		}

		array_remove_at(&messages_received_on_server, i);
		i--;
	}
	
	allocator_clear(&server_incomming_allocator);
	mutex_unlock(&messages_received_on_server.mutex);

	if (update_timer >= SERVER_TICK_RATE) { // send at 60 ticks
		update_spawners_server();
		update_drops_server();
		update_wallitems_server();
		update_bullets_server(window);
		update_players_server();
		update_zombies_server(window);

		broadcast_to_clients(create_protocol_user_list());
		broadcast_to_clients(create_protocol_zombie_list());
		broadcast_to_clients(create_protocol_bullets_list());
		broadcast_to_clients(create_protocol_drop_list());
		update_timer = 0.0f;
	}

	update_timer += update_delta;

	logic_update_time = platform_get_time(TIME_FULL, TIME_NS) - logic_update_time;
	if ((logic_update_time/1000000.0f) > 5.0f) {
		log_infox("Server update took %.2fms", (logic_update_time/1000000.0f));
	}
}

static void apply_user_list(protocol_user_list* msg_players) {
	player* p = get_player_by_id(player_id);
	player copy;
	if (p) copy = *p;
	memcpy(players, msg_players->players, sizeof(players));

	// These properties are simulated locally so dont overwrite.
	if (p) {
		p->playerx = copy.playerx;
		p->playery = copy.playery;
		p->gunx = copy.gunx;
		p->guny = copy.guny;
		p->gun_height = copy.gun_height;
	}
}

static void load_bullets_into_existing_list(protocol_bullets_list* msg_bullets) {
	for (int i = 0; i < max_bullets; i++) {
		if (bullets[i].active) continue;
		for (int x = 0; x < max_bullets; x++) {
			if (!msg_bullets->bullets[x].active) continue;
			bullets[i] = msg_bullets->bullets[x];
			//bullets[i].alive_time = 0.0f;
			msg_bullets->bullets[x].active = false;
		}
	}
}

void update_client(platform_window* window) {
	logic_update_time = platform_get_time(TIME_FULL, TIME_NS);
	mutex_lock(&messages_received_on_client.mutex);

	for (int i = 0; i < messages_received_on_client.length; i++) {
		protocol_generic_client_message* msg = *(protocol_generic_client_message**)array_at(&messages_received_on_client, i);

		switch (msg->type)
		{
		case MESSAGE_GET_ID_DOWNSTREAM: {
			protocol_get_id_downstream* msg_id = (protocol_get_id_downstream*)msg;
			player_id = msg_id->id;
			global_state.network_state = CONNECTED;
			log_infox("Id received: %u", player_id);
		} break;

		case MESSAGE_USER_LIST: {
			if (global_state.server) break; // players are simulated on server so dont overwrite data.
			protocol_user_list* msg_players = (protocol_user_list*)msg;
			apply_user_list(msg_players);
		} break;

		case MESSAGE_ZOMBIE_LIST: {
			if (global_state.server) break; // zombies are simulated on server so dont overwrite data.
			protocol_zombie_list* msg_zombies = (protocol_zombie_list*)msg;
			memcpy(zombies, msg_zombies->zombies, sizeof(zombies));
		} break;

		case MESSAGE_BULLET_LIST: {
			if (global_state.server) break; // bullets are simulated on server so dont overwrite data.
			protocol_bullets_list* msg_bullets = (protocol_bullets_list*)msg;
			load_bullets_into_existing_list(msg_bullets);
		} break;

		case MESSAGE_DROP_LIST: {
			if (global_state.server) break; // drops are simulated on server so dont overwrite data.
			protocol_drop_list* msg_drops = (protocol_drop_list*)msg;
			memcpy(drops, msg_drops->drops, sizeof(drops));	
		} break;
		default:
			log_info("Unhandled message received");
			break;
		}

		array_remove_at(&messages_received_on_client, i);
		i--;
	}

	allocator_clear(&client_incomming_allocator);
	mutex_unlock(&messages_received_on_client.mutex);
	logic_update_time = platform_get_time(TIME_FULL, TIME_NS) - logic_update_time;
}

void update_game(platform_window* window) {
	clear_bullets();

	if (global_state.server) {
		update_server(window);
	}
	else {
		update_client(window);
	}
	
	if (global_state.network_state == CONNECTED) {
		if (!global_state.server) {
			update_zombies_client(window); // move to update_client?
		}
		take_player_input(window);

		draw_grid(window);
		draw_wallitems(window);
		draw_drops(window);
		draw_bullets(window);
		draw_zombies(window);
		draw_players(window);
		draw_spawners(window);
		draw_overlay(window);

		_global_camera.x = (int)_next_camera_pos.x;
		_global_camera.y = (int)_next_camera_pos.y;
	}
}