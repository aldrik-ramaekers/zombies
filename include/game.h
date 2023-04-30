#ifndef INCLUDE_GAME
#define INCLUDE_GAME

#include <projectbase/project_base.h>

#include "../include/protocol.h"
#include "../include/players.h"
#include "../include/game.h"

#define SERVER_TICK_RATE (1.0f/60.0f)
#define SERVER_MAX_PLAYERS (10)
#define SERVER_PATHFINDING_INTERVAL (0.25f)

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

game global_state = {GAMESTATE_IDLE,DISCONNECTED,0,0};

void init_game();

#endif