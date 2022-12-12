#ifndef INCLUDE_GAME
#define INCLUDE_GAME

#include <projectbase/project_base.h>

#include "../include/protocol.h"

typedef enum t_game_state {
	GAMESTATE_IDLE,
	GAMESTATE_LOADING_MAP,
	GAMESTATE_PLAYING,
} game_state;

typedef enum t_network_state {
	CONNECTING,
	WAITING_FOR_ID,
	CONNECTED,
	DISCONNECTED,
} network_state;

typedef struct t_game {
	game_state state;
	network_state network_state;
	network_server *server;
	network_client *client;
} game;

game global_state;

void init_game();

#endif