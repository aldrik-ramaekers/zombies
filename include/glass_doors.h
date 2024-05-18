#ifndef INCLUDE_GLASS_DOOR
#define INCLUDE_GLASS_DOOR

#include "objects.h"

typedef struct t_glass_door
{
	object obj;
	bool is_open;
	float time_since_open;
} glass_door;

#define MAX_GLASS_DOORS 100
glass_door* glass_doors;

void create_glass_door(object o);
void draw_glass_doors(platform_window* window, uint32_t ystart, uint32_t yend);
void update_glass_doors_server();

#endif