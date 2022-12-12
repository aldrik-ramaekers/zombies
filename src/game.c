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

void update_server() {
	for (int i = 0; i < messages_received_on_server.length; i++) {
		protocol_generic_message* msg = *(protocol_generic_message**)array_at(&messages_received_on_server, i);

		switch (msg->type)
		{
		case MESSAGE_GET_ID_UPSTREAM: {
			network_client_send(&msg->client, create_protocol_get_id_down(current_id));
			current_id++;
		} break;
		
		default:
			log_info("Unhandled message received");
			break;
		}

		array_remove_at(&messages_received_on_server, i);
		i--;
	}
}

void update_client() {
	for (int i = 0; i < messages_received_on_client.length; i++) {
		protocol_generic_client_message* msg = *(protocol_generic_client_message**)array_at(&messages_received_on_client, i);

		switch (msg->type)
		{
		case MESSAGE_GET_ID_DOWNSTREAM: {
			protocol_get_id_downstream* msg_id = (protocol_get_id_downstream*)msg;
			my_id = msg_id->id;
			global_state.network_state = CONNECTED;
			spawn_player(my_id);
			log_info("Id received, spawning player");
		} break;
		
		default:
			log_info("Unhandled message received");
			break;
		}

		array_remove_at(&messages_received_on_client, i);
		i--;
	}
}

void update_game(platform_window* window) {
	if (global_state.server) {
		update_server();
	}

	update_client();

	if (global_state.network_state == CONNECTED) {
		draw_grid(window);
		draw_spawners(window);
	}
}