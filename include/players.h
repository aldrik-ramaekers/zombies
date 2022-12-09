#ifndef INCLUDE_PLAYER
#define INCLUDE_PLAYER

#include <projectbase/project_base.h>

#include "map.h"
#include "objects.h"
#include "zombies.h"
#include "math_helper.h"

float sec_since_last_shot = 10.0f;
float playerx = 3;
float playery = 3;
float gunx = 0.0f;
float guny = 0.0f;
float gun_height = 0.0f;

void shoot(platform_window* window);
void draw_player(platform_window* window);
void draw_bullets(platform_window* window);
float get_player_size(platform_window* window);

#endif