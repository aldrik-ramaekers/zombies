#ifndef INCLUDE_GAME
#define INCLUDE_GAME

#include <projectbase/project_base.h>

#include "../include/protocol.h"
#include "../include/players.h"
#include "../include/game.h"

#define SERVER_TICK_RATE (1.0f/60.0f)
#define SERVER_MAX_PLAYERS (5)
#define SERVER_CLOSE_PATHFINDING_INTERVAL (0.25f)
#define SERVER_FAR_PATHFINDING_INTERVAL (1.0f)
#define SERVER_PATHFINDING_THREADS 3

typedef enum t_game_state {
	GAMESTATE_IDLE,
	GAMESTATE_LOADING_MAP,
	GAMESTATE_LOADING_ASSETS,
	GAMESTATE_PLAYING,
} game_state;

typedef enum t_scene_state
{
	SCENE_MAIN_MENU,
	SCENE_GAME,
	SCENE_CREDITS,
	SCENE_SETTINGS,
	SCENE_LOBBY,
} scene_state;

typedef struct t_game {
	game_state state;
	network_state network_state;
	network_server *server;
	network_client *client;
} game;

scene_state global_scene_state = SCENE_MAIN_MENU;
game global_state = {GAMESTATE_IDLE,DISCONNECTED,0,0};

pathfinding_request active_requests[SERVER_PATHFINDING_THREADS] = {0};

void init_game();
void start_solo_game();
bool connect_to_game(char* ip, char* port);
void fill_game_structs();

#endif