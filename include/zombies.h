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
} zombie_type;

typedef struct t_zombie {
	bool alive;
	float health;
	vec3f position;
	vec3f size;
	array path;
	array next_path;
	float time_since_last_path;
	pathfinding_request request;
	vec2f next2tiles[2];
	zombie_type type;
} zombie;

typedef struct t_spawner {
	bool active;
	vec2 position;
	float sec_since_last_spawn;
	sprite sprite;
} spawner;

#define MAX_SPAWNERS (5)
spawner spawner_tiles[MAX_SPAWNERS] = {0};

#define SERVER_MAX_ZOMBIES (50)
zombie zombies[SERVER_MAX_ZOMBIES] = {0};

bool hit_zombie(int index, int damage);
void create_spawner(vec2 position);
void draw_spawners(platform_window* window);
void draw_zombies(platform_window* window);
void spawn_zombie(int x, int y);
u32 number_of_zombies_active();

#endif