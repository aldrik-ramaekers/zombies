#ifndef INCLUDE_WALL_ITEM
#define INCLUDE_WALL_ITEM

#include <projectbase/project_base.h>

#include "objects.h"
#include "guns.h"

typedef enum t_wall_item_type {
	WALLITEM_GUN,
} wall_item_type;

typedef union t_wall_item_data {
	gun_type gun;
} wall_item_data;

typedef struct t_wall_item {
	bool active;
	vec3f position;
	float time_active;
	float start_h;
	wall_item_type item;
	wall_item_data data;
	image* img;
} wall_item;

#define MAX_WALLITEMS (200)
wall_item wallitems[MAX_WALLITEMS] = {0};

void update_wallitems_server();
void draw_wallitems(platform_window* window);
void create_wallitem(vec3f position, wall_item_type item, wall_item_data data);

#endif