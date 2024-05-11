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

static void update_object_editor(platform_window* window);

static void update_tile_editor(platform_window* window) {
	map_info info = get_map_info(window);
	vec2 pos = screen_pos_to_world_pos(window, _global_mouse.x, _global_mouse.y);

	if (pos.x < 0 || pos.y < 0) return;
	if (pos.x >= loaded_map.width || pos.y >= loaded_map.height) return;

	switch (tile_edit_state)
	{
		case PLACING_TILE:
			if (is_left_down()) {
				map_to_load.tiles[pos.y][pos.x] = tile_to_place;
				load_mapdata_into_world();
			}
			break;

		case RAISING_GROUND:
			if (is_left_clicked()) {
				map_to_load.heightmap[pos.y][pos.x]++;
				load_mapdata_into_world();
			}
			break;

		case LOWERING_GROUND:
			if (is_left_clicked()) {
				if (map_to_load.heightmap[pos.y][pos.x] > 0) map_to_load.heightmap[pos.y][pos.x]--;
				load_mapdata_into_world();
			}
			break;
		
		default:
			break;
	}
}

void update_lighting_panel(platform_window* window, vec2 cursor_pos) {
	if (keyboard_is_key_pressed(KEY_ENTER)) {
		for (int i = 0; i < MAX_LIGHT_EMITTERS; i++) {
			light_emitter emitter = loaded_map.light_emitters[i];
			if (emitter.active) continue;

			map_to_load.light_emitters[i].position.x = cursor_pos.x;
			map_to_load.light_emitters[i].position.y = cursor_pos.y;
			map_to_load.light_emitters[i].position.z = 1;
			map_to_load.light_emitters[i].active = 1;
			map_to_load.light_emitters[i].brightness = 1.0f;
			map_to_load.light_emitters[i].range = 10.0f;

			load_mapdata_into_world();
			return;
		}
	}
}

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

/*
	if (_global_mouse.scroll_state == SCROLL_UP) {
		player_zoom++;
	}
	if (_global_mouse.scroll_state == SCROLL_DOWN) {
		player_zoom--;
	}
*/

	_next_camera_pos.x = -(window->width / 2) + camera_x;
	_next_camera_pos.y = -(window->height / 2) + camera_y;

	vec2 pos = screen_pos_to_world_pos(window, _global_mouse.x, _global_mouse.y);
	switch (edit_state)
	{
		case EDITING_TILES: update_tile_editor(window); break;
		case EDITING_OBJECTS: update_object_editor(window); break;
		case EDITING_LIGHTING: update_lighting_panel(window, pos); break;
	}


	if (keyboard_is_key_down(KEY_LEFT_CONTROL) && keyboard_is_key_pressed(KEY_S)) {
		#if 0
		map_data2* tmp = malloc(sizeof(map_data2));
		tmp->width = map_to_load.width;
		tmp->height = map_to_load.height;
		memcpy(tmp->heightmap, map_to_load.heightmap, sizeof(map_to_load.heightmap));
		memcpy(tmp->tiles, map_to_load.tiles, sizeof(map_to_load.tiles));
		memcpy(tmp->light_emitters, map_to_load.light_emitters, sizeof(map_to_load.light_emitters));

		for (int i = 0; i < MAX_OBJECTS; i++) {
			object o = map_to_load.objects[i];
			tmp->objects[i].active = o.active;
			tmp->objects[i].position = o.position;
			tmp->objects[i].size = o.size;
			tmp->objects[i].type = o.type;
			tmp->objects[i].collision = 1;
		}
		platform_write_file_content("../data/maps/map1.dat", "wb", (u8*)tmp, sizeof(map_data2));
		platform_write_file_content("data/maps/map1.dat", "wb", (u8*)tmp, sizeof(map_data2));
		log_info("Saved map");
		#else
		platform_write_file_content("../data/maps/map1.dat", "wb", (u8*)&map_to_load, sizeof(map_to_load));
		platform_write_file_content("data/maps/map1.dat", "wb", (u8*)&map_to_load, sizeof(map_to_load));
		log_info("Saved map");
		#endif
	}
}

void draw_floor_grid(platform_window* window) {
	map_info info = get_map_info(window);

	for (int y = 0; y < MAP_SIZE_Y; y++) {
		int x1 = 0;
		int y1 = y*info.tile_height;
		int x2 = MAP_SIZE_X*info.tile_width;
		int y2 = y*info.tile_height;
		renderer->render_line(x1, y1, x2, y2, 1, rgba(255,0,0,80));	
	}

	for (int x = MAP_SIZE_X-1; x >= 0; x--) {
		int x1 = x*info.tile_width;
		int y1 = 0;
		int x2 = x*info.tile_width;
		int y2 = MAP_SIZE_Y*info.tile_height;
		renderer->render_line(x1, y1, x2, y2, 1, rgba(255,0,0,80));	
	}
}

void draw_placing_rectangle(platform_window* window) {
	map_info info = get_map_info(window);
	int mouse_tile_y = (_global_mouse.y + _global_camera.y) / info.tile_height;
	int mouse_tile_x = (((_global_mouse.x + _global_camera.x) - (info.px_incline * mouse_tile_y)) / info.tile_width);

	if (mouse_tile_x < 0 || mouse_tile_y < 0) return;
	if (mouse_tile_x >= loaded_map.width || mouse_tile_y >= loaded_map.height) return;

	tile t = loaded_map.heightmap[mouse_tile_y][mouse_tile_x];
	box box = get_render_box_of_square(window, (vec3f){mouse_tile_x, mouse_tile_y, 0}, (vec3f){1,1,t.height});

	render_fill_quad_with_outline(box.tl_d, box.tr_d, box.bl_d, box.br_d, rgba(255,0,0, 70));
	render_fill_quad_with_outline(box.tl_u, box.tr_u, box.bl_u, box.br_u, rgba(255,0,0, 70));
	render_fill_quad_with_outline(box.tl_u, box.tl_d, box.bl_u, box.bl_d, rgba(255,0,0, 70));
	render_fill_quad_with_outline(box.bl_u, box.br_u, box.bl_d, box.br_d, rgba(255,0,0, 70));
}

static bool push_icon_button(int x, int y, int w, image* img, bool isSelected) {
	if (img) {
		int imgw = w;
		int imgh = w;
		if (img->height > img->width) imgw = w*(img->width/(float)img->height);
		if (img->height < img->width) imgh = w*(img->height/(float)img->width);
		renderer->render_image(img,_global_camera.x+ x,_global_camera.y+ y, imgw, imgh);
	}
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
	static bool dragging_light = false;
	static int dragging_light_index = 0;
	static int drag_start_x = 0;
	static int drag_start_y = 0;
	static float orig_x = 0;
	static float orig_y = 0;

	int row_h = 20;
	int offsety = 0;

	for (int i = 0; i < MAX_LIGHT_EMITTERS; i++) {
		light_emitter emitter = loaded_map.light_emitters[i];
		if (!emitter.active) continue;

		renderer->render_rectangle(_global_camera.x, _global_camera.y + offset_y + row_h + offsety, editor_width, row_h, rgba(255,0,0,40));

		char buf[50];
		sprintf(buf, "{x: %.0f y: %.0f, z: %.0f} {%.0f}", emitter.position.x, emitter.position.y, emitter.position.z, emitter.range);
		renderer->render_text(fnt_20, _global_camera.x, _global_camera.y + offset_y + row_h + offsety + 5, buf, rgb(0,0,0));

		vec2f pos = world_pos_to_screen_pos(window, emitter.position.x, emitter.position.y, emitter.position.z);
		renderer->render_rectangle(pos.x-3, pos.y-3, 36, 36, rgb(100,0,0));
		renderer->render_rectangle(pos.x, pos.y, 30, 30, rgb(255,0,0));
		renderer->render_image(img_sunny, pos.x, pos.y, 30, 30);

		if (_global_mouse.x + _global_camera.x > pos.x && 
			_global_mouse.x + _global_camera.x < pos.x + 30 && 
			_global_mouse.y + _global_camera.y > pos.y && 
			_global_mouse.y + _global_camera.y < pos.y + 30) {
			if (is_left_clicked()) {
				dragging_light = true;
				dragging_light_index = i;
				drag_start_x = _global_mouse.x;
				drag_start_y = _global_mouse.y;
				orig_x = emitter.position.x;
				orig_y = emitter.position.y;
			}
		}

		if (dragging_light && dragging_light_index == i) {
			if (is_left_down()) {
				int newx = (_global_mouse.x - drag_start_x) - _global_camera.x;
				int newy = (_global_mouse.y - drag_start_y) - _global_camera.y;
				vec2 newpos = screen_pos_to_world_pos(window, newx, newy);

				map_to_load.light_emitters[i].position.x = orig_x + newpos.x;
				map_to_load.light_emitters[i].position.y = orig_y + newpos.y;

				if (_global_mouse.scroll_state == SCROLL_UP) {
					map_to_load.light_emitters[i].range++;
				}
				if (_global_mouse.scroll_state == SCROLL_DOWN) {
					map_to_load.light_emitters[i].range--;
				}
				load_mapdata_into_world();
			}
			else {
				dragging_light = false;
			}
		}

		offsety+=row_h+1;
	}
}

typedef enum t_object_editor_state {
	OBJECT_EDITOR_SELECTING,
	OBJECT_EDITOR_PLACING,
} object_editor_state;

object_editor_state object_edit_state = OBJECT_EDITOR_SELECTING;
object_type object_to_place = OBJECT_NONE;
vec2 select_start = {0,0};
vec2 select_end = {0,0};
static bool is_selecting = false;
static bool is_dragging = false;

void update_object_selection(platform_window* window, vec2 cursor_pos) {
	static int drag_start_x = 0;
	static int drag_start_y = 0;
	static vec2 drag_select_start = {0,0};
	static vec2 drag_select_end = {0,0};

	if (is_left_clicked()) {
		if (rect_contains_point(cursor_pos, select_start, select_end)) {
			is_dragging = true;
			drag_start_x = cursor_pos.x;
			drag_start_y = cursor_pos.y;
			drag_select_start = select_start;
			drag_select_end = select_end;
		}
		else {
			select_start = cursor_pos;
			select_end = cursor_pos;
			is_selecting = true;
		}	
	}

	if (is_selecting) {
		if (is_left_down()) {
			select_end = cursor_pos;
		}
		if (is_left_released()) {
			select_end = cursor_pos;
			is_selecting = false;
		}
	}
	if (is_dragging) {
		if (is_left_down()) {
			vec2 newpos = cursor_pos;
			int diffx = newpos.x - drag_start_x;
			int diffy = newpos.y - drag_start_y;

			select_start.x += diffx;
			select_start.y += diffy;
			select_end.x += diffx;
			select_end.y += diffy;

			array l = array_create(sizeof(object*));
			array_reserve(&l, 200);
			for (int y = drag_select_start.y; y <= drag_select_end.y; y++)
			{
				for (int x = drag_select_start.x; x <= drag_select_end.x; x++)
				{
					object* obj = get_object_at_tile_from_mapfile(x, y);
					if (obj) {
						bool already_moved = false;
						for (int i = 0; i < l.length; i++) {
							object* existing_obj = *(object**)array_at(&l, i);
							if (existing_obj == obj) already_moved = true;
						}
						if (already_moved) {
							continue;
						}

						array_push(&l, (void*)&obj);

						int index = (obj-map_to_load.objects);
						loaded_map.objects[index].position.x += diffx;
						loaded_map.objects[index].position.y += diffy;
					}
				}
			}

			array_destroy(&l);

			drag_start_x = newpos.x;
			drag_start_y = newpos.y;
		}
		else {
			for (int y = drag_select_start.y; y <= drag_select_end.y; y++)
			{
				for (int x = drag_select_start.x; x <= drag_select_end.x; x++)
				{
					object* obj = get_object_at_tile_from_mapfile(x, y);
					if (obj) {
						int index = (obj-map_to_load.objects);
						map_to_load.objects[index].position.x = loaded_map.objects[index].position.x;
						map_to_load.objects[index].position.y = loaded_map.objects[index].position.y;
					}
				}
			}

			load_mapdata_into_world();
			is_dragging = false;
		}
	}
	

	if (!is_selecting) {
		if (keyboard_is_key_down(KEY_DELETE)) {
			for (int y = select_start.y; y <= select_end.y; y++)
			{
				for (int x = select_start.x; x <= select_end.x; x++)
				{
					object* obj = get_object_at_tile_from_mapfile(x, y);
					if (obj) obj->active = false;
				}
			}
			load_mapdata_into_world();
		}
	}
}

void update_object_editor(platform_window* window) {
	map_info info = get_map_info(window);
	vec2 pos = screen_pos_to_world_pos(window, _global_mouse.x, _global_mouse.y);

	if (keyboard_is_key_down(KEY_ESCAPE)) {
		object_edit_state = OBJECT_EDITOR_SELECTING;
	}
	if (_global_mouse.x < editor_width) return;

	if (pos.x < 0 || pos.y < 0) return;
	if (pos.x >= loaded_map.width || pos.y >= loaded_map.height) return;

	switch (object_edit_state)
	{
		case OBJECT_EDITOR_PLACING:
			if (is_left_clicked()) {
				object obj = object_dict[object_to_place-1];
				obj.position.x = pos.x;
				obj.position.y = pos.y;
				obj.active = true;
				add_object(obj);
				load_mapdata_into_world();
			}
			break;

		case OBJECT_EDITOR_SELECTING:
			update_object_selection(window, pos);
			break;

		default:
			break;
	}
}


void draw_object_panel(platform_window* window) {
	int start_y = 1; // rows to skip
	int cols = 4;
	int tile_w = editor_width / cols;

	int tile_start = OBJECT_NONE+1;
	for (int i = tile_start; i < OBJECT_END; i++) {
		int x = ((i-tile_start) % cols) * tile_w;
		int y = (start_y + ((i-tile_start) / cols)) * tile_w;

		y += offset_y;

		image* img = get_image_from_objecttype((object_type)i);
		if (push_icon_button(x, y, tile_w, img, object_to_place == (object_type)i)) {
			object_to_place = (object_type)i;
			object_edit_state = OBJECT_EDITOR_PLACING;
		}
	}

	if (object_edit_state == OBJECT_EDITOR_SELECTING)
	{
		vec2f start = world_pos_to_screen_pos(window, select_start.x, select_start.y, 0);
		vec2f end = world_pos_to_screen_pos(window, select_end.x - select_start.x+1, select_end.y - select_start.y+1, 0);
		renderer->render_rectangle(start.x, start.y, end.x, end.y, rgba(255,0,0,100));
	}
}


void draw_editor(platform_window* window)
{
	if (is_editing_map) {
		draw_floor_grid(window);
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
			case EDITING_OBJECTS: draw_object_panel(window); break;
			case EDITING_LIGHTING: draw_lighting_panel(window); break;
		}
		
	}
}