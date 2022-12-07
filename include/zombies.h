#ifndef INCLUDE_ZOMBIES
#define INCLUDE_ZOMBIES

#include <projectbase/project_base.h>

#include "players.h"
#include "objects.h"

typedef struct t_zombie {
	bool alive;
	float health;
	vec2f position;
} zombie;


zombie zombies[50] = {0};
int max_zombies = 50;

void draw_spawners(platform_window* window);
void draw_zombies_at_tile(platform_window* window, int x, int y);
void spawn_zombie(int x, int y);

#endif