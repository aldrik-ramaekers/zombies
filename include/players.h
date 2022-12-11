#ifndef INCLUDE_PLAYER
#define INCLUDE_PLAYER

#include <projectbase/project_base.h>

#include "map.h"
#include "objects.h"
#include "zombies.h"
#include "math_helper.h"

typedef struct t_player {
	int id;
	bool active;
	float sec_since_last_shot;
	float playerx;
	float playery;
	float gunx;
	float guny;
	float gun_height;
} player;

int my_id = 1;

int max_players = 10;
player players[10] = {0};

player get_player_by_id(int id);
void draw_players_at_tile(platform_window* window, int x, int y);
void draw_bullets(platform_window* window);
float get_player_size(platform_window* window);

#endif