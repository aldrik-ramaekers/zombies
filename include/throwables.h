#ifndef INCLUDE_THROWABLES
#define INCLUDE_THROWABLES

#include <projectbase/project_base.h>

#include "players.h"
#include "objects.h"
#include "map.h"
#include "guns.h"

typedef enum t_throwable_type
{
	THROWABLE_GRENADE,
} throwable_type;

typedef struct t_throwable {
	u32 player_id;
	bool active;
	throwable_type type;
	vec3f position;
	vec3f direction;
	float alive_time;
} throwable;

throwable throwables[500] = {0};
int max_throwables = 500;

void clear_throwables();
void throw_throwable(platform_window* window, u32 id, throwable_type type, float dirx, float diry);
void draw_throwables(platform_window* window);

#endif