#ifndef INCLUDE_OBJECT
#define INCLUDE_OBJECT

#include <projectbase/project_base.h>

#include "map.h"

typedef struct t_vec3f {
	float x,y,z;
} vec3f;

typedef struct t_object {
	bool active;
	vec2f position;
	vec3f size;
	float h;
} object;

typedef struct t_box {
	vec2f tl_b;
	vec2f tr_b;
	vec2f bl_b;
	vec2f br_b;

	vec2f tl_u;
	vec2f tr_u;
	vec2f bl_u;
	vec2f br_u;
} box;

object objects[50];
int max_objects = 50;

void create_objects();
void draw_objects_at_row(platform_window* window, int row);
box get_box_of_object(platform_window* window, object o);

#endif