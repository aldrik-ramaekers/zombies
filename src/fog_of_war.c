#include "../include/fog_of_war.h"

void draw_fog_of_war(platform_window* window)
{
	map_info info = get_map_info(window);

	int tilemap_render_min_y = (_global_camera.y / info.tile_height);
	int tilemap_render_max_y = tilemap_render_min_y + (window->height/ info.tile_height) + 1;

	int tilemap_render_min_x = (_global_camera.x / info.tile_width);
	int tilemap_render_max_x = tilemap_render_min_x + (window->width/ info.tile_width) + 1;

	static float dance_floor_disco_timestamp = 0.0f;
	dance_floor_disco_timestamp += update_delta;

	for (int y = 0; y < MAP_SIZE_Y; y++) {

		if (y < tilemap_render_min_y) continue;
		if (y > tilemap_render_max_y) continue;

		for (int x = 0; x < MAP_SIZE_X; x++) {

			if (x < tilemap_render_min_x) continue;
			if (x > tilemap_render_max_x) continue;

			tile tile = loaded_map->heightmap[y][x];

			float xdiff_between_bottom_and_top = info.px_incline;
			float render_x =  (info.tile_width * x) + (xdiff_between_bottom_and_top * y);
			float render_y = info.tile_height * y;
			render_y -= tile.height*info.px_raised_per_h;

			float brightness = 1.0f;

			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (!players[i].active) continue;

				float dist = distance_between((vec2f){.x = players[i].playerx, .y = players[i].playery}, 
					(vec2f){.x = x, .y = y});
				if (dist < 40.0f) {
					float new_brightness = 1.0f / (40.0f - dist);
					if (new_brightness < brightness) brightness = new_brightness;
					break;
				}

				bullet b1 = {.active = true, .position = {.x = players[i].playerx, .y = players[i].playery, .z = 0.0f}, .endx = x, .endy = y};
				object_type obj_collision = check_if_bullet_collided_with_object(&b1, window);
				if (obj_collision != OBJECT_NONE) {
					brightness = 1.0f;
				}
			}
			
			renderer->render_rectangle(render_x, render_y, info.tile_width, info.tile_height, rgba(0,0,0, (brightness*220.0f)));
		}
	}
}