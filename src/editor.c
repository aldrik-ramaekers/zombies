#include "../include/editor.h"

float camera_x = 0;
float camera_y = 0;

void update_editor(platform_window* window)
{
	if (keyboard_is_key_pressed(KEY_F1)) {
		is_editing_map = !is_editing_map;
		log_infox("Editing map: %d", is_editing_map);
	}

	if (!is_editing_map) return;

	float cam_speed = 500;
	if (keyboard_is_key_down(KEY_W)) {
		camera_y -= update_delta*cam_speed;
	}
	if (keyboard_is_key_down(KEY_S)) {
		camera_y += update_delta*cam_speed;
	}
	if (keyboard_is_key_down(KEY_A)) {
		camera_x -= update_delta*cam_speed;
	}
	if (keyboard_is_key_down(KEY_D)) {
		camera_x += update_delta*cam_speed;
	}

	if (keyboard_is_key_down(KEY_LEFT_CONTROL) && keyboard_is_key_pressed(KEY_S)) {
		platform_write_file_content("../data/maps/map1.dat", "wb", (u8*)&map_to_load, sizeof(map_to_load));
		platform_write_file_content("data/maps/map1.dat", "wb", (u8*)&map_to_load, sizeof(map_to_load));
		log_info("Saved map");
	}
}

void draw_editor(platform_window* window)
{
	if (is_editing_map) {
		_next_camera_pos.x = -(window->width / 2) + camera_x;
		_next_camera_pos.y = -(window->height / 2) + camera_y;

		map_info info = get_map_info(window);
		int mouse_tile_y = (_global_mouse.y + _global_camera.y) / info.tile_height;
		int mouse_tile_x = (((_global_mouse.x + _global_camera.x) - (info.px_incline * mouse_tile_y)) / info.tile_width);

		if (mouse_tile_x < 0 || mouse_tile_y < 0) return;
		if (mouse_tile_x >= loaded_map.width || mouse_tile_y >= loaded_map.height) return;

		tile t = loaded_map.heightmap[mouse_tile_y][mouse_tile_x];
		renderer->render_rectangle_outline(t.tl.x, t.tl.y, t.tr.x - t.tl.x, t.br.y - t.tr.y, 2, rgb(255,0,0));

		if (is_left_clicked()) {
			map_to_load.heightmap[mouse_tile_y][mouse_tile_x]++;
			load_mapdata_into_world();
		}
		if (is_right_clicked()) {
			map_to_load.heightmap[mouse_tile_y][mouse_tile_x]--;
			load_mapdata_into_world();
		}
	}
}