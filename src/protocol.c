#include "../include/protocol.h"
#include "../include/players.h"

#define alloc_network_message(_type) mem_alloc(sizeof(_type) + NETWORK_PACKET_OVERHEAD);

network_message create_protocol_get_id_up(u32 id)
{
	protocol_get_id_upstream *buf = alloc_network_message(protocol_get_id_upstream);
	buf->type = MESSAGE_GET_ID_UPSTREAM;
	buf->id = id;
	return network_create_message((u8*)buf, sizeof(protocol_get_id_upstream), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_get_id_down(u32 id)
{
	protocol_get_id_downstream *buf = alloc_network_message(protocol_get_id_downstream);
	buf->type = MESSAGE_GET_ID_DOWNSTREAM;
	buf->id = id;
	return network_create_message((u8*)buf, sizeof(protocol_get_id_downstream), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_sound_list()
{
	protocol_sound_list *buf = alloc_network_message(protocol_sound_list);
	buf->type = MESSAGE_SOUND_LIST;
	memcpy(buf->audio_events, audio_events, sizeof(audio_events));
	return network_create_message((u8*)buf, sizeof(protocol_sound_list), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_user_list()
{
	protocol_user_list *buf = alloc_network_message(protocol_user_list);
	buf->type = MESSAGE_USER_LIST;
	memcpy(buf->players, players, sizeof(players));
	return network_create_message((u8*)buf, sizeof(protocol_user_list), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_throwables_list()
{
	protocol_throwables_list *buf = alloc_network_message(protocol_throwables_list);
	buf->type = MESSAGE_THROWABLES_LIST;
	memcpy(buf->throwables, throwables, sizeof(throwables));
	return network_create_message((u8*)buf, sizeof(protocol_throwables_list), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_zombie_list()
{
	protocol_zombie_list *buf = alloc_network_message(protocol_zombie_list);
	buf->type = MESSAGE_ZOMBIE_LIST;
	memcpy(buf->zombies, zombies, sizeof(zombies));
	return network_create_message((u8*)buf, sizeof(protocol_zombie_list), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_bullets_list()
{
	protocol_bullets_list *buf = alloc_network_message(protocol_bullets_list);
	buf->type = MESSAGE_BULLET_LIST;
	memcpy(buf->bullets, bullets, sizeof(bullets));
	return network_create_message((u8*)buf, sizeof(protocol_bullets_list), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_drop_list()
{
	protocol_drop_list *buf = alloc_network_message(protocol_drop_list);
	buf->type = MESSAGE_DROP_LIST;
	memcpy(buf->drops, drops, sizeof(drops));
	return network_create_message((u8*)buf, sizeof(protocol_drop_list), MAX_NETWORK_BUFFER_SIZE);
}


network_message create_protocol_user_moved(protocol_move_type move, u32 id)
{
	protocol_move *buf = alloc_network_message(protocol_move);
	buf->type = MESSAGE_USER_MOVED;
	buf->move = move;
	buf->id = id;
	buf->delta = update_delta;
	return network_create_message((u8*)buf, sizeof(protocol_move), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_user_throw(u32 id, float dirx, float diry, throwable_type type)
{
	protocol_user_throw *buf = alloc_network_message(protocol_user_throw);
	buf->type = MESSAGE_USER_THROW;
	buf->id = id;
	buf->dirx = dirx;
	buf->diry = diry;
	buf->throwable = type;
	return network_create_message((u8*)buf, sizeof(protocol_user_throw), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_user_shoot(u32 id, float dirx, float diry)
{
	protocol_user_shoot *buf = alloc_network_message(protocol_user_shoot);
	buf->type = MESSAGE_USER_SHOOT;
	buf->id = id;
	buf->dirx = dirx;
	buf->diry = diry;
	return network_create_message((u8*)buf, sizeof(protocol_user_shoot), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_user_look(u32 id, float gunx, float guny)
{
	protocol_user_look *buf = alloc_network_message(protocol_user_look);
	buf->type = MESSAGE_USER_LOOK;
	buf->id = id;
	buf->gunx = gunx;
	buf->guny = guny;
	return network_create_message((u8*)buf, sizeof(protocol_user_look), MAX_NETWORK_BUFFER_SIZE);
}

void server_on_message_received(u8 *buffer, u32 length, u64 timestamp, network_client client)
{
	mutex_lock(&messages_received_on_server.mutex);
	{
		protocol_generic_message *allocated_buf = allocator_alloc(&server_incomming_allocator, sizeof(protocol_generic_message));
		allocated_buf->client = client;
		allocated_buf->message = allocator_alloc(&server_incomming_allocator, length);
		allocated_buf->send_timestamp = timestamp;
		memcpy(allocated_buf->message, buffer, length);
		array_push(&messages_received_on_server, (u8 *)&allocated_buf);
	}
	mutex_unlock(&messages_received_on_server.mutex);
}

void client_on_message_received(u8 *buffer, u32 length)
{
	mutex_lock(&messages_received_on_client.mutex);
	{
		u8 *allocated_buf = allocator_alloc(&client_incomming_allocator, length);
		memcpy(allocated_buf, buffer, length);
		array_push(&messages_received_on_client, (u8 *)&allocated_buf);
	}
	mutex_unlock(&messages_received_on_client.mutex);
}

void add_message_to_outgoing_queuex(network_message message, network_client c) {
	send_queue_entry entry;
	entry.message = message;
	memset(entry.recipients, 0, sizeof(entry.recipients));
	entry.recipients[0] = c;
	add_message_to_outgoing_queue(entry);
}

void add_message_to_outgoing_queue(send_queue_entry entry) {
	network_message_type type = *(network_message_type*)(entry.message.data+NETWORK_PACKET_OVERHEAD);

	bool can_overwrite = type != MESSAGE_USER_SHOOT && type != MESSAGE_USER_MOVED && type != MESSAGE_USER_LOOK;

	mutex_lock(&messages_to_send_queue_mutex);
	for (int i = 0; i < OUTGOING_QUEUE_SIZE; i++)
	{
		if (messages_to_send_queue[i].active) {
			network_message_type type_existing = *(network_message_type*)(messages_to_send_queue[i].message.data+NETWORK_PACKET_OVERHEAD);
			if (type == type_existing && can_overwrite) {
				messages_to_send_queue[i] = entry;
				mutex_unlock(&messages_to_send_queue_mutex);
				return;
			}
			else {
				continue;
			}
		}
		messages_to_send_queue[i] = entry;
		messages_to_send_queue[i].active = true;
		mutex_unlock(&messages_to_send_queue_mutex);
		return;
	}
	mutex_unlock(&messages_to_send_queue_mutex);
	log_info("Outgoing network queue is full");
}

void* network_send_thread(void* args) {
	while (1) {
		mutex_lock(&messages_to_send_queue_mutex);
		for (int i = 0; i < OUTGOING_QUEUE_SIZE; i++)
		{
			if (!messages_to_send_queue[i].active) {
				continue;
			}
			send_queue_entry message = messages_to_send_queue[i];
			messages_to_send_queue[i].active = false;

			for (int x = 0; x < SERVER_MAX_PLAYERS; x++) {
				network_client c = message.recipients[x];
				if (c.ConnectSocket != 0) {
					network_client_send(&c, message.message);
				}
			}
			mem_free(message.message.data);	
		}
		mutex_unlock(&messages_to_send_queue_mutex);
		thread_sleep(1000);
	}	
}