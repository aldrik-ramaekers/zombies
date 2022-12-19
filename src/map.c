#include "../include/map.h"

static int get_height_of_tile_tl(int current_height, int x, int y) {
	int highest_point = current_height;
	if (y > 0) {
		int tile_above = map[y-1][x];
		if (tile_above > highest_point) {
			highest_point = tile_above;
		}
	}
	if (y > 0 && x > 0) {
		int tile_above = map[y-1][x-1];
		if (tile_above > highest_point) {
			highest_point = tile_above;
		}
	}
	if (x > 0) {
		int tile_above = map[y][x-1];
		if (tile_above > highest_point) {
			highest_point = tile_above;
		}
	}
	return highest_point;
}


static int get_height_of_tile_br(int current_height, int x, int y) {
	int highest_point = current_height;
	if (x < MAP_SIZE_X-1) {
		int tile_right = map[y][x+1];
		if (tile_right > highest_point) {
			highest_point = tile_right;
		}
	}
	if (y < MAP_SIZE_Y-1 && x < MAP_SIZE_X-1) {
		int tile_bottom_right = map[y+1][x+1];
		if (tile_bottom_right > highest_point) {
			highest_point = tile_bottom_right;
		}
	}
	if (y < MAP_SIZE_Y-1) {
		int tile_bottom = map[y+1][x];
		if (tile_bottom > highest_point) {
			highest_point = tile_bottom;
		}
	}
	return highest_point;
}

static int get_height_of_tile_bl(int current_height, int x, int y) {
	int highest_point = current_height;
	if (y > 0 && x > 0) {
		int tile_left = map[y][x-1];
		if (tile_left > highest_point) {
			highest_point = tile_left;
		}
	}
	if (y < MAP_SIZE_Y-1 && x > 0) {
		int tile_bottom_left = map[y+1][x-1];
		if (tile_bottom_left > highest_point) {
			highest_point = tile_bottom_left;
		}
	}
	if (y < MAP_SIZE_Y-1) {
		int tile_bottom = map[y+1][x];
		if (tile_bottom > highest_point) {
			highest_point = tile_bottom;
		}
	}
	return highest_point;
}

static int get_height_of_tile_tr(int current_height, int x, int y) {
	int highest_point = current_height;
	if (y > 0) {
		int tile_above = map[y-1][x];
		if (tile_above > highest_point) {
			highest_point = tile_above;
		}
	}
	if (y > 0 && x < MAP_SIZE_X-1) {
		int tile_above_right = map[y-1][x+1];
		if (tile_above_right > highest_point) {
			highest_point = tile_above_right;
		}
	}
	if (x < MAP_SIZE_X-1) {
		int tile_right = map[y][x+1];
		if (tile_right > highest_point) {
			highest_point = tile_right;
		}
	}
	return highest_point;
}

// load hardcoded map.
void load_map_from_data() {
	for (int y = 0; y < MAP_SIZE_Y; y++) {
		for (int x = MAP_SIZE_X-1; x >= 0; x--) {
			int h = map[y][x];
			int highest_point_topleft = get_height_of_tile_tl(h, x, y);
			int highest_point_topright = get_height_of_tile_tr(h, x, y);
			int highest_point_bottomright = get_height_of_tile_br(h, x, y);
			int highest_point_bottomleft = get_height_of_tile_bl(h, x, y);
			map_loaded[y][x] = (tile){h, highest_point_topleft, highest_point_topright, highest_point_bottomleft, highest_point_bottomright};
		}
	}
}

tile get_tile_under_coords(float x, float y) {
	return map_loaded[(int)(y)][(int)(x)];
}

float get_height_of_tile_under_coords(float tocheckx, float tochecky) {
	tile tile_under_coords = get_tile_under_coords(tocheckx, tochecky);

	int ty = (int)(tochecky);
	int tx = (int)(tocheckx);

	float percentage_x = tocheckx - tx;
	float percentage_y = tochecky - ty;

	float topline_h = min(tile_under_coords.topright, tile_under_coords.topleft);
	float topline_diff = (tile_under_coords.topright - tile_under_coords.topleft) * percentage_x;
	if (topline_diff < 0.0f) topline_diff = abs(tile_under_coords.topright - tile_under_coords.topleft) + topline_diff;
	topline_h += topline_diff;

	float bottomline_h = min(tile_under_coords.bottomright, tile_under_coords.bottomleft);
	float bottomline_diff = (tile_under_coords.bottomright - tile_under_coords.bottomleft) * percentage_x;
	if (bottomline_diff < 0.0f) bottomline_diff = abs(tile_under_coords.bottomright - tile_under_coords.bottomleft) + bottomline_diff;
	bottomline_h += bottomline_diff;

	float playerline = min(topline_h, bottomline_h);
	float playerline_diff = (bottomline_h - topline_h) * percentage_y;
	if (bottomline_h < topline_h) {
		playerline_diff = (topline_h - bottomline_h) * (1 -percentage_y);
	}
	playerline += playerline_diff;

	return playerline;
}

inline int get_tile_width(platform_window* window) {
	int tile_width = window->height / 30;
	if (window->width > window->height) tile_width = window->width / 30;
	return tile_width;
}

inline int get_tile_height(platform_window* window) {
	int tile_width = get_tile_width(window);
	int tile_height = tile_width * 1;
	return tile_height;
}

bool is_in_bounds(float x, float y) {
	return (x >= 0 && x <= MAP_SIZE_X && y >= 0 && y < MAP_SIZE_Y);
}

static float offset = 0.0f;
static bool dir = true;
void draw_grid(platform_window* window) {
	/*if (dir) offset += 0.005f;
	else offset -= 0.005f;
	if (offset >= 0.5f) dir = false;
	if (offset <= -0.5f) dir = true;*/

	map_info info = get_map_info(window);

	for (int y = 0; y < MAP_SIZE_Y; y++) {
		MAP_RENDER_DEPTH;
		for (int x = MAP_SIZE_X-1; x >= 0; x--) {
			tile tile = map_loaded[y][x];

			float xdiff_between_bottom_and_top = info.px_incline;
			float render_x =  (info.tile_width * x) + (xdiff_between_bottom_and_top * y);
			float render_y = info.tile_height * y;
			render_y -= tile.height*info.px_raised_per_h;

			vec2f topleft;
			vec2f bottomleft;
			vec2f bottomright;
			vec2f topright;

			int highest_point_topleft = tile.topleft;
			topleft = (vec2f){render_x, info.tile_width * y - highest_point_topleft*info.px_raised_per_h};
			
			int highest_point_topright = tile.topright;
			topright = (vec2f){render_x + info.tile_width, info.tile_height * y - highest_point_topright*info.px_raised_per_h};

			int highest_point_bottomright = tile.bottomright;
			bottomright = (vec2f){render_x + xdiff_between_bottom_and_top+info.tile_width, info.tile_height * y + info.tile_height - highest_point_bottomright*info.px_raised_per_h};

			int highest_point_bottomleft = tile.bottomleft;
			bottomleft = (vec2f){render_x + xdiff_between_bottom_and_top, info.tile_height * y + info.tile_height - highest_point_bottomleft*info.px_raised_per_h};

			color c = rgb(128, 64, 0);
			if (highest_point_topleft > highest_point_bottomleft || highest_point_topright > highest_point_bottomright ||
				highest_point_topleft > highest_point_bottomright || highest_point_topright > highest_point_bottomleft ||
				highest_point_topright > highest_point_topleft || highest_point_bottomright > highest_point_bottomleft) c = rgb(108, 64, 0);
			if (highest_point_topleft < highest_point_bottomleft || highest_point_topright < highest_point_bottomright ||
				highest_point_topleft > highest_point_topright) c = rgb(148, 64, 0);

			renderer->render_quad(
				topleft.x, topleft.y, 
				bottomleft.x, bottomleft.y, 
				bottomright.x, bottomright.y, 
				topright.x, topright.y, 
				c);

			if (highest_point_topleft != highest_point_bottomright && highest_point_bottomleft == highest_point_topright) {
				if (highest_point_bottomleft < highest_point_topleft || highest_point_bottomleft < highest_point_bottomright) {
					renderer->render_tri(topleft.x, topleft.y, 
						bottomleft.x, bottomleft.y, 
						bottomright.x, bottomright.y, rgb(108, 64, 0));
				}
				renderer->render_line(topleft.x, topleft.y, bottomright.x, bottomright.y, 1, rgb(0,0,255)); // diag
			}

			if (highest_point_bottomleft != highest_point_topright && highest_point_topleft == highest_point_bottomright) {
				renderer->render_line(topright.x, topright.y, bottomleft.x, bottomleft.y, 1, rgb(0,0,255)); // diag
			}
			
			renderer->render_line(topleft.x, topleft.y, topright.x, topright.y, 1, rgb(0,0,255)); // top
			renderer->render_line(topleft.x, topleft.y, bottomleft.x, bottomleft.y, 1, rgb(0,0,255)); // left
			renderer->render_line(topright.x, topright.y, bottomright.x, bottomright.y, 1, rgb(0,0,255)); // right
			renderer->render_line(bottomleft.x, bottomleft.y, bottomright.x, bottomright.y, 1, rgb(0,0,255)); // bottom

			map_loaded[y][x].tl = topleft;
			map_loaded[y][x].tr = topright;
			map_loaded[y][x].bl = bottomleft;
			map_loaded[y][x].br = bottomright;
		}

		draw_objects_at_row(window, y);
	}
}

inline map_info get_map_info(platform_window* window) {
	map_info info;
	info.tile_width = get_tile_width(window);
	info.tile_height = get_tile_height(window);
	info.px_incline = info.tile_width/3; // info.tile_width/3; // offset*info.tile_width;
	info.px_raised_per_h = info.tile_height/2.5;
	return info;
}