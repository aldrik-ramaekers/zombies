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
	vec2f tl_d;
	vec2f tr_d;
	vec2f bl_d;
	vec2f br_d;

	vec2f tl_u;
	vec2f tr_u;
	vec2f bl_u;
	vec2f br_u;
} box;

int max_objects = 150;
object objects[150];

object get_object_at_tile(float x, float y);
void create_objects();
void draw_objects_at_row(platform_window* window, int row);
box get_box_of_object(platform_window* window, object o);
void render_quad_with_outline(vec2f tl, vec2f tr, vec2f bl, vec2f br, color c);

#endif