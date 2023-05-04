#include "../include/editor.h"

float camera_x = 0;
float camera_y = 0;

typedef enum t_tile_editor_state
{
	PLACING_TILE,
	RAISING_GROUND,
	LOWERING_GROUND,
} tile_editor_state;

typedef enum t_editor_state
{
	EDITING_TILES,
	EDITING_OBJECTS,
	EDITING_LIGHTING,
} editor_state;

editor_state edit_state = EDITING_TILES;
tile_editor_state tile_edit_state = PLACING_TILE;
tile_type tile_to_place = TILE_NONE;

int editor_width = 200;
int borderw = 8;
int offset_y = 50;

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

	map_info info = get_map_info(window);
	int mouse_tile_y = (_global_mouse.y + _global_camera.y) / info.tile_height;
	int mouse_tile_x = (((_global_mouse.x + _global_camera.x) - (info.px_incline * mouse_tile_y)) / info.tile_width);

	if (mouse_tile_x < 0 || mouse_tile_y < 0) return;
	if (mouse_tile_x >= loaded_map.width || mouse_tile_y >= loaded_map.height) return;

	switch (tile_edit_state)
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
				if (map_to_load.heightmap[mouse_tile_y][mouse_tile_x] > 0) map_to_load.heightmap[mouse_tile_y][mouse_tile_x]--;
				load_mapdata_into_world();
			}
			break;
		
		default:
			break;
	}

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
	
	//renderer->render_rectangle_outline(t.tl.x, t.tl.y, t.tr.x - t.tl.x, t.br.y - t.tr.y, 2, rgb(255,0,0));

	renderer->render_line(t.tl.x, t.tl.y, t.tr.x, t.tr.y, 1, rgb(0,0,255)); // top
	renderer->render_line(t.tl.x, t.tl.y, t.bl.x, t.bl.y, 1, rgb(0,0,255)); // left
	renderer->render_line(t.tr.x, t.tr.y, t.br.x, t.br.y, 1, rgb(0,0,255)); // right
	renderer->render_line(t.bl.x, t.bl.y, t.br.x, t.br.y, 1, rgb(0,0,255)); // bottom
}

static bool push_icon_button(int x, int y, int w, image* img, bool isSelected) {
	if (img) renderer->render_image(img,_global_camera.x+ x,_global_camera.y+ y, w, w);
	renderer->render_rectangle_outline(_global_camera.x+ x,_global_camera.y+ y, w+1, w+1, 1, rgb(255,255,255));

	if (isSelected) {
		renderer->render_rectangle_outline(_global_camera.x+ x + 1,_global_camera.y+ y + 1, w-2, w-2, 3, rgb(50,50,200));
	}

	if (_global_mouse.x > x && _global_mouse.x < x + w && _global_mouse.y > y && _global_mouse.y < y + w) {
		renderer->render_rectangle(_global_camera.x+ x,_global_camera.y+ y, w, w, rgba(100,120,200,120));

		if (is_left_clicked()) {
			return true;
		}
	}

	return false;
}

void draw_tile_panel(platform_window* window) {
	int start_y = 1; // rows to skip
	int cols = 4;
	int tile_w = editor_width / cols;

	if (push_icon_button(tile_w*0, offset_y, tile_w, img_arrow_up, tile_edit_state == RAISING_GROUND)) {
		tile_edit_state = RAISING_GROUND;
	}
	if (push_icon_button(tile_w*1, offset_y, tile_w, img_arrow_down, tile_edit_state == LOWERING_GROUND)) {
		tile_edit_state = LOWERING_GROUND;
	}
	if (push_icon_button(tile_w*2, offset_y, tile_w, img_cancel, tile_edit_state == PLACING_TILE && tile_to_place == TILE_NONE)) {
		tile_to_place = TILE_NONE;
		tile_edit_state = PLACING_TILE;
	}

	int tile_start = TILE_NONE+1;
	for (int i = tile_start; i < TILE_END; i++) {
		int x = ((i-tile_start) % cols) * tile_w;
		int y = (start_y + ((i-tile_start) / cols)) * tile_w;

		y += offset_y;

		image* img = get_image_from_tiletype((tile_type)i);
		if (push_icon_button(x, y, tile_w, img, tile_edit_state == PLACING_TILE && tile_to_place == i)) {
			tile_to_place = i;
			tile_edit_state = PLACING_TILE;
		}
	}
}

void draw_lighting_panel(platform_window* window) {
	int row_h = 40;

	int count = 0;
	for (int i = 0; i < MAX_LIGHT_EMITTERS; i++) {
		light_emitter emitter = loaded_map.light_emitters[i];
		if (!emitter.active) continue;

		renderer->render_rectangle(_global_camera.x, _global_camera.y + offset_y + ((row_h+1)*count), editor_width, row_h, rgba(255,0,0,40));
	}
}

void draw_editor(platform_window* window)
{
	if (is_editing_map) {
		draw_placing_rectangle(window);

		renderer->render_rectangle(_global_camera.x, _global_camera.y, editor_width, window->height, rgb(73, 140, 138));
		renderer->render_rectangle(_global_camera.x + editor_width, _global_camera.y, borderw, window->height, rgb(70, 172, 194));

		if (push_icon_button(10, 10, 30, img_tiles, edit_state == EDITING_TILES)) {
			edit_state = EDITING_TILES;
		}
		if (push_icon_button(50, 10, 30, img_3d, edit_state == EDITING_OBJECTS)) {
			edit_state = EDITING_OBJECTS;
		}
		if (push_icon_button(90, 10, 30, img_sunny, edit_state == EDITING_LIGHTING)) {
			edit_state = EDITING_LIGHTING;
		}
		
		switch (edit_state)
		{
			case EDITING_TILES: draw_tile_panel(window); break;
			case EDITING_OBJECTS:  break;
			case EDITING_LIGHTING: draw_lighting_panel(window); break;
		}
		
	}
}