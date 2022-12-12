#ifndef INCLUDE_PROTOCOL
#define INCLUDE_PROTOCOL

#include <projectbase/project_base.h>

typedef enum t_network_message_type {
	MESSAGE_GET_ID_UPSTREAM,
	MESSAGE_GET_ID_DOWNSTREAM,
} network_message_type;

typedef struct t_protocol_generic_message {
	network_client client;
	network_message_type type;
} protocol_generic_message;

typedef struct t_protocol_generic_client_message {
	network_message_type type;
} protocol_generic_client_message;

typedef struct t_protocol_get_id_upstream {
	network_message_type type;
} protocol_get_id_upstream;

typedef struct t_protocol_get_id_downstream {
	network_message_type type;
	u32 id;
} protocol_get_id_downstream;

#define MAX_NETWORK_BUFFER_SIZE 50000
u8 network_buffer[50000];
network_message create_protocol_get_id_up();
network_message create_protocol_get_id_down(u32 id);

array messages_received_on_server;
array messages_received_on_client;

void server_on_message_received(u8* buffer, u32 length, network_client client);
void client_on_message_received(u8* buffer, u32 length);

#endif