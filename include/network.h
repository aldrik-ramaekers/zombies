#ifndef INCLUDE_NETWORK
#define INCLUDE_NETWORK

typedef enum t_network_state {
	CONNECTING,
	WAITING_FOR_ID,
	CONNECTED,
	DISCONNECTED,
} network_state;

#endif