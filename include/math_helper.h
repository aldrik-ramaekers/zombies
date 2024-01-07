#ifndef INCLUDE_MATH_HELPER
#define INCLUDE_MATH_HELPER

#include <projectbase/project_base.h>

#include "players.h"
#include "objects.h"
#include "map.h"

#define RENDER_DEPTH_MAP 0
#define RENDER_DEPTH_BEHIND_PLAYER 1
#define RENDER_DEPTH_PLAYER 2
#define RENDER_DEPTH_INFRONT_PLAYER 3

bool onSegment(vec2f p, vec2f q, vec2f r);
int orientation(vec2f p, vec2f q, vec2f r);
bool lines_intersect(vec2f p1, vec2f q1, vec2f p2, vec2f q2);
vec2f get_intersection_point(vec2f A, vec2f B, vec2f C, vec2f D);
box get_render_box_of_square(platform_window* window, vec3f position, vec3f size);
box get_box_of_square(vec3f position, vec3f size);
vec3f get_center_of_square(vec3f position, vec3f size);
bool rect_contains_point(vec2 point, vec2 tl, vec2 br);
bool vec3f_equals(vec3f p1, vec3f p2);

#endif