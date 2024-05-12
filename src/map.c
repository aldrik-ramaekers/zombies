#include "../include/map.h"
#include "../include/glass_doors.h"

int player_zoom = 30;
static int get_height_of_tile_tl(int current_height, int x, int y) {
	int highest_point = current_height;
	if (y > 0) {
		int tile_above = map_to_load.heightmap[y-1][x];
		if (tile_above > highest_point) {
			highest_point = tile_above;
		}
	}
	if (y > 0 && x > 0) {
		int tile_above = map_to_load.heightmap[y-1][x-1];
		if (tile_above > highest_point) {
			highest_point = tile_above;
		}
	}
	if (x > 0) {
		int tile_above = map_to_load.heightmap[y][x-1];
		if (tile_above > highest_point) {
			highest_point = tile_above;
		}
	}
	return highest_point;
}


static int get_height_of_tile_br(int current_height, int x, int y) {
	int highest_point = current_height;
	if (x < MAP_SIZE_X-1) {
		int tile_right = map_to_load.heightmap[y][x+1];
		if (tile_right > highest_point) {
			highest_point = tile_right;
		}
	}
	if (y < MAP_SIZE_Y-1 && x < MAP_SIZE_X-1) {
		int tile_bottom_right = map_to_load.heightmap[y+1][x+1];
		if (tile_bottom_right > highest_point) {
			highest_point = tile_bottom_right;
		}
	}
	if (y < MAP_SIZE_Y-1) {
		int tile_bottom = map_to_load.heightmap[y+1][x];
		if (tile_bottom > highest_point) {
			highest_point = tile_bottom;
		}
	}
	return highest_point;
}

static int get_height_of_tile_bl(int current_height, int x, int y) {
	int highest_point = current_height;
	if (y > 0 && x > 0) {
		int tile_left = map_to_load.heightmap[y][x-1];
		if (tile_left > highest_point) {
			highest_point = tile_left;
		}
	}
	if (y < MAP_SIZE_Y-1 && x > 0) {
		int tile_bottom_left = map_to_load.heightmap[y+1][x-1];
		if (tile_bottom_left > highest_point) {
			highest_point = tile_bottom_left;
		}
	}
	if (y < MAP_SIZE_Y-1) {
		int tile_bottom = map_to_load.heightmap[y+1][x];
		if (tile_bottom > highest_point) {
			highest_point = tile_bottom;
		}
	}
	return highest_point;
}

static int get_height_of_tile_tr(int current_height, int x, int y) {
	int highest_point = current_height;
	if (y > 0) {
		int tile_above = map_to_load.heightmap[y-1][x];
		if (tile_above > highest_point) {
			highest_point = tile_above;
		}
	}
	if (y > 0 && x < MAP_SIZE_X-1) {
		int tile_above_right = map_to_load.heightmap[y-1][x+1];
		if (tile_above_right > highest_point) {
			highest_point = tile_above_right;
		}
	}
	if (x < MAP_SIZE_X-1) {
		int tile_right = map_to_load.heightmap[y][x+1];
		if (tile_right > highest_point) {
			highest_point = tile_right;
		}
	}
	return highest_point;
}

static float distance_between_3f(vec3f v1, vec3f v2)
{
	return sqrt((v1.x-v2.x)*(v1.x-v2.x)+(v1.y-v2.y)*(v1.y-v2.y)+(v1.z-v2.z)*(v1.z-v2.z));
}

bool check_if_bullet_collided_with_section(float* dist_of_closest_intersect, vec2f bstart, 
	vec2f bend, vec2f l1, vec2f l2, vec2f* intersect_point_buf);

static bool ray_intersects_with_object(vec3f begin, vec3f end) {
	float dist_of_closest_intersect = __FLT_MAX__;
	vec2f bstart = (vec2f){begin.x, begin.y};
	vec2f bend = (vec2f){end.x, end.y};

	for (int i = 0; i < MAX_OBJECTS; i++) {
		object o = loaded_map.objects[i];
		if (!o.active) continue;
		if (begin.z <= o.position.z + o.size.z && begin.z >= o.position.z) {
			box obj_box = get_box_of_square((vec3f){o.position.x, o.position.y, o.position.z}, o.size);
			vec2f intersect_point;
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.bl_d, obj_box.br_d, &intersect_point)) {
				return true;
			}
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.tl_d, obj_box.tr_d, &intersect_point)) {
				return true;
			}
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.tl_d, obj_box.bl_d, &intersect_point)) {
				return true;
			}
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.tr_d, obj_box.br_d, &intersect_point)) {
				return true;
			}
		}
	}
	return false;
}

static bool ray_intersects_with_ground(vec3f begin, vec3f end) {
	float dirx = (end.x - begin.x);
	float diry = (end.y - begin.y);
	float dirz = (end.z - begin.z);
	float length = sqrt(dirx * dirx + diry * diry + dirz * dirz);
	dirx /= length;
	diry /= length;
	dirz /= length;
	double nr_tiles_to_check = length*4; // check 4 points per tile.
	for (int i = 1; i < nr_tiles_to_check; i++) {
		float xtocheck = begin.x + (dirx*i/4);
		float ytocheck = begin.y + (diry*i/4);
		float ztocheck = begin.z + (dirz*i/4);
		if (!is_in_bounds(xtocheck, ytocheck)) break;
		tile tile = get_tile_under_coords(xtocheck, ytocheck);
		float h = get_height_of_tile_under_coords(xtocheck, ytocheck);
		if (ztocheck < h) {
			return true;
		}
	}
	return false;
}

int sort_objects(const void * obj1, const void* obj2) {
	return (((object*)obj1)->position.y - ((object*)obj2)->position.y);
}

void load_mapdata_into_world() {
	loaded_map.width = map_to_load.width;
	loaded_map.height = map_to_load.height;

	// Load heightmap
	for (int y = 0; y < MAP_SIZE_Y; y++) {
		for (int x = MAP_SIZE_X-1; x >= 0; x--) {
			int h = map_to_load.heightmap[y][x];
			int highest_point_topleft = get_height_of_tile_tl(h, x, y);
			int highest_point_topright = get_height_of_tile_tr(h, x, y);
			int highest_point_bottomright = get_height_of_tile_br(h, x, y);
			int highest_point_bottomleft = get_height_of_tile_bl(h, x, y);

			if (x >= 205) map_to_load.tiles[y][x] = TILE_NONE;

			loaded_map.heightmap[y][x] = (tile){.height = h, .type = map_to_load.tiles[y][x], highest_point_topleft, highest_point_topright, highest_point_bottomleft, highest_point_bottomright};
			loaded_map.lightmap[y][x] = (light_data){0.0f,0.0f,0.0f,0.0f};
		}
	}

	// Load emitters
	for (int i = 0; i < MAX_LIGHT_EMITTERS; i++) {
		loaded_map.light_emitters[i] = map_to_load.light_emitters[i];
		loaded_map.light_emitters[i].position.z = 1;
	}

	// Load lightmap
	for (int l = 0; l < MAX_LIGHT_EMITTERS; l++) {
		light_emitter emitter = loaded_map.light_emitters[l];
		if (!emitter.active) continue;

		int ystart = emitter.position.y - emitter.range-2;
		int xstart = emitter.position.x - emitter.range-2;
		int yend = ystart + emitter.range*2 + 4;
		int xend = xstart + emitter.range*2 + 4;

		if (ystart >= MAP_SIZE_Y) ystart = MAP_SIZE_Y-1;
		if (xstart >= MAP_SIZE_X) xstart = MAP_SIZE_X-1;
		if (yend >= MAP_SIZE_Y) yend = MAP_SIZE_Y-1;
		if (xend >= MAP_SIZE_X) xend = MAP_SIZE_X-1;

		for (int y = ystart; y <= yend; y++) {
			for (int x = xstart; x <= xend; x++) {
							
				float dist_tl = distance_between_3f(emitter.position, (vec3f){x, y, loaded_map.heightmap[y][x].topleft});
				float dist_tr = distance_between_3f(emitter.position, (vec3f){x, y, loaded_map.heightmap[y][x].topright});
				float dist_bl = distance_between_3f(emitter.position, (vec3f){x, y, loaded_map.heightmap[y][x].bottomleft});
				float dist_br = distance_between_3f(emitter.position, (vec3f){x, y, loaded_map.heightmap[y][x].bottomright});

				if (dist_tl > emitter.range) {
					continue;
				}

				float p_tl = 1.0f - dist_tl / emitter.range;
				float p_tr = 1.0f - dist_tr / emitter.range;
				float p_bl = 1.0f - dist_bl / emitter.range;
				float p_br = 1.0f - dist_br / emitter.range;
				if (p_tl < 0.0f) p_tl = 0.0f;
				if (p_tr < 0.0f) p_tr = 0.0f;
				if (p_bl < 0.0f) p_bl = 0.0f;
				if (p_br < 0.0f) p_br = 0.0f;

				if (ray_intersects_with_ground((vec3f){x, y, loaded_map.heightmap[y][x].topleft}, emitter.position)) {
					p_tl = 0.0f;
				}
				if (ray_intersects_with_ground((vec3f){x, y, loaded_map.heightmap[y][x].topright}, emitter.position)) {
					p_tr = 0.0f;
				}
				if (ray_intersects_with_ground((vec3f){x, y, loaded_map.heightmap[y][x].bottomleft}, emitter.position)) {
					p_bl = 0.0f;
				}
				if (ray_intersects_with_ground((vec3f){x, y, loaded_map.heightmap[y][x].bottomright}, emitter.position)) {
					p_br = 0.0f;
				}
/*
				if (ray_intersects_with_object((vec3f){x, y, loaded_map.heightmap[y][x].topleft}, emitter.position)) {
					p_tl = 0.0f;
				}
				if (ray_intersects_with_object((vec3f){x, y, loaded_map.heightmap[y][x].topright}, emitter.position)) {
					p_tr = 0.0f;
				}
				if (ray_intersects_with_object((vec3f){x, y, loaded_map.heightmap[y][x].bottomleft}, emitter.position)) {
					p_bl = 0.0f;
				}
				if (ray_intersects_with_object((vec3f){x, y, loaded_map.heightmap[y][x].bottomright}, emitter.position)) {
					p_br = 0.0f;
				}
*/
				p_tl += loaded_map.lightmap[y][x].tl;
				p_tr += loaded_map.lightmap[y][x].tr;
				p_bl += loaded_map.lightmap[y][x].bl;
				p_br += loaded_map.lightmap[y][x].br;
				if (p_tl > 1.0f) p_tl = 1.0f;
				if (p_tr > 1.0f) p_tr = 1.0f;
				if (p_bl > 1.0f) p_bl = 1.0f;
				if (p_br > 1.0f) p_br = 1.0f;

				loaded_map.lightmap[y][x].tl = p_tl;
				loaded_map.lightmap[y][x].tr = p_tr;
				loaded_map.lightmap[y][x].bl = p_bl;
				loaded_map.lightmap[y][x].br = p_br;
			}
		}
	}

	// Load objects
	memcpy(loaded_map.objects, map_to_load.objects, sizeof(loaded_map.objects));

	// Load special objects
	for (int i = 0; i < MAX_OBJECTS; i++) {
		object o = loaded_map.objects[i];
		if (!o.active) continue;

		if (o.position.x >= 205) map_to_load.objects[i].active = 0;

		if (o.type == OBJECT_ZOMBIE_SPAWNER) {
			create_spawner((vec2){.x = o.position.x, .y = o.position.y});
		}

		if (o.type == OBJECT_GLASS_DOOR_H) {
			create_glass_door(o);
		}	
	}
}

void create_empty_map() {
	map_to_load.width = MAP_SIZE_X;
	map_to_load.height = MAP_SIZE_Y;

	for (int y = 0; y < MAP_SIZE_Y; y++) {
		for (int x = 0; x < MAP_SIZE_X; x++) {
			map_to_load.tiles[y][x] = TILE_FLOOR1;
		}
	}

	for (int x = 0; x < MAP_SIZE_X; x++) {
		map_to_load.objects[x] = (object){.active = true, .position = (vec3f){x, 0, 0}, .size = (vec3f){1,1,1}, .type = OBJECT_METAL_WALL};
	}

	//map_to_load.light_emitters[0] = (light_emitter){.brightness = 1.0f, .position = (vec3f){0, 0, 10}, .range = 20.0f, .active = true};
	//map_to_load.light_emitters[1] = (light_emitter){.brightness = 1.0f, .position = (vec3f){0, 30, 10}, .range = 20.0f, .active = true};

	load_mapdata_into_world();
}

void load_map_from_file() {

	// load map from file..
	file_content content = platform_read_file_content("data/maps/map1.dat", "rb");
	memcpy(&map_to_load, content.content, content.content_length);

	//map_to_load.width = MAP_SIZE_X;
	//map_to_load.height = MAP_SIZE_Y;
	//memcpy(map_to_load.heightmap, map, sizeof(map));

	loaded_map.width = map_to_load.width;
	loaded_map.height = map_to_load.height;

	load_mapdata_into_world();
}

tile get_tile_under_coords(float x, float y) {
	return loaded_map.heightmap[(int)(y)][(int)(x)];
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

static int round_up(int numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;

    int remainder = numToRound % multiple;
    if (remainder == 0)
        return numToRound;

    return numToRound + multiple - remainder;
}

inline int get_tile_width(platform_window* window) {
	//int tile_width = window->height / player_zoom;
	//if (window->width > window->height) tile_width = window->width / player_zoom;
	//return round_up(tile_width, 8);
	#ifdef MODE_DEBUG
	if (is_editing_map) return 32;
	#endif
	return 256/5.0f+1;
}

inline int get_tile_height(platform_window* window) {
	int tile_width = get_tile_width(window);
	return tile_width;
}

bool is_in_bounds(float x, float y) {
	return (x >= 0 && x <= MAP_SIZE_X && y >= 0 && y < MAP_SIZE_Y);
}

image* get_image_from_tiletype(tile_type tile) {
	switch (tile)
	{
		case TILE_FLOOR1: return img_tile_floor1;
		case TILE_FLOOR2: return img_tile_floor2;
		default: return 0;
	}
}

vec2 screen_pos_to_world_pos(platform_window* window, float x, float y) {
	map_info info = get_map_info(window);
	int tile_y = (y + _global_camera.y) / info.tile_height;
	int tile_x = (((x + _global_camera.x) - (info.px_incline * tile_y)) / info.tile_width);
	return (vec2){.x = (s32)tile_x, .y = (s32)tile_y};
}

vec2f world_pos_to_screen_pos(platform_window* window, float x, float y, float z) {
	map_info info = get_map_info(window);
	float xdiff_between_bottom_and_top = info.px_incline;
	float render_x =  (info.tile_width * x) + (xdiff_between_bottom_and_top * y);
	float render_y = info.tile_height * y;
	render_y -= z*info.px_raised_per_h;
	return (vec2f){.x = render_x, .y = render_y};
}

static void draw_backdrop(platform_window* window)
{
	//map_info info = get_map_info(window);
	int tile_size = 512;

	int tilemap_render_min_y = (_global_camera.y / tile_size)-2;
	int tilemap_render_max_y = tilemap_render_min_y + (window->height/ tile_size) + 3;

	int tilemap_render_min_x = (_global_camera.x / tile_size)-2;
	int tilemap_render_max_x = tilemap_render_min_x + (window->width/ tile_size) + 3;

	for (int y = tilemap_render_min_y; y <= tilemap_render_max_y; y++)
	{
		if (y < tilemap_render_min_y) continue;
		if (y > tilemap_render_max_y) continue;

		for (int x = tilemap_render_min_x; x <= tilemap_render_max_x; x++)
		{
			if (x < tilemap_render_min_x) continue;
			if (x > tilemap_render_max_x) continue;
			
			renderer->render_image(img_space_parallax, 
				((int)(_global_camera.x*0.9f)%512) + x*tile_size, 
				((int)(_global_camera.y*0.9f)%512) + y*tile_size, tile_size, tile_size);
		}
	}
}

static float offset = 0.0f;
static bool dir = true;
void draw_grid(platform_window* window) {
	map_info info = get_map_info(window);

	draw_backdrop(window);

	int tilemap_render_min_y = (_global_camera.y / info.tile_height);
	int tilemap_render_max_y = tilemap_render_min_y + (window->height/ info.tile_height) + 1;

	int tilemap_render_min_x = (_global_camera.x / info.tile_width);
	int tilemap_render_max_x = tilemap_render_min_x + (window->width/ info.tile_width) + 1;

	for (int y = 0; y < MAP_SIZE_Y; y++) {

		if (y < tilemap_render_min_y) continue;
		if (y > tilemap_render_max_y) continue;

		for (int x = 0; x < MAP_SIZE_X; x++) {

			if (x < tilemap_render_min_x) continue;
			if (x > tilemap_render_max_x) continue;

			tile tile = loaded_map.heightmap[y][x];

			float xdiff_between_bottom_and_top = info.px_incline;
			float render_x =  (info.tile_width * x) + (xdiff_between_bottom_and_top * y);
			float render_y = info.tile_height * y;
			render_y -= tile.height*info.px_raised_per_h;

			vec2f topleft;
			vec2f bottomleft;
			vec2f bottomright;
			vec2f topright;

			int highest_point_topleft = tile.topleft;
			int highest_point_topright = tile.topright;
			int highest_point_bottomright = tile.bottomright;
			int highest_point_bottomleft = tile.bottomleft;

			topleft = (vec2f){render_x, info.tile_width * y - highest_point_topleft*info.px_raised_per_h};
			topright = (vec2f){render_x + info.tile_width, info.tile_height * y - highest_point_topright*info.px_raised_per_h};
			bottomright = (vec2f){render_x + xdiff_between_bottom_and_top+info.tile_width, info.tile_height * y + info.tile_height - highest_point_bottomright*info.px_raised_per_h};
			bottomleft = (vec2f){render_x + xdiff_between_bottom_and_top, info.tile_height * y + info.tile_height - highest_point_bottomleft*info.px_raised_per_h};

			/*
			color c = rgb(205,205,205);
			if (highest_point_topleft > highest_point_bottomleft || highest_point_topright > highest_point_bottomright ||
				highest_point_topleft > highest_point_bottomright || highest_point_topright > highest_point_bottomleft ||
				highest_point_topright > highest_point_topleft || highest_point_bottomright > highest_point_bottomleft) c = rgb(165,165,165);
			if (highest_point_topleft < highest_point_bottomleft || highest_point_topright < highest_point_bottomright ||
				highest_point_topleft > highest_point_topright) c = rgb(255,255,255);
			*/
			
			float min_brightness = 150;
			image* img = get_image_from_tiletype(tile.type);
			if (img) {
				renderer->render_image_quad(img, 
					topleft.x, topleft.y, 
					bottomleft.x, bottomleft.y, 
					bottomright.x, bottomright.y, 
					topright.x, topright.y);

				color c_tl = rgba(0,0,0, (u8)(min_brightness * (1.0f - loaded_map.lightmap[y][x].tl)));
				color c_tr = rgba(0,0,0, (u8)(min_brightness * (1.0f - loaded_map.lightmap[y][x].tr)));
				color c_bl = rgba(0,0,0, (u8)(min_brightness * (1.0f - loaded_map.lightmap[y][x].bl)));
				color c_br = rgba(0,0,0, (u8)(min_brightness * (1.0f - loaded_map.lightmap[y][x].br)));
				renderer->render_quad_gradient(topleft.x, topleft.y, 
					bottomleft.x, bottomleft.y, 
					bottomright.x, bottomright.y, 
					topright.x, topright.y, c_tl, c_bl, c_br, c_tr);
			}

/*
			if (highest_point_topleft != highest_point_bottomright && highest_point_bottomleft == highest_point_topright) {
				if (highest_point_bottomleft < highest_point_topleft || highest_point_bottomleft < highest_point_bottomright) {
					renderer->render_tri(topleft.x, topleft.y, 
						bottomleft.x, bottomleft.y, 
						bottomright.x, bottomright.y, rgba(0,0,0,80));
				}
				//renderer->render_line(topleft.x, topleft.y, bottomright.x, bottomright.y, 1, rgb(0,0,255)); // diag
			}
*/

			loaded_map.heightmap[y][x].tl = topleft;
			loaded_map.heightmap[y][x].tr = topright;
			loaded_map.heightmap[y][x].bl = bottomleft;
			loaded_map.heightmap[y][x].br = bottomright;
		}
	}
}

inline map_info get_map_info(platform_window* window) {
	map_info info;
	info.tile_width = get_tile_width(window);
	info.tile_height = get_tile_height(window);
	info.px_incline = 0.0f; //info.tile_width/3; // info.tile_width/3; // offset*info.tile_width;
	info.px_raised_per_h = info.tile_height;
	return info;
}