#ifndef INCLUDE_OBJECT
#define INCLUDE_OBJECT

#include "../project-base/src/project_base.h"

#include "map.h"

#define MAX_OBJECTS 10000
#define MAX_DECORATION_OBJECTS 100

typedef struct t_vec3f {
	float x,y,z;
} vec3f;

typedef struct t_light_emitter {
	bool active;
	vec3f position;
	float brightness;
	float range;
} light_emitter;

// @NEWOBJECT
typedef enum t_object_type {
	OBJECT_NONE,
	OBJECT_SPACE_CONTROL_PANEL = 1,
	OBJECT_SPACE_WINDOW = 2,
	OBJECT_METAL_WALL = 3,
	OBJECT_METAL_WALL2 = 4,
	OBJECT_SPACE_CONTROL_PANEL2 = 5,
	OBJECT_CHAIR_UP = 6,
	OBJECT_SPACE_WINDOW_H = 7,
	OBJECT_ZOMBIE_SPAWNER = 8, // Substitute.
	OBJECT_LAMP_EAST = 9,
	OBJECT_METAL_TABLE_H = 10,
	OBJECT_CANTINE_GLASS = 11,
	OBJECT_GLASS_DOOR_H = 12, // Substitute.
	OBJECT_LAMP_SOUTH = 13,
	OBJECT_BIG_CONTROL_PANEL = 14,
	OBJECT_CHAIR_DOWN = 15,
	OBJECT_COMPUTER_RACK = 16,
	OBJECT_GENERATOR = 17,
	OBJECT_SCHOOL_TABLE = 18,
	OBJECT_SCHOOL_BENCH = 19,
	OBJECT_LOCKER = 20,
	OBJECT_BAR_H = 21,
	OBJECT_BAR_V = 22,
	OBJECT_BAR_HV = 23,
	OBJECT_CLUB_SEAT1 = 24,
	OBJECT_CLUB_SEAT2 = 25,
	OBJECT_CLUB_SEAT3 = 26,
	OBJECT_CLUB_SEAT4 = 27,
	OBJECT_CLUB_WALL1 = 28,
	OBJECT_CLUB_WALL2 = 29,
	OBJECT_CLUB_WALL3 = 30,
	OBJECT_CLUB_SEAT5 = 31,
	OBJECT_CLUB_SEAT6 = 32,
	OBJECT_CLUB_SEAT7 = 33,
	OBJECT_BOWLING_LANE = 34,
	OBJECT_BOWLING_LANE_END = 35,
	OBJECT_GLASS_DOOR_V = 36, // Substitute.
	OBJECT_BOWLING_MACHINE = 37,
	OBJECT_PAINTING1 = 38,
	OBJECT_PAINTING2 = 39,
	OBJECT_PAINTING3 = 40,
	OBJECT_PAINTING4 = 41,
	OBJECT_PAINTING5 = 42,
	OBJECT_PAINTING6 = 43,
	OBJECT_PAINTING7 = 44,
	OBJECT_PAINTING8 = 45,
	OBJECT_PAINTING9 = 46,
	OBJECT_PAINTING10 = 47,
	OBJECT_FLOORMAT1 = 48,
	OBJECT_FLOORMAT2 = 49,
	OBJECT_METAL_WALL3 = 50,
	OBJECT_METAL_WALL4 = 51,
	OBJECT_METAL_WALL5 = 52,
	OBJECT_METAL_WALL6 = 53,
	OBJECT_METAL_WALL7 = 54,
	OBJECT_METAL_WALL8 = 55,
	OBJECT_METAL_WALL9 = 56,
	OBJECT_METAL_WALL10 = 57,

	OBJECT_END,
} object_type;

typedef struct t_object {
	bool active;
	vec3f position;
	vec3f size;
	object_type type;
	bool collision;
} object;

/*
typedef struct t_object2 {
	bool active;
	vec3f position;
	vec3f size;
	object_type type;
	bool collision;
} object2;*/

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

object object_dict[OBJECT_END];

// decoration objects laying on floor.
object decoration_objects[MAX_DECORATION_OBJECTS] = {0};

void add_decoration_object(object o);
object get_object_at_tile(float x, float y);
object* get_pobject_at_tile(float x, float y);
object* get_object_at_tile_from_mapfile(float x, float y);
void create_objects();
void add_object(object obj);
void draw_objects(platform_window* window);
box get_box_of_object(platform_window* window, object o);
void render_fill_quad_with_outline(vec2f tl, vec2f tr, vec2f bl, vec2f br, color c);
void render_fill_box_with_outline(box box, color c);
void render_box_outline(box box, color c);

#endif