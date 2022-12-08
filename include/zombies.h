#ifndef INCLUDE_ZOMBIES
#define INCLUDE_ZOMBIES

#include <projectbase/project_base.h>

#include "players.h"
#include "objects.h"

typedef struct t_zombie {
	bool alive;
	float health;
	vec3f position;
	vec3f size;
} zombie;

typedef struct t_spawner {
	vec2 position;
	float sec_since_last_spawn;
} spawner;

// data data that is stored on disk
spawner spawner_tiles[2] = {
	{9, 0, 0},
	{1, 8, 0},
};


zombie zombies[50] = {0};
int max_zombies = 50;

void draw_spawners(platform_window* window);
void draw_zombies_at_tile(platform_window* window, int x, int y);
void spawn_zombie(int x, int y);

#endif