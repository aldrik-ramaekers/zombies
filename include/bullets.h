#ifndef INCLUDE_BULLETS
#define INCLUDE_BULLETS

#include <projectbase/project_base.h>

#include "players.h"
#include "objects.h"
#include "map.h"
#include "guns.h"

typedef struct t_bullet {
	int player_id;
	bool active;
	vec3f position;
	float endx;
	float endy;
	float alive_time;
} bullet;

bullet bullets[500] = {0};
int max_bullets = 500;

void shoot(platform_window* window, u32 id, float dirx, float diry);
void draw_bullets(platform_window* window);

#endif