#include "../include/game.h"
#include "../include/pathfinding.h"

bool game_is_paused = false;

static void server_on_client_disconnect(network_client c) {
	for (int i = 0; i < MAX_PLAYERS; i++) {
		player p = players[i];
		if (p.client.ConnectSocket == c.ConnectSocket) {
			//players[i].active = false;
			players[i].connection_state = DISCONNECTED;
			network_client_close(&players[i].client);
			return;
		}
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

static void client_on_disconnect()
{
	clear_players();
	clear_zombies();
	global_scene_state = SCENE_MAIN_MENU;
	global_state.state = GAMESTATE_PLAYING;
	current_menu_state = MENU_STATE_MAIN;
	network_client_close(global_state.client);
	global_state.server = 0;
	global_state.client = 0;
}

bool connect_to_server(char* ip, char* port) {
	client_incomming_allocator = create_allocator(MAX_NETWORK_BUFFER_SIZE);

	messages_received_on_client = array_create(sizeof(protocol_generic_message*));
	array_reserve(&messages_received_on_client, 100);

	global_state.network_state = CONNECTING;
	global_state.client = network_connect_to_server(ip, port);
	global_state.client->on_message = client_on_message_received;
	global_state.client->on_disconnect = client_on_disconnect;

	if (global_state.server) {
		player_id = 127001;
		spawn_player(player_id, (network_client){0, false, 0, 0, "Host"});
		global_state.network_state = CONNECTED;
		return true;
	}
	else {
		if (global_state.client->is_connected) {
			global_state.network_state = WAITING_FOR_ID;

			player_id = get_session_id();
			network_message message = create_protocol_get_id_up(player_id);
			add_message_to_outgoing_queuex(message, *global_state.client);
			return true;
		}
	}
	return false;
}

bool connect_to_game(char* ip, char* port)
{
	if (ip && port) {
		if (strcmp(ip, "127.0.0.1") == 0) {
			start_server(port);
		}
		if (!connect_to_server(ip, port)) {
			return false;
		}
	}

	log_info("STATE: GAMESTATE_PLAYING");
	global_state.state = GAMESTATE_PLAYING;

	play_music(music_inside1);
	return true;
}

void start_solo_game()
{
	char* ip = "127.0.0.1";
	char* port = "27015";
	start_server(port);
	connect_to_server(ip, port);

	log_info("STATE: GAMESTATE_PLAYING");
	global_state.state = GAMESTATE_PLAYING;

	play_music(music_inside1);
}

void load_map() {
	log_info("STATE: GAMESTATE_LOADING_MAP");
	global_state.state = GAMESTATE_LOADING_MAP;

	outgoing_allocator = create_allocator(MAX_NETWORK_BUFFER_SIZE);
	messages_to_send_queue_mutex = mutex_create();

	for (int i = 0; i < 1; i++) {
		thread send_thread = thread_start(network_send_thread, 0);
		thread_detach(&send_thread);
	}

	//create_empty_map();
	load_map_from_file();
	create_objects();

	pathfinding_init();
	
	// TODO: only need this on server?
	for (int i = 0; i < SERVER_PATHFINDING_THREADS; i++)
	{
		thread t1 = thread_start(pathfinding_thread, 0);
		thread_detach(&t1);
	}

	log_info("STATE: GAMESTATE_LOADING_ASSETS");
	global_state.state = GAMESTATE_LOADING_ASSETS;
}

void fill_game_structs()
{
	glass_doors = calloc(MAX_GLASS_DOORS, sizeof(glass_door));
	spawner_tiles = calloc(MAX_SPAWNERS, sizeof(spawner));
	zombies = calloc(SERVER_MAX_ZOMBIES, sizeof(zombie));

	map_to_load = malloc(sizeof(map_data));
	loaded_map = malloc(sizeof(extracted_map_data));

	memset(glass_doors, 0, MAX_GLASS_DOORS*sizeof(glass_door));
	memset(spawner_tiles, 0, MAX_SPAWNERS*sizeof(spawner));
	memset(zombies, 0, SERVER_MAX_ZOMBIES*sizeof(zombie));

	memset(map_to_load, 0, sizeof(map_data));
	memset(loaded_map, 0, sizeof(extracted_map_data));

	global_audio_source_position = (vec3f){-1,-1,-1};

	object _localobject_dict[OBJECT_END] = {
	{0,(vec3f){0, 0, 0},{1,3,0.5f},OBJECT_SPACE_CONTROL_PANEL, 1},
	{0,(vec3f){0, 0, 0},{1,1,1},OBJECT_SPACE_WINDOW, 1},
	{0,(vec3f){0, 0, 0},{1,1,1},OBJECT_METAL_WALL, 1},
	{0,(vec3f){0, 0, 0},{1,1,1},OBJECT_METAL_WALL2, 1},
	{0,(vec3f){0, 0, 0},{3,1,0.5f},OBJECT_SPACE_CONTROL_PANEL2, 1},
	{0,(vec3f){0, 0, 0},{1,1,0.5f},OBJECT_CHAIR_UP, 1},
	{0,(vec3f){0, 0, 0},{1,1,1},OBJECT_SPACE_WINDOW_H, 1},
	{0,(vec3f){0, 0, 0},{1,1,0.5},OBJECT_ZOMBIE_SPAWNER, 1},
	{0,(vec3f){0, 0, 0},{1,1,0},OBJECT_LAMP_EAST, 0},
	{0,(vec3f){0, 0, 0},{3,1,0.5},OBJECT_METAL_TABLE_H, 1},
	{0,(vec3f){0, 0, 0},{2,2,0.5},OBJECT_CANTINE_GLASS, 1},
	{0,(vec3f){0, 0, 0},{2,1,1},OBJECT_GLASS_DOOR_H, 0},
	{0,(vec3f){0, 0, 0},{1,1,1},OBJECT_LAMP_SOUTH, 0},
	{0,(vec3f){0, 0, 0},{3,7,0.5f},OBJECT_BIG_CONTROL_PANEL, 1},
	{0,(vec3f){0, 0, 0},{1,1,0.5f},OBJECT_CHAIR_DOWN, 1},
	{0,(vec3f){0, 0, 0},{2,1,1.0f},OBJECT_COMPUTER_RACK, 1},
	{0,(vec3f){0, 0, 0},{8,2,1.0f},OBJECT_GENERATOR, 1},
	{0,(vec3f){0, 0, 0},{1,4,0.5f},OBJECT_SCHOOL_TABLE, 1},
	{0,(vec3f){0, 0, 0},{2,1,0.5f},OBJECT_SCHOOL_BENCH, 1},
	{0,(vec3f){0, 0, 0},{1,1,1.0f},OBJECT_LOCKER, 1},
	{0,(vec3f){0, 0, 0},{1,1,0.5f},OBJECT_BAR_H, 1},
	{0,(vec3f){0, 0, 0},{1,1,0.5f},OBJECT_BAR_V, 1},
	{0,(vec3f){0, 0, 0},{1,1,0.5f},OBJECT_BAR_HV, 1},
	{0,(vec3f){0, 0, 0},{1,1,0.5f},OBJECT_CLUB_SEAT1, 1},
	{0,(vec3f){0, 0, 0},{1,1,0.5f},OBJECT_CLUB_SEAT2, 1},
	{0,(vec3f){0, 0, 0},{1,1,0.5f},OBJECT_CLUB_SEAT3, 1},
	{0,(vec3f){0, 0, 0},{1,1,0.5f},OBJECT_CLUB_SEAT4, 1},
	{0,(vec3f){0, 0, 0},{1,1,0.5f},OBJECT_CLUB_WALL1, 1},
	{0,(vec3f){0, 0, 0},{1,1,0.5f},OBJECT_CLUB_WALL2, 1},
	{0,(vec3f){0, 0, 0},{1,1,1.0f},OBJECT_CLUB_WALL3, 1},
	{0,(vec3f){0, 0, 0},{1,1,0.5f},OBJECT_CLUB_SEAT5, 1},
	{0,(vec3f){0, 0, 0},{1,1,0.5f},OBJECT_CLUB_SEAT6, 1},
	{0,(vec3f){0, 0, 0},{1,1,0.5f},OBJECT_CLUB_SEAT7, 1},
	{0,(vec3f){0, 0, 0},{13,3,0},OBJECT_BOWLING_LANE, 0},
	{0,(vec3f){0, 0, 0},{2,3,1},OBJECT_BOWLING_LANE_END, 1},
	{0,(vec3f){0, 0, 0},{1,2,1},OBJECT_GLASS_DOOR_V, 0},
	{0,(vec3f){0, 0, 0},{3,1,0.5f},OBJECT_BOWLING_MACHINE, 1},
	{0,(vec3f){0, 0, 0},{2,1,1},OBJECT_PAINTING1, 0},
	{0,(vec3f){0, 0, 0},{2,1,1},OBJECT_PAINTING2, 0},
	{0,(vec3f){0, 0, 0},{2,1,1},OBJECT_PAINTING3, 0},
	{0,(vec3f){0, 0, 0},{2,1,1},OBJECT_PAINTING4, 0},
	{0,(vec3f){0, 0, 0},{2,1,1},OBJECT_PAINTING5, 0},
	{0,(vec3f){0, 0, 0},{2,1,1},OBJECT_PAINTING6, 0},
	{0,(vec3f){0, 0, 0},{2,1,1},OBJECT_PAINTING7, 0},
	{0,(vec3f){0, 0, 0},{2,1,1},OBJECT_PAINTING8, 0},
	{0,(vec3f){0, 0, 0},{2,1,1},OBJECT_PAINTING9, 0},
	{0,(vec3f){0, 0, 0},{2,1,1},OBJECT_PAINTING10, 0},
	{0,(vec3f){0, 0, 0},{10,10,0},OBJECT_FLOORMAT1, 0},
	{0,(vec3f){0, 0, 0},{10,10,0},OBJECT_FLOORMAT2, 0},
	{0,(vec3f){0, 0, 0},{1,1,1},OBJECT_METAL_WALL3, 1},
	{0,(vec3f){0, 0, 0},{1,1,1},OBJECT_METAL_WALL4, 1},
	{0,(vec3f){0, 0, 0},{1,1,1},OBJECT_METAL_WALL5, 1},
	{0,(vec3f){0, 0, 0},{1,1,1},OBJECT_METAL_WALL6, 1},
	{0,(vec3f){0, 0, 0},{1,1,1},OBJECT_METAL_WALL7, 1},
	{0,(vec3f){0, 0, 0},{1,1,1},OBJECT_METAL_WALL8, 1},
	{0,(vec3f){0, 0, 0},{1,1,1},OBJECT_METAL_WALL9, 1},
	{0,(vec3f){0, 0, 0},{1,1,1},OBJECT_METAL_WALL10, 1},
	};
	// @NEWOBJECT
	memcpy(object_dict, _localobject_dict, sizeof(_localobject_dict));

	vec2 _local_available_resolutions[] = {
	(vec2){1366, 769},
	(vec2){1440, 900},
	(vec2){1600, 900},
	(vec2){1920, 1080},
	(vec2){2560, 1080},
	(vec2){2560, 1440},
	(vec2){2560, 1600},
	(vec2){3440, 1440},
	(vec2){3840, 2160},
	(vec2){5120, 1440},
	};
	memcpy(available_resolutions, _local_available_resolutions, sizeof(_local_available_resolutions));
}

void init_game() {
	log_info("STATE: GAMESTATE_IDLE");
	global_state.state = GAMESTATE_IDLE;
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
	if (p->interact_state == INTERACT_DEAD) return;

	p->gunx = p->playerx + message->gunx;
	p->guny = p->playery - 0.5f + message->guny;
	p->dirx = message->dirx;
	p->diry = message->diry;
}

bool every_player_died()
{
	for (int i = 0; i < MAX_PLAYERS; i++) {
		player p = players[i];
		if (!p.active) continue;
		// TODO: if player is disconnected for 2+ min, kill player.
		if (p.health != 0) return false;
	}
	return true;
}

static void set_ping_for_player(protocol_generic_message* msg) {
	u64 diff = platform_get_time(TIME_FULL, TIME_MILI_S) - msg->send_timestamp;

	for (int i = 0; i < MAX_PLAYERS; i++) {
		player p = players[i];
		if (p.client.ConnectSocket == msg->client.ConnectSocket) {
			players[i].ping = diff;
			return;
		}
	}
}

static u32 get_id_from_ip(network_client client) {
	u32 result = 0;
	int i = 0;
	while (client.ip[i] != '\0') {
        if (client.ip[i] < '0' || client.ip[i] > '9') {
			i++;
            continue;
        }
        result = result * 10 + (client.ip[i] - '0');
        i++;
    }

	return result;
}

float update_timer = 0.0f;
void update_server(platform_window* window) {
	logic_update_time = platform_get_time(TIME_FULL, TIME_NS);

	mutex_lock(&messages_received_on_server.mutex);

	for (int i = 0; i < messages_received_on_server.length; i++) {
		protocol_generic_message* msg = *(protocol_generic_message**)array_at(&messages_received_on_server, i);
		set_ping_for_player(msg);

		// Ignore all messages except join request when game is paused.
		if (!game_is_paused || msg->message->type == MESSAGE_GET_ID_UPSTREAM)
		{
			switch (msg->message->type)
			{
				case MESSAGE_PING_UPSTREAM: {
					add_message_to_outgoing_queuex(create_protocol_ping_downstream(), msg->client);

				} break;
				case MESSAGE_GET_ID_UPSTREAM: {
					protocol_get_id_upstream* m = (protocol_get_id_upstream*)msg->message;
					u32 new_id = get_id_from_ip(msg->client);

					if (player_has_old_session(new_id)) {
						rejoin_player(new_id, msg->client);
						log_infox("Player rejoined session / ip: %s id: %d", msg->client.ip, new_id);
					}
					else {
						spawn_player(new_id, msg->client);
						log_infox("Player connected to session / ip: %s id: %d", msg->client.ip, new_id);
					}
					
					add_message_to_outgoing_queuex(create_protocol_get_id_down(new_id), msg->client);
					
				} break;

				case MESSAGE_USER_THROW: {
					protocol_user_throw* throw_msg = (protocol_user_throw*)msg->message;
					throw_throwable(throw_msg->id, throw_msg->throwable, throw_msg->dirx, throw_msg->diry);
				} break;

				case MESSAGE_USER_MOVED: {
					protocol_move* move_msg = (protocol_move*)msg->message;
					if (move_msg->id != player_id) move_user(window,  move_msg->id, move_msg->move, move_msg->delta);
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
		}

		array_remove_at(&messages_received_on_server, i);
		i--;
	}

	u64 handle_messages = platform_get_time(TIME_FULL, TIME_NS);
	u64 broadcast_stamp = 0;
	u64 broadcast_players = 0;
	u64 broadcast_zombies = 0;
	
	allocator_clear(&server_incomming_allocator);
	mutex_unlock(&messages_received_on_server.mutex);

	update_bullets_server(window);
	if (update_timer >= SERVER_TICK_RATE) { // send at 60 ticks
		if (!game_is_paused) {
			update_spawners_server();
			update_drops_server();
			//update_wallitems_server();
			update_throwables_server();
			update_zombie_chunks_server();
			update_round_server();
			update_points_animation_server();
			update_glass_doors_server();
		

			broadcast_players = platform_get_time(TIME_FULL, TIME_NS);
			update_players_server();
			broadcast_players = platform_get_time(TIME_FULL, TIME_NS) - broadcast_players;

			broadcast_zombies = platform_get_time(TIME_FULL, TIME_NS);
			update_zombies_server(window);
			broadcast_zombies = platform_get_time(TIME_FULL, TIME_NS) - broadcast_zombies;
		}

		broadcast_stamp = platform_get_time(TIME_FULL, TIME_NS);
		broadcast_to_clients(create_protocol_user_list());
		broadcast_to_clients(create_protocol_zombie_list());
		broadcast_to_clients(create_protocol_bullets_list());
		broadcast_to_clients(create_protocol_drop_list());
		broadcast_to_clients(create_protocol_throwables_list());
		broadcast_to_clients(create_protocol_zombie_chunk_list());
		broadcast_to_clients(create_protocol_round_data(_current_round));

		// play sounds locally and send them to clients.
		play_sounds_in_queue();
		broadcast_to_clients(create_protocol_sound_list());
		clear_sounds_in_queue();
		broadcast_stamp = platform_get_time(TIME_FULL, TIME_NS) - broadcast_stamp;

		update_timer = 0.0f;
	}

	update_timer += update_delta;

	u64 handle_messages2 = handle_messages;
	handle_messages = handle_messages  - logic_update_time;
	logic_update_time = platform_get_time(TIME_FULL, TIME_NS) - logic_update_time;
	u64 server_tick = platform_get_time(TIME_FULL, TIME_NS) - handle_messages2;
	if ((logic_update_time/1000000.0f) > 10.0f) {
		log_infox("Server update took %.2fms:\n\tmessages: %.2fms\n\ttick: %.2fms\n\t\tbroadcast: %.2fms\n\t\tplayers: %.2fms\n\t\tzombies: %.2fms\n",
			(logic_update_time/1000000.0f), (handle_messages/1000000.0f), (server_tick/1000000.0f), 
			(broadcast_stamp/1000000.0f), (broadcast_players/1000000.0f), (broadcast_zombies/1000000.0f));
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
			protocol_user_list* msg_players = (protocol_user_list*)msg;
			memcpy(players, msg_players->players, sizeof(players));
		} break;

		case MESSAGE_ROUND_DATA: {
			protocol_round* msg_round = (protocol_round*)msg;
			_current_round = msg_round->round;
			game_is_paused = msg_round->game_is_paused;
		} break;

		case MESSAGE_ZOMBIE_LIST: {
			protocol_zombie_list* msg_zombies = (protocol_zombie_list*)msg;
			memcpy(zombies, msg_zombies->zombies, sizeof(zombies));
		} break;

		case MESSAGE_ZOMBIE_CHUNK_LIST: {
			protocol_zombie_chunk_list* msg_zombies = (protocol_zombie_chunk_list*)msg;
			memcpy(zombie_chunks, msg_zombies->zombie_chunks, sizeof(zombie_chunks));
		} break;

		case MESSAGE_THROWABLES_LIST: {
			protocol_throwables_list* msg_throwables = (protocol_throwables_list*)msg;
			memcpy(throwables, msg_throwables->throwables, sizeof(throwables));
		} break;

		case MESSAGE_BULLET_LIST: {
			protocol_bullets_list* msg_bullets = (protocol_bullets_list*)msg;
			load_bullets_into_existing_list(msg_bullets);
		} break;

		case MESSAGE_DROP_LIST: {
			protocol_drop_list* msg_drops = (protocol_drop_list*)msg;
			memcpy(drops, msg_drops->drops, sizeof(drops));	
		} break;

		case MESSAGE_SOUND_LIST: {
			protocol_sound_list* msg_sound = (protocol_sound_list*)msg;
			memcpy(audio_events, msg_sound->audio_events, sizeof(audio_events));
			play_sounds_in_queue();
			clear_sounds_in_queue();
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

	if (!game_is_paused) {
		update_zombies_client(window);
	}
}

static void move_camera(platform_window* window) {
	static camera _camera_buffer;
	float speedx = window->width / 1000.0f;
	float diffx = (_next_camera_pos.x - _global_camera.x);
	if (abs(diffx) <= speedx) diffx = 0.0f;
	speedx += (abs(diffx)/50.0f*speedx);
	
	float speedy = window->height / 600.0f;
	float diffy = (_next_camera_pos.y - _global_camera.y);
	if (abs(diffy) <= speedy) diffy = 0.0f;
	speedy += (abs(diffy)/50.0f*speedy);

	float length = sqrt(diffx * diffx + diffy * diffy);
	if (length == 0) length = 1.0f;

	float dirx = diffx/length;
	float diry = diffy/length;

	_camera_buffer.x += dirx*speedx;
	_camera_buffer.y += diry*speedy;
	_global_camera.x = (int)_camera_buffer.x;
	_global_camera.y = (int)_camera_buffer.y;
}

void draw_splash(platform_window* window) {
	image* img = img_splash_art1;
	int imgw = img->width * (window->height/(float)img->height);
	int imgh = window->height;

	int imgx = (window->width - imgw) / 2 + _global_camera.x;
	int imgy = 0 + _global_camera.y;

	renderer->render_image(img, imgx, imgy, imgw, imgh);
}

void update_game(platform_window* window) {
	clear_bullets();

	if (global_state.state != GAMESTATE_PLAYING) {
		draw_splash(window);
		return;
	}

	if (global_state.server) {
		update_server(window);
	}
	else {
		update_client(window);
	}
	
	if (global_state.network_state == CONNECTED) {
		take_player_input(window);

		draw_grid(window);
		draw_objects(window);
		//draw_wallitems(window);
		draw_zombie_chunks(window);
		draw_drops(window);
		draw_bullets(window);
		draw_throwables(window);
		draw_round(window);
		draw_spawners(window);
		//draw_fog_of_war(window);
		draw_overlay(window);
	
#ifdef MODE_DEBUG
		draw_editor(window);
#endif

		move_camera(window);
	}
}