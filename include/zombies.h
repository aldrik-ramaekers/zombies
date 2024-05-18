#ifndef INCLUDE_ZOMBIES
#define INCLUDE_ZOMBIES

#include <projectbase/project_base.h>

#include "players.h"
#include "objects.h"
#include "pathfinding.h"
#include "sprite.h"

typedef enum t_zombie_type {
	ZOMBIE_TYPE_NONE,
	ZOMBIE_TYPE_NORMAL,
	ZOMBIE_TYPE_ENRAGED,
} zombie_type;

typedef struct t_zombie {
	bool alive;
	s32 health;
	s32 max_health;
	vec3f position;
	vec3f size;
	array path;
	float speed;
	float attack_range;
	float attack_rate;
	u32 attack_damage;
	float sec_since_last_step;
	float sec_since_last_attack;
	array next_path;
	float time_since_last_path;
	float time_since_last_successfull_req;
	pathfinding_request request;
	vec2f next2tiles[2];
	zombie_type type;
	sprite sprite_run;
	vec2f dir;
} zombie;

typedef struct t_spawner {
	bool active;
	vec2 position;
	float sec_since_last_spawn;
	sprite sprite;
} spawner;

// Despawn distance.
#define MAX_DISTANCE_BETWEEN_ZOMBIE_AND_PLAYER 40

// Spawners closer than this will not spawn.
#define MIN_DISTANCE_BETWEEN_ZOMBIE_AND_PLAYER 20

#define MAX_SPAWNERS (25)
spawner* spawner_tiles;

#define SERVER_MAX_ZOMBIES (50)
zombie* zombies;

bool hit_zombie(int index, int damage);
void create_spawner(vec2 position);
void draw_spawners(platform_window* window);
void draw_zombies(platform_window* window, uint32_t ystart, uint32_t yend);
void spawn_zombie(int x, int y);
void clear_zombies();
u32 number_of_zombies_active();

#endif