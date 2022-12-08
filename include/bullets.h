#ifndef INCLUDE_BULLETS
#define INCLUDE_BULLETS

#include <projectbase/project_base.h>

#include "players.h"
#include "objects.h"
#include "map.h"

typedef struct t_bullet {
	bool active;
	vec3f position;
	float endx;
	float endy;
	float alive_time;
} bullet;

bullet bullets[500] = {0};
int max_bullets = 500;

void draw_bullets(platform_window* window);

#endif