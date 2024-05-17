#ifndef INCLUDE_PROTOCOL
#define INCLUDE_PROTOCOL

#include <projectbase/project_base.h>

#include "list.h"
#include "players.h"

typedef enum t_network_message_type
{
	MESSAGE_PING_UPSTREAM,
	MESSAGE_PING_DOWNSTREAM,
	MESSAGE_GET_ID_UPSTREAM,
	MESSAGE_GET_ID_DOWNSTREAM,
	MESSAGE_USER_LIST,
	MESSAGE_USER_MOVED,
	MESSAGE_USER_LOOK,
	MESSAGE_USER_THROW,
	MESSAGE_ZOMBIE_CHUNK_LIST,
	MESSAGE_ZOMBIE_LIST,
	MESSAGE_USER_SHOOT,
	MESSAGE_BULLET_LIST,
	MESSAGE_DROP_LIST,
	MESSAGE_SOUND_LIST,
	MESSAGE_THROWABLES_LIST,
	MESSAGE_ROUND_DATA,
} network_message_type;

typedef struct t_protocol_generic_client_message
{
	network_message_type type;
} protocol_generic_client_message;

typedef struct t_protocol_generic_message
{
	network_client client;
	protocol_generic_client_message *message;
	u64 send_timestamp;
} protocol_generic_message;

typedef struct t_protocol_get_id_upstream
{
	network_message_type type;
	u32 id;
} protocol_get_id_upstream;

typedef struct t_protocol_get_id_downstream
{
	network_message_type type;
	u32 id;
} protocol_get_id_downstream;

typedef struct t_protocol_ping_downstream
{
	network_message_type type;
	char program_version[30];
} protocol_ping_downstream;

typedef struct t_protocol_user_list
{
	network_message_type type;
	player players[MAX_PLAYERS];
} protocol_user_list;

#include "audio.h"
typedef struct t_protocol_sound_list
{
	network_message_type type;
	audio_event audio_events[MAX_AUDIO_EVENTS];
} protocol_sound_list;

#include "zombies.h"
typedef struct t_protocol_zombie_list
{
	network_message_type type;
	zombie zombies[SERVER_MAX_ZOMBIES];
} protocol_zombie_list;

#include "zombie_chunk.h"
typedef struct t_protocol_zombie_chunk_list
{
	network_message_type type;
	zombie_chunk zombie_chunks[MAX_ZOMBIE_CHUNKS];
} protocol_zombie_chunk_list;

#include "drops.h"
typedef struct t_protocol_drop_list
{
	network_message_type type;
	drop drops[MAX_DROPS];
} protocol_drop_list;

#include "rounds.h"
typedef struct t_protocol_round
{
	network_message_type type;
	zombie_round round;
	bool game_is_paused;
} protocol_round;

typedef enum t_protocol_move_type
{
	MOVE_UP,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT,

	MOVE_UPRIGHT,
	MOVE_UPLEFT,
	MOVE_DOWNRIGHT,
	MOVE_DOWNLEFT,

	MOVE_NONE,
} protocol_move_type;

typedef struct t_protocol_move
{
	network_message_type type;
	protocol_move_type move;
	u32 id;
	float delta;
} protocol_move;

typedef struct t_protocol_user_throw
{
	network_message_type type;
	u32 id;
	float dirx;
	float diry;
	throwable_type throwable;
} protocol_user_throw;

typedef struct t_protocol_user_look
{
	network_message_type type;
	u32 id;
	float gunx;
	float guny;
	float dirx;
	float diry;
} protocol_user_look;

#include "bullets.h"

typedef struct t_protocol_bullets_list
{
	network_message_type type;
	bullet bullets[500];
} protocol_bullets_list;

typedef struct t_protocol_throwables_list
{
	network_message_type type;
	throwable throwables[50];
} protocol_throwables_list;

typedef struct t_protocol_user_shoot
{
	network_message_type type;
	u32 id;
	float dirx;
	float diry;
} protocol_user_shoot;

typedef struct t_send_queue_entry {
	bool active;
	network_client recipients[MAX_PLAYERS];
	network_message message;
} send_queue_entry;

#define MAX_NETWORK_BUFFER_SIZE 5000000
u8 network_buffer[MAX_NETWORK_BUFFER_SIZE];

allocator server_incomming_allocator;
allocator client_incomming_allocator;
allocator outgoing_allocator;

network_message create_protocol_ping_downstream();
network_message create_protocol_get_id_up(u32 id);
network_message create_protocol_get_id_down(u32 id);
network_message create_protocol_sound_list();
network_message create_protocol_user_list();
network_message create_protocol_user_moved(protocol_move_type move, u32 id);
network_message create_protocol_user_look(u32 id, float gunx, float guny, float dirx, float diry);
network_message create_protocol_user_shoot(u32 id, float dirx, float diry);
network_message create_protocol_user_throw(u32 id, float dirx, float diry, throwable_type type);
network_message create_protocol_zombie_list();
network_message create_protocol_zombie_chunk_list();
network_message create_protocol_bullets_list();
network_message create_protocol_drop_list();
network_message create_protocol_throwables_list();
network_message create_protocol_round_data(zombie_round round);

array messages_received_on_server;
array messages_received_on_client;

#define OUTGOING_QUEUE_SIZE 100
mutex messages_to_send_queue_mutex;
send_queue_entry messages_to_send_queue[OUTGOING_QUEUE_SIZE] = {0};

void add_message_to_outgoing_queuex(network_message message, network_client c);
void add_message_to_outgoing_queue(send_queue_entry entry);
void* network_send_thread(void* args);

void server_on_message_received(u8 *buffer, u32 length, u64 timestamp, network_client client);
void client_on_message_received(u8 *buffer, u32 length);

#endif