#ifndef INCLUDE_THROWABLES
#define INCLUDE_THROWABLES

#include "../project-base/src/project_base.h"

#include "players.h"
#include "objects.h"
#include "map.h"
#include "guns.h"

typedef enum t_throwable_type
{
	THROWABLE_NONE = 0,
	THROWABLE_GRENADE,
	THROWABLE_MOLOTOV,
} throwable_type;

typedef enum t_throwable_state
{
	THROWABLE_FLYING,
	THROWABLE_EXPLODED,
} throwable_state;

typedef struct t_throwable {
	u32 player_id;
	bool active;
	throwable_state state;
	throwable_type type;
	vec3f position;
	vec3f direction;
	float alive_time;
	int bounces;
	sprite sprite;
	int damage;
	float rotation;
	float sec_since_last_tick;
} throwable;

throwable throwables[50] = {0};
int max_throwables = 50;

void throw_throwable(u32 id, throwable_type type, float dirx, float diry);
void draw_throwables(platform_window* window);

#endif