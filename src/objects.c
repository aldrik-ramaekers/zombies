#include "../include/objects.h"
#include "../include/wall_item.h"

box get_box_of_object(platform_window* window, object o) {
	return get_render_box_of_square(window, (vec3f){o.position.x, o.position.y, o.position.z}, o.size);
}

void render_fill_box_with_outline(box box, color c) {
	render_fill_quad_with_outline(box.tl_d, box.tr_d, box.bl_d, box.br_d, c); // down
	render_fill_quad_with_outline(box.tl_u, box.tr_u, box.bl_u, box.br_u, c); // up

	render_fill_quad_with_outline(box.tr_u, box.tr_d, box.br_u, box.br_d, c); // right
	render_fill_quad_with_outline(box.tl_u, box.tl_d, box.bl_u, box.bl_d, c); // left
	render_fill_quad_with_outline(box.bl_u, box.br_u, box.bl_d, box.br_d, c); // bottom

}

void render_quad_outline(vec2f tl, vec2f tr, vec2f bl, vec2f br, color c) {
	renderer->render_line(tl.x, tl.y, tr.x, tr.y, 1, rgba(0,0,255, c.a)); // top
	renderer->render_line(tl.x, tl.y, bl.x, bl.y, 1, rgba(0,0,255, c.a)); // left
	renderer->render_line(tr.x, tr.y, br.x, br.y, 1, rgba(0,0,255, c.a)); // right
	renderer->render_line(bl.x, bl.y, br.x, br.y, 1, rgba(0,0,255, c.a)); // bottom
}
void render_box_outline(box box, color c) {
	render_quad_outline(box.tl_d, box.tr_d, box.bl_d, box.br_d, c); // down
	render_quad_outline(box.tl_u, box.tr_u, box.bl_u, box.br_u, c); // up

	render_quad_outline(box.tr_u, box.tr_d, box.br_u, box.br_d, c); // right
	render_quad_outline(box.tl_u, box.tl_d, box.bl_u, box.bl_d, c); // left
	render_quad_outline(box.bl_u, box.br_u, box.bl_d, box.br_d, c); // bottom
}

void render_fill_quad_with_outline(vec2f tl, vec2f tr, vec2f bl, vec2f br, color c) {
	renderer->render_quad(
			tl.x, tl.y, 
			bl.x, bl.y, 
			br.x, br.y, 
			tr.x, tr.y, 
			c);

	renderer->render_line(tl.x, tl.y, tr.x, tr.y, 1, rgba(0,0,255, c.a)); // top
	renderer->render_line(tl.x, tl.y, bl.x, bl.y, 1, rgba(0,0,255, c.a)); // left
	renderer->render_line(tr.x, tr.y, br.x, br.y, 1, rgba(0,0,255, c.a)); // right
	renderer->render_line(bl.x, bl.y, br.x, br.y, 1, rgba(0,0,255, c.a)); // bottom
}

object* get_object_at_tile_from_mapfile(float x, float y) {
	for (int i = 0; i < MAX_OBJECTS; i++) {
		object o = map_to_load.objects[i];
		if (!o.active) continue;
		if (x >= o.position.x && x < o.position.x + o.size.x && y >= o.position.y && y < o.position.y + o.size.y) return &map_to_load.objects[i];
	}
	return 0;
}

object* get_pobject_at_tile(float x, float y) {
	for (int i = 0; i < MAX_OBJECTS; i++) {
		object o = loaded_map.objects[i];
		if (!o.active) continue;	
		if (x >= o.position.x && x < o.position.x + o.size.x && y >= o.position.y && y < o.position.y + o.size.y) return &loaded_map.objects[i];
	}
	return 0;
}

object get_object_at_tile(float x, float y) {
	for (int i = 0; i < MAX_OBJECTS; i++) {
		object o = loaded_map.objects[i];
		if (!o.active) continue;	
		if (x >= o.position.x && x < o.position.x + o.size.x && y >= o.position.y && y < o.position.y + o.size.y) return o;
	}
	return (object){0};
}

void add_object(object obj) {
	object existing_obj = get_object_at_tile(obj.position.x, obj.position.y);
	if (existing_obj.active) {
		log_info("Space occupied, cannot place object.");
		return;
	}

	for (int i = 0; i < MAX_OBJECTS; i++) {
		object o = map_to_load.objects[i];
		if (o.active) continue;	
		map_to_load.objects[i] = obj;
		map_to_load.objects[i].active = true;

		// sort y-axis
		qsort(map_to_load.objects, MAX_OBJECTS, sizeof(object), sort_objects);
		return;
	}

	log_info("Object limit reached.");
}

// @NEWOBJECT
image* get_image_from_objecttype(object_type tile) {
	switch (tile)
	{
	case OBJECT_SPACE_CONTROL_PANEL:
		return img_space_control_panel;
	case OBJECT_SPACE_WINDOW:
		return img_space_window;
	case OBJECT_METAL_WALL:
		return img_metal_wall;
	case OBJECT_METAL_WALL2:
		return img_metal_wall2;
	default:
		return 0;
	}
}

void draw_objects(platform_window* window) {
	map_info info = get_map_info(window);

	uint32_t prev_y = 0;
	for (int i = 0; i < MAX_OBJECTS; i++) {
		if (!loaded_map.objects[i].active) continue;
		object o = loaded_map.objects[i];
		
		box box = get_box_of_object(window, o);

		image* img = get_image_from_objecttype(o.type);
		if (img) {
			renderer->render_image(img, box.tl_u.x, box.tl_u.y, 
				box.br_d.x - box.tl_d.x, box.br_d.y - box.tr_u.y);
		}
		render_box_outline(box, rgb(255,0,0));

		if (prev_y < o.position.y) {
			prev_y = o.position.y;
			draw_zombies(window, prev_y, prev_y);
			draw_players(window, prev_y, prev_y);
		}
	}

	draw_zombies(window, prev_y, MAP_SIZE_Y);
	draw_players(window, prev_y, MAP_SIZE_Y);
}

void create_objects() {
	create_spawner((vec2){11, 18});
	create_wallitem((vec3f){14, 1, 0}, WALLITEM_GUN, (wall_item_data){.gun = GUN_NOVA});
}