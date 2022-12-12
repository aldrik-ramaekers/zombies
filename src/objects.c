#include "../include/objects.h"

box get_box_of_object(platform_window* window, object o) {
	return get_render_box_of_square(window, (vec3f){o.position.x, o.position.y, o.h}, o.size);
}

void render_quad_with_outline(vec2f tl, vec2f tr, vec2f bl, vec2f br) {
	renderer->render_quad(
			tl.x, tl.y, 
			bl.x, bl.y, 
			br.x, br.y, 
			tr.x, tr.y, 
			rgb(200,200,0));

	renderer->render_line(tl.x, tl.y, tr.x, tr.y, 1, rgb(0,0,255)); // top
	renderer->render_line(tl.x, tl.y, bl.x, bl.y, 1, rgb(0,0,255)); // left
	renderer->render_line(tr.x, tr.y, br.x, br.y, 1, rgb(0,0,255)); // right
	renderer->render_line(bl.x, bl.y, br.x, br.y, 1, rgb(0,0,255)); // bottom
}

object get_object_at_tile(int x, int y) {
	for (int i = 0; i < max_objects; i++) {
		object o = objects[i];
		if (!o.active) continue;
		if (o.position.x == x && o.position.y == y) return o;
	}
	return (object){0};
}

void draw_objects_at_row(platform_window* window, int row) {
	map_info info = get_map_info(window);

	for (int i = MAP_SIZE_X-1; i >= 0; i--) {
		object o = get_object_at_tile(i, row);
	
		OBJECT_RENDER_DEPTH(o.h);
		draw_players_at_tile(window, i, row);

		draw_zombies_at_tile(window, i, row);

		draw_bullets(window);
		OBJECT_RENDER_DEPTH(o.h);
		
		if (!o.active) continue;
		box box = get_box_of_object(window, o);
		render_quad_with_outline(box.tl_b, box.tr_b, box.bl_b, box.br_b);
		render_quad_with_outline(box.tl_u, box.tr_u, box.bl_u, box.br_u);
		render_quad_with_outline(box.tl_u, box.tl_b, box.bl_u, box.bl_b);
		render_quad_with_outline(box.bl_u, box.br_u, box.bl_b, box.br_b);
	}
}

void create_box(float x, float y, float h) {
	for (int i = 0; i < max_objects; i++) {
		object o = objects[i];
		if (o.active) continue;

		objects[i].active = true;
		objects[i].position = (vec2f){x, y};
		objects[i].h = h;
		objects[i].size = (vec3f){1,1,2};
		break;
	}
}

void create_objects() {
	// rechts naar links op map.

	for (int i = MAP_SIZE_X-1; i >= 0; i--) {
		create_box(i, 0, 0);
		create_box(i, MAP_SIZE_Y-1, 0);
	}

	for (int i = MAP_SIZE_Y-1; i >= 0; i--) {
		create_box(0, i, 0);
		create_box(MAP_SIZE_Y-1, i, 0);
	}

	create_box(16, 8, 0);
	create_box(14, 8, 0);
	create_box(11, 8, 0);
	create_box(10, 8, 0);

	create_box(15, 10, 0);
	create_box(14, 10, 0);
	create_box(13, 10, 0);
	create_box(11, 10, 0);
}