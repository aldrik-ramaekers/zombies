#ifndef INCLUDE_PLAYER
#define INCLUDE_PLAYER

#include "../project-base/src/project_base.h"

#include "map.h"
#include "objects.h"
#include "zombies.h"
#include "math_helper.h"
#include "guns.h"
#include "sprite.h"

#define MAX_PLAYERS 5
#define MAX_POINT_ANIMATIONS 10
#define PLAYER_HEAL_DELAY 2.0f
#define GUN_OFFSET_FROM_PLAYER 0.8f
#define PLAYER_HEALTH_REGEN_PER_SEC 200

typedef enum t_player_interact_state {
	INTERACT_IDLE,
	INTERACT_RELOADING,
	INTERACT_DEAD,
} player_interact_state;

typedef enum t_player_move_state
{
	PLAYER_MOVE_STATE_RUNNING,
	PLAYER_MOVE_STATE_IDLE,
	PLAYER_MOVE_STATE_JUMPING,
} player_move_state;

typedef enum t_player_direction {
	DIRECTION_UP,
	DIRECTION_RIGHT,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_TOPLEFT,
	DIRECTION_TOPRIGHT,
	DIRECTION_BOTTOMLEFT,
	DIRECTION_BOTTOMRIGHT,
} player_direction;

typedef struct t_point_animation
{
	bool active;
	u32 points;
	float diry;
	vec2f position;
	float sec_alive;
	float opacity;
} point_animation;

typedef struct t_player {
	u32 id;
	bool active;
	s32 health;
	s32 max_health;
	float sec_since_last_shot;
	float sec_since_last_damage_taken;
	player_interact_state interact_state;
	float sec_since_interact_state_change;
	float playerx;
	float playery;
	float gunx;
	float guny;
	float dirx;
	float diry;
	player_move_state move_state;
	float gun_height;
	float sec_since_last_step;
	player_direction direction;
	int total_ammo;
	int ammo_in_mag;
	float height;
	gun_type guntype;
	network_client client; // For the host: is_connected = false and socket = 0
	int kills;
	u64 ping;
	sprite sprite;
	sprite sprite_death;
	sprite sprite_idle;
	network_state connection_state;
	u32 points;
	struct {
		int grenades;
		int molotovs;
	} throwables;
	point_animation point_animations[MAX_POINT_ANIMATIONS];
} player;



#include "protocol.h"

u32 player_id = -1; // Session id on current server.

camera _next_camera_pos;

player players[MAX_PLAYERS] = {0};

int get_player_count();
player* get_player_by_id(u32 id);
void draw_players(platform_window* window, uint32_t ystart, uint32_t yend);
void draw_bullets(platform_window* window);
object check_if_player_collided_with_object(player p);
float get_player_size(platform_window* window);
void move_user(platform_window* window, u32 id, protocol_move_type move, float delta);
void update_players_server();
void hurt_player(u32 id, u32 damage);
void spawn_player(u32 id, network_client client);
bool player_has_old_session(u32 id);
void rejoin_player(u32 id, network_client client);
char* get_player_name_by_player_index(int index);
color get_color_tint_by_player_index(int index);
void add_points_to_player(player* p, u32 points);
void update_points_animation_server();
void clear_players();

#endif