#ifndef INCLUDE_PROTOCOL
#define INCLUDE_PROTOCOL

#include <projectbase/project_base.h>

typedef enum t_network_message_type {
	MESSAGE_GET_ID_UPSTREAM,
	MESSAGE_GET_ID_DOWNSTREAM,
	MESSAGE_USER_LIST,
	MESSAGE_USER_MOVED,
	MESSAGE_USER_LOOK,
	MESSAGE_ZOMBIE_LIST,
	MESSAGE_USER_SHOOT,
	MESSAGE_BULLET_LIST,
} network_message_type;

typedef struct t_protocol_generic_client_message {
	network_message_type type;
} protocol_generic_client_message;

typedef struct t_protocol_generic_message {
	network_client client;
	protocol_generic_client_message *message;
} protocol_generic_message;

typedef struct t_protocol_get_id_upstream {
	network_message_type type;
} protocol_get_id_upstream;

typedef struct t_protocol_get_id_downstream {
	network_message_type type;
	u32 id;
} protocol_get_id_downstream;

typedef struct t_protocol_user_list {
	network_message_type type;
	player players[10];
} protocol_user_list;

typedef struct t_protocol_zombie_list {
	network_message_type type;
	zombie zombies[20];
} protocol_zombie_list;

typedef enum t_protocol_move_type {
	MOVE_UP,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT,
} protocol_move_type;

typedef struct t_protocol_move {
	network_message_type type;
	protocol_move_type move;
	u32 id;
} protocol_move;

typedef struct t_protocol_user_look {
	network_message_type type;
	u32 id;
	float gunx;
	float guny;
} protocol_user_look;

#include "bullets.h"

typedef struct t_protocol_bullets_list {
	network_message_type type;
	bullet bullets[500];
} protocol_bullets_list;

typedef struct t_protocol_user_shoot {
	network_message_type type;
	u32 id;
	float dirx;
	float diry;
} protocol_user_shoot;

#define MAX_NETWORK_BUFFER_SIZE 50000
u8 network_buffer[50000];
network_message create_protocol_get_id_up();
network_message create_protocol_get_id_down(u32 id);
network_message create_protocol_user_list();
network_message create_protocol_user_moved(protocol_move_type move, u32 id);
network_message create_protocol_user_look(u32 id, float gunx, float guny);
network_message create_protocol_user_shoot(u32 id, float dirx, float diry);
network_message create_protocol_zombie_list();
network_message create_protocol_bullets_list();

array messages_received_on_server;
array messages_received_on_client;

void server_on_message_received(u8* buffer, u32 length, network_client client);
void client_on_message_received(u8* buffer, u32 length);

#endif