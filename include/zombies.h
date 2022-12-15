#ifndef INCLUDE_ZOMBIES
#define INCLUDE_ZOMBIES

#include <projectbase/project_base.h>

#include "players.h"
#include "objects.h"
#include "pathfinding.h"

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
} zombie;

typedef struct t_spawner {
	vec2 position;
	float sec_since_last_spawn;
} spawner;

// data data that is stored on disk
spawner spawner_tiles[2] = {
	{15, 5, 999},
	{3, 8, 999},
};

zombie zombies[20] = {0};
int max_zombies = 20;

void draw_spawners(platform_window* window);
void draw_zombies(platform_window* window);
void spawn_zombie(int x, int y);

#endif