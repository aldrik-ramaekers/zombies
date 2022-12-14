#ifndef INCLUDE_PLAYER
#define INCLUDE_PLAYER

#include <projectbase/project_base.h>

#include "map.h"
#include "objects.h"
#include "zombies.h"
#include "math_helper.h"
#include "guns.h"

typedef struct t_player {
	int id;
	bool active;
	float sec_since_last_shot;
	float playerx;
	float playery;
	float gunx;
	float guny;
	float gun_height;
	int total_ammo;
	int ammo_in_mag;
	gun_type guntype;
} player;

#include "protocol.h"

u32 my_id = -1;

camera _next_camera_pos;

int max_players = 10;
player players[10] = {0};

player* get_player_by_id(u32 id);
void draw_players_at_tile(platform_window* window, int x, int y);
void draw_bullets(platform_window* window);
object check_if_player_collided_with_object(platform_window* window, player p);
float get_player_size(platform_window* window);
void move_user(platform_window* window, u32 id, protocol_move_type move);
void update_players_server();

#endif