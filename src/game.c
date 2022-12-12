#include "../include/game.h"
#include "../include/pathfinding.h"

u32 current_id = 0;

void start_server(char* port) {
	messages_received_on_server = array_create(sizeof(protocol_generic_message*));
	array_reserve(&messages_received_on_server, 100);

	global_state.server = networking_create_server();
	global_state.server->on_message = server_on_message_received;
}

void connect_to_server(char* ip, char* port) {
	messages_received_on_client = array_create(sizeof(protocol_generic_message*));
	array_reserve(&messages_received_on_client, 100);

	global_state.network_state = CONNECTING;
	global_state.client = network_connect_to_server(ip, port);
	global_state.client->on_message = client_on_message_received;

	if (global_state.client->is_connected) {
		global_state.network_state = WAITING_FOR_ID;

		network_message message = create_protocol_get_id_up();
		network_client_send(global_state.client, message);
	}
}

void load_map() {
	global_state.state = GAMESTATE_LOADING_MAP;

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
	
	load_map();
}

void destroy_game() {
	if (global_state.server) networking_destroy_server(global_state.server);
	if (global_state.client) network_client_close(global_state.client);
}

static void broadcast_to_clients(network_message message) {
	if (!global_state.server || !global_state.server->is_open) return;
	for (int i = 0; i < global_state.server->clients.length; i++) {
		network_client* client = array_at(&global_state.server->clients, i);
		network_client_send(client, message);
	}
}

static void rotate_user(platform_window* window, protocol_user_look *message) {
	player* p = get_player_by_id(message->id);
	if (p == 0) {
		log_info("Unknown user rotated");
		return;
	}

	p->gunx = message->gunx;
	p->guny = message->guny;
}

static void move_user(platform_window* window, protocol_move *message) {
	float speed = 0.1f;
	float pad_between_player_and_obj = 0.01f;

	player* p = get_player_by_id(message->id);
	if (p == 0) {
		log_info("Unknown user moved");
		return;
	}

	if (message->move == MOVE_UP) {
		float newy = p->playery - speed;
		if (is_in_bounds(p->playerx, newy)) {
			p->playery = newy;
			object o = check_if_player_collided_with_object(window, *p);
			if (o.active) p->playery = o.position.y+o.size.y - get_player_size_in_tile() + pad_between_player_and_obj;
		}
	}

	if (message->move == MOVE_DOWN) {
		float newy = p->playery + speed;
		if (is_in_bounds(p->playerx, newy)) {
			p->playery = newy;
			object o = check_if_player_collided_with_object(window, *p);
			if (o.active) p->playery = o.position.y - get_player_size_in_tile() - pad_between_player_and_obj;
		}
	}

	if (message->move == MOVE_LEFT) {
		float newx = p->playerx - speed;
		if (is_in_bounds(newx, p->playery)) {
			p->playerx = newx;
			object o = check_if_player_collided_with_object(window, *p);
			if (o.active) p->playerx = o.position.x+o.size.x + pad_between_player_and_obj;
		}
	}

	if (message->move == MOVE_RIGHT) {
		float newx = p->playerx + speed;
		if (is_in_bounds(newx, p->playery)) {
			p->playerx = newx;
			object o = check_if_player_collided_with_object(window, *p);
			if (o.active) p->playerx = o.position.x-get_player_size_in_tile() - pad_between_player_and_obj;
		}
	}
}

float update_timer = 0.0f;
void update_server(platform_window* window) {
	update_spawners();
	update_zombies_server(window);

	for (int i = 0; i < messages_received_on_server.length; i++) {
		protocol_generic_message* msg = *(protocol_generic_message**)array_at(&messages_received_on_server, i);

		switch (msg->message->type)
		{
			case MESSAGE_GET_ID_UPSTREAM: {
				network_client_send(&msg->client, create_protocol_get_id_down(current_id));
				spawn_player(current_id);
				current_id++;
				log_info("Player connected to server");
			} break;

			case MESSAGE_USER_MOVED: {
				move_user(window, (protocol_move*)msg->message);
			} break;

			case MESSAGE_USER_LOOK: {
				rotate_user(window, (protocol_user_look*)msg->message);
			} break;
			
			default:
				log_info("Unhandled message received");
				break;
		}

		mem_free(msg->message);
		mem_free(msg);
		array_remove_at(&messages_received_on_server, i);
		i--;
	}

	broadcast_to_clients(create_protocol_user_list());
	
	if (update_timer > 0.05f) {
		broadcast_to_clients(create_protocol_zombie_list());
		update_timer = 0.0f;
	}

	update_timer += update_delta;
}

void update_client(platform_window* window) {
	for (int i = 0; i < messages_received_on_client.length; i++) {
		protocol_generic_client_message* msg = *(protocol_generic_client_message**)array_at(&messages_received_on_client, i);

		switch (msg->type)
		{
		case MESSAGE_GET_ID_DOWNSTREAM: {
			protocol_get_id_downstream* msg_id = (protocol_get_id_downstream*)msg;
			my_id = msg_id->id;
			global_state.network_state = CONNECTED;
			printf("Received id: %d\n", my_id);
			log_info("Id received");
		} break;

		case MESSAGE_USER_LIST: {
			protocol_user_list* msg_players = (protocol_user_list*)msg;
			memcpy(players, msg_players->players, sizeof(players));
		} break;
		case MESSAGE_ZOMBIE_LIST: {
			if (global_state.server) break; // zombies are simulated on server so dont overwrite data.
			protocol_zombie_list* msg_zombies = (protocol_zombie_list*)msg;
			memcpy(zombies, msg_zombies->zombies, sizeof(zombies));	
		} break;
		default:
			log_info("Unhandled message received");
			break;
		}

		mem_free(msg);
		array_remove_at(&messages_received_on_client, i);
		i--;
	}

	if (!global_state.server) {
		update_zombies_client(window);
	}
}

void update_game(platform_window* window) {
	update_client(window);
	if (global_state.server) {
		update_server(window);
	}

	if (global_state.network_state == CONNECTED) {
		draw_grid(window);
		draw_spawners(window);
	}
}