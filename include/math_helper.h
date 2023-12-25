#ifndef INCLUDE_MATH_HELPER
#define INCLUDE_MATH_HELPER

#include <projectbase/project_base.h>

#include "players.h"
#include "objects.h"
#include "map.h"

#define MAP_RENDER_DEPTH renderer->set_render_depth(1);

#define DROP_RENDER_DEPTH(_h) renderer->set_render_depth(4 + ceil(_h));
#define BULLET_RENDER_DEPTH(_h) renderer->set_render_depth(5 + ceil(_h));
#define THROWABLE_RENDER_DEPTH(_h) renderer->set_render_depth(3 + ceil(_h));
#define OBJECT_RENDER_DEPTH(_h) renderer->set_render_depth(50 + ceil(_h));
#define OVERLAY_RENDER_DEPTH() renderer->set_render_depth(100);

bool onSegment(vec2f p, vec2f q, vec2f r);
int orientation(vec2f p, vec2f q, vec2f r);
bool lines_intersect(vec2f p1, vec2f q1, vec2f p2, vec2f q2);
vec2f get_intersection_point(vec2f A, vec2f B, vec2f C, vec2f D);
box get_render_box_of_square(platform_window* window, vec3f position, vec3f size);
box get_box_of_square(vec3f position, vec3f size);
vec3f get_center_of_square(vec3f position, vec3f size);
bool rect_contains_point(vec2 point, vec2 tl, vec2 br);

#endif