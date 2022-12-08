#ifndef INCLUDE_MAP
#define INCLUDE_MAP

#include <projectbase/project_base.h>

#include "players.h"
#include "objects.h"

typedef struct t_tile {
	int height;

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

#define MAP_SIZE_X 20
#define MAP_SIZE_Y 20

tile map_loaded[MAP_SIZE_Y][MAP_SIZE_X];

typedef struct t_map_info {
	int tile_width;
	int tile_height;
	int px_raised_per_h;
	float px_incline;
} map_info;

// data data that is stored on disk
int map[MAP_SIZE_Y][MAP_SIZE_X] = {
	{0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,1,1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

void load_map_from_data();
tile get_tile_under_coords(platform_window* window, float x, float y);
float get_height_of_tile_under_coords(platform_window* window, float tocheckx, float tochecky);
int get_tile_height(platform_window* window);
int get_tile_width(platform_window* window);
bool is_in_bounds(platform_window* window, float x, float y);
void draw_grid(platform_window* window);
map_info get_map_info(platform_window* window);

#endif