#ifndef INCLUDE_PLAYER
#define INCLUDE_PLAYER

#include <projectbase/project_base.h>

#include "map.h"
#include "objects.h"

typedef struct t_bullet {
	bool active;
	float x;
	float y;
	float h;
	float xacceleration;
	float yacceleration;
} bullet;

bullet bullets[500] = {0};
int max_bullets = 500;

float playerx = 3;
float playery = 3;

void shoot(platform_window* window);
void draw_player(platform_window* window);
void draw_bullets(platform_window* window);
float get_player_size(platform_window* window);

#endif