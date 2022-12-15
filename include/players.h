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
	float height;
	gun_type guntype;
	network_client client; // For the host: is_connected = false and socket = 0
	int kills;
	u64 ping;
} player;

#include "protocol.h"

u32 my_id = -1;

camera _next_camera_pos;

int max_players = 10;
player players[10] = {0};

int get_player_count();
player* get_player_by_id(u32 id);
void draw_players(platform_window* window);
void draw_bullets(platform_window* window);
object check_if_player_collided_with_object(platform_window* window, player p);
float get_player_size(platform_window* window);
void move_user(platform_window* window, u32 id, protocol_move_type move);
void update_players_server();
void spawn_player(int id, network_client client);

#endif