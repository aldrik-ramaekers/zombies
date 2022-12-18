#ifndef INCLUDE_DROPS
#define INCLUDE_DROPS

#include <projectbase/project_base.h>

#include "guns.h"

typedef enum t_drop_type {
	DROP_GUN,
	DROP_AMMO,
} drop_type;

typedef struct t_drop {
	bool active;
	float time_active;
	vec3f position;
	vec3f size;
	float start_h;
	drop_type type;
	union {
		int ammo_count;
		gun_type gun;
	} data;
} drop;

#define MAX_DROPS 50
drop drops[MAX_DROPS] = {0};
#define DROP_MAX_DURATION (4.0f)
#define DROP_FADE_TIME (2.0f)

void update_drops_server();
void draw_drops(platform_window* window);
void spawn_drop(vec3f pos);

#endif