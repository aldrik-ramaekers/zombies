#include "../include/editor.h"

float camera_x = 0;
float camera_y = 0;

typedef enum t_editor_state
{
	PLACING_TILE,
	RAISING_GROUND,
	LOWERING_GROUND,
} editor_state;

editor_state edit_state = PLACING_TILE;
tile_type tile_to_place = TILE_NONE;

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

	_next_camera_pos.x = -(window->width / 2) + camera_x;
	_next_camera_pos.y = -(window->height / 2) + camera_y;

	if (keyboard_is_key_down(KEY_LEFT_CONTROL) && keyboard_is_key_pressed(KEY_S)) {
		platform_write_file_content("../data/maps/map1.dat", "wb", (u8*)&map_to_load, sizeof(map_to_load));
		platform_write_file_content("data/maps/map1.dat", "wb", (u8*)&map_to_load, sizeof(map_to_load));
		log_info("Saved map");
	}
}

void draw_placing_rectangle(platform_window* window) {
	map_info info = get_map_info(window);
	int mouse_tile_y = (_global_mouse.y + _global_camera.y) / info.tile_height;
	int mouse_tile_x = (((_global_mouse.x + _global_camera.x) - (info.px_incline * mouse_tile_y)) / info.tile_width);

	if (mouse_tile_x < 0 || mouse_tile_y < 0) return;
	if (mouse_tile_x >= loaded_map.width || mouse_tile_y >= loaded_map.height) return;

	tile t = loaded_map.heightmap[mouse_tile_y][mouse_tile_x];
	renderer->render_rectangle_outline(t.tl.x, t.tl.y, t.tr.x - t.tl.x, t.br.y - t.tr.y, 2, rgb(255,0,0));

	switch (edit_state)
	{
	case PLACING_TILE:
		if (is_left_down()) {
			map_to_load.tiles[mouse_tile_y][mouse_tile_x] = tile_to_place;
			load_mapdata_into_world();
		}
		break;

	case RAISING_GROUND:
		if (is_left_clicked()) {
			map_to_load.heightmap[mouse_tile_y][mouse_tile_x]++;
			load_mapdata_into_world();
		}
		break;

	case LOWERING_GROUND:
		if (is_left_clicked()) {
			map_to_load.heightmap[mouse_tile_y][mouse_tile_x]--;
			load_mapdata_into_world();
		}
		break;
	
	default:
		break;
	}
}

static bool push_icon_button(int x, int y, int w, image* img) {
	if (img) renderer->render_image(img,_global_camera.x+ x,_global_camera.y+ y, w, w);
	renderer->render_rectangle_outline(_global_camera.x+ x,_global_camera.y+ y, w, w, 1, rgb(255,0,0));

	if (_global_mouse.x > x && _global_mouse.x < x + w && _global_mouse.y > y && _global_mouse.y < y + w) {
		renderer->render_rectangle(_global_camera.x+ x,_global_camera.y+ y, w, w, rgba(100,120,200,120));

		if (is_left_clicked()) {
			return true;
		}
	}

	return false;
}

void draw_tile_panel(platform_window* window) {
	int start_y = 1;
	int width = 200;
	int cols = 4;
	int tile_w = width / cols;

	renderer->render_rectangle(_global_camera.x, _global_camera.y, width, window->height, rgb(200,150,100));

	if (push_icon_button(tile_w*0, 0, tile_w, img_arrow_up)) {
		edit_state = RAISING_GROUND;
	}
	if (push_icon_button(tile_w*1, 0, tile_w, img_arrow_down)) {
		edit_state = LOWERING_GROUND;
	}
	if (push_icon_button(tile_w*2, 0, tile_w, img_cancel)) {
		tile_to_place = TILE_NONE;
		edit_state = PLACING_TILE;
	}

	int tile_start = TILE_NONE+1;
	for (int i = tile_start; i < TILE_END; i++) {
		int x = ((i-tile_start) % cols) * tile_w;
		int y = (start_y + ((i-tile_start) / cols)) * tile_w;

		image* img = get_image_from_tiletype((tile_type)i);
		if (push_icon_button(x, y, tile_w, img)) {
			tile_to_place = i;
			edit_state = PLACING_TILE;
		}
	}
}

void draw_editor(platform_window* window)
{
	if (is_editing_map) {
		draw_placing_rectangle(window);
		draw_tile_panel(window);
	}
}