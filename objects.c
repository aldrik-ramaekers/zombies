#include "include/objects.h"

box get_box_of_object(platform_window* window, object o) {
	map_info info = get_map_info(window);
	float render_x = (info.tile_width * o.position.x) + (info.px_incline * o.position.y);
	vec2f rendertl = (vec2f){render_x, info.tile_width * o.position.y - o.h*info.px_raised_per_h};
	vec2f rendertr = (vec2f){render_x + info.tile_width, info.tile_height * o.position.y - o.h*info.px_raised_per_h};
	vec2f renderbr = (vec2f){render_x + info.px_incline+info.tile_width, info.tile_height * o.position.y + info.tile_height - o.h*info.px_raised_per_h};
	vec2f renderbl = (vec2f){render_x + info.px_incline, info.tile_height * o.position.y + info.tile_height - o.h*info.px_raised_per_h};

	o.h += o.size.z;
	vec2f rendertl2 = (vec2f){render_x, info.tile_width * o.position.y - o.h*info.px_raised_per_h};
	vec2f rendertr2 = (vec2f){render_x + info.tile_width, info.tile_height * o.position.y - o.h*info.px_raised_per_h};
	vec2f renderbr2 = (vec2f){render_x + info.px_incline+info.tile_width, info.tile_height * o.position.y + info.tile_height - o.h*info.px_raised_per_h};
	vec2f renderbl2 = (vec2f){render_x + info.px_incline, info.tile_height * o.position.y + info.tile_height - o.h*info.px_raised_per_h};

	return (box){rendertl, rendertr, renderbl, renderbr, rendertl2, rendertr2, renderbl2, renderbr2};
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

	bool did_player_draw = false;
	int x_of_player = playerx;
	int y_of_player = playery;

	for (int i = 10; i >= 0; i--) {
		object o = get_object_at_tile(i, row);
		
		if (row == y_of_player && x_of_player == i) {
			draw_player(window);
		}

		draw_zombies_at_tile(window, i, row);
		
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
	create_box(4, 0, 0);
	create_box(1, 0, 0);
	create_box(0, 0, 0);

	create_box(0, 1, 0);
	create_box(3, 5, 0);

	spawn_zombie(2, 7);
}