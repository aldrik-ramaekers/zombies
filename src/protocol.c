#include "../include/protocol.h"
#include "../include/players.h"

network_message create_protocol_get_id_up()
{
	protocol_get_id_upstream *buf = (protocol_get_id_upstream *)network_buffer;
	buf->type = MESSAGE_GET_ID_UPSTREAM;
	return network_create_message(network_buffer, sizeof(protocol_get_id_upstream), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_get_id_down(u32 id)
{
	protocol_get_id_downstream *buf = (protocol_get_id_downstream *)network_buffer;
	buf->type = MESSAGE_GET_ID_DOWNSTREAM;
	buf->id = id;
	return network_create_message(network_buffer, sizeof(protocol_get_id_downstream), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_user_list()
{
	protocol_user_list *buf = (protocol_user_list *)network_buffer;
	buf->type = MESSAGE_USER_LIST;
	memcpy(buf->players, players, sizeof(players));
	return network_create_message(network_buffer, sizeof(protocol_user_list), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_zombie_list()
{
	protocol_zombie_list *buf = (protocol_zombie_list *)network_buffer;
	buf->type = MESSAGE_ZOMBIE_LIST;
	memcpy(buf->zombies, zombies, sizeof(zombies));
	return network_create_message(network_buffer, sizeof(protocol_zombie_list), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_bullets_list()
{
	protocol_bullets_list *buf = (protocol_bullets_list *)network_buffer;
	buf->type = MESSAGE_BULLET_LIST;
	memcpy(buf->bullets, bullets, sizeof(bullets));
	return network_create_message(network_buffer, sizeof(protocol_bullets_list), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_user_moved(protocol_move_type move, u32 id)
{
	protocol_move *buf = (protocol_move *)network_buffer;
	buf->type = MESSAGE_USER_MOVED;
	buf->move = move;
	buf->id = id;
	return network_create_message(network_buffer, sizeof(protocol_move), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_user_shoot(u32 id, float dirx, float diry)
{
	protocol_user_shoot *buf = (protocol_user_shoot *)network_buffer;
	buf->type = MESSAGE_USER_SHOOT;
	buf->id = id;
	buf->dirx = dirx;
	buf->diry = diry;
	return network_create_message(network_buffer, sizeof(protocol_user_shoot), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_user_look(u32 id, float gunx, float guny)
{
	protocol_user_look *buf = (protocol_user_look *)network_buffer;
	buf->type = MESSAGE_USER_LOOK;
	buf->id = id;
	buf->gunx = gunx;
	buf->guny = guny;
	return network_create_message(network_buffer, sizeof(protocol_user_look), MAX_NETWORK_BUFFER_SIZE);
}

void server_on_message_received(u8 *buffer, u32 length, u64 timestamp, network_client client)
{
	protocol_generic_message *allocated_buf = mem_alloc(sizeof(protocol_generic_message));
	allocated_buf->client = client;
	allocated_buf->message = mem_alloc(length);
	allocated_buf->send_timestamp = timestamp;
	memcpy(allocated_buf->message, buffer, length);
	array_push(&messages_received_on_server, (u8 *)&allocated_buf);
}

void client_on_message_received(u8 *buffer, u32 length)
{
	u8 *allocated_buf = mem_alloc(length);
	memcpy(allocated_buf, buffer, length);
	array_push(&messages_received_on_client, (u8 *)&allocated_buf);
}
