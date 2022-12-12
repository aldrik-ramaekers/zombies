#include "../include/protocol.h"

network_message create_protocol_get_id_up() {
	protocol_get_id_upstream* buf = (protocol_get_id_upstream*)network_buffer;
	buf->type = MESSAGE_GET_ID_UPSTREAM;
	return network_create_message(network_buffer, sizeof(protocol_get_id_upstream), MAX_NETWORK_BUFFER_SIZE);
}

network_message create_protocol_get_id_down(u32 id) {
	protocol_get_id_downstream* buf = (protocol_get_id_downstream*)network_buffer;
	buf->type = MESSAGE_GET_ID_DOWNSTREAM;
	buf->id = id;
	return network_create_message(network_buffer, sizeof(protocol_get_id_downstream), MAX_NETWORK_BUFFER_SIZE);
}

void server_on_message_received(u8* buffer, u32 length, network_client client) {
	u8* allocated_buf = mem_alloc(length + sizeof(network_client));
	memcpy(allocated_buf, &client, sizeof(network_client));
	memcpy(allocated_buf + sizeof(network_client), buffer + 4, length-4);
	array_push(&messages_received_on_server, (u8*)&allocated_buf);
}

void client_on_message_received(u8* buffer, u32 length) {
	u8* allocated_buf = mem_alloc(length);
	memcpy(allocated_buf, buffer+4, length-4);
	array_push(&messages_received_on_client, (u8*)&allocated_buf);
}
