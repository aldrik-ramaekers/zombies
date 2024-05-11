#ifndef INCLUDE_MAP
#define INCLUDE_MAP

#include <projectbase/project_base.h>

#include "players.h"
#include "objects.h"

typedef enum t_tile_type {
	TILE_NONE = 0,
	TILE_FLOOR1 = 1,
	TILE_FLOOR2 = 2,

	TILE_END,
} tile_type;

typedef struct t_tile {
	int height;
	tile_type type;

	// filled in on load.
	int topleft;
	int topright;
	int bottomleft;
	int bottomright;

	// filled after each frame.
	vec2f tl;
	vec2f tr;
	vec2f bl;
	vec2f br;
} tile;

#define MAP_SIZE_X 1000
#define MAP_SIZE_Y 1000
#define MAX_LIGHT_EMITTERS 1000

typedef struct t_map_info {
	int tile_width;
	int tile_height;
	int px_raised_per_h;
	float px_incline;
} map_info;

typedef struct t_map_data { // Data written to disk.
	int width;
	int height;
	int heightmap[MAP_SIZE_Y][MAP_SIZE_X];
	tile_type tiles[MAP_SIZE_Y][MAP_SIZE_X];
	object objects[MAX_OBJECTS];
	light_emitter light_emitters[MAX_LIGHT_EMITTERS];
} map_data;

typedef struct t_light_data {
	float tl;
	float tr;
	float bl;
	float br;
} light_data;

typedef struct t_extracted_map_data { // Data extracted from file on disk.
	int width;
	int height;
	tile heightmap[MAP_SIZE_Y][MAP_SIZE_X]; // tilemap
	object objects[MAX_OBJECTS];
	light_data lightmap[MAP_SIZE_Y][MAP_SIZE_X];
	light_emitter light_emitters[MAX_LIGHT_EMITTERS];
} extracted_map_data;

extern int player_zoom;
map_data map_to_load = {0};
extracted_map_data loaded_map = {0};

vec2 screen_pos_to_world_pos(platform_window* window, float x, float y);
vec2f world_pos_to_screen_pos(platform_window* window, float x, float y, float z);
void create_empty_map();
void load_map_from_file();
tile get_tile_under_coords(float x, float y);
float get_height_of_tile_under_coords(float tocheckx, float tochecky);
int get_tile_height(platform_window* window);
int get_tile_width(platform_window* window);
bool is_in_bounds(float x, float y);
void draw_grid(platform_window* window);
map_info get_map_info(platform_window* window);
image* get_image_from_tiletype(tile_type tile);

#endif