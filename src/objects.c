#include "../include/objects.h"
#include "../include/wall_item.h"

box get_box_of_object(platform_window* window, object o) {
	return get_render_box_of_square(window, (vec3f){o.position.x, o.position.y, o.position.z}, o.size);
}

void render_box_with_outline(box box, color c) {
	render_quad_with_outline(box.tl_d, box.tr_d, box.bl_d, box.br_d, c); // down
	render_quad_with_outline(box.tl_u, box.tr_u, box.bl_u, box.br_u, c); // up

	render_quad_with_outline(box.tr_u, box.tr_d, box.br_u, box.br_d, c); // right
	render_quad_with_outline(box.tl_u, box.tl_d, box.bl_u, box.bl_d, c); // left
	render_quad_with_outline(box.bl_u, box.br_u, box.bl_d, box.br_d, c); // bottom

}

void render_quad_with_outline(vec2f tl, vec2f tr, vec2f bl, vec2f br, color c) {
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
		return;
	}
	log_info("Object limit reached.");
}

image* get_image_from_objecttype(object_type tile) {
	switch (tile)
	{
	case OBJECT_COBBLESTONEWALL1:
		return img_obj_wall1;
	case OBJECT_PLANTBOX1:
		return img_obj_plants;
	case OBJECT_METAL_WALL_FRONT:
		return img_metal_wall_front;
	default:
		return 0;
	}
}

void draw_objects(platform_window* window) {
	map_info info = get_map_info(window);

	for (int i = 0; i < MAX_OBJECTS; i++) {
		if (!loaded_map.objects[i].active) continue;
		object o = loaded_map.objects[i];
		OBJECT_RENDER_DEPTH((int)o.position.y);
		
		box box = get_box_of_object(window, o);

		image* img = get_image_from_objecttype(o.type);
		if (img) {
			renderer->render_image(img, box.tl_u.x, box.tl_u.y, 
				box.br_d.x - box.tl_d.x, box.br_d.y - box.tr_u.y);
		}
			/*
		render_quad_with_outline(box.tl_d, box.tr_d, box.bl_d, box.br_d, rgb(200,200,0));
		render_quad_with_outline(box.tl_u, box.tr_u, box.bl_u, box.br_u, rgb(200,200,0));
		render_quad_with_outline(box.tl_u, box.tl_d, box.bl_u, box.bl_d, rgb(200,200,0));
		render_quad_with_outline(box.bl_u, box.br_u, box.bl_d, box.br_d, rgb(200,200,0));
		*/
	}
}

void create_objects() {
	/*
	// rechts naar links op map.
	for (int i = MAP_SIZE_X-1; i >= 0; i--) {
		create_box(i, 0, 0, OBJECT_COBBLESTONEWALL1);
		create_box(i, MAP_SIZE_Y-1, 0, OBJECT_COBBLESTONEWALL1);
	}

	for (int i = MAP_SIZE_Y-1; i >= 0; i--) {
		create_box(0, i, 0, OBJECT_COBBLESTONEWALL1);
		create_box(MAP_SIZE_X-1, i, 0, OBJECT_COBBLESTONEWALL1);
	}

	create_box(16, 8, 0, OBJECT_PLANTBOX1);
	create_box(14, 8, 0, OBJECT_PLANTBOX1);
	create_box(11, 8, 0, OBJECT_PLANTBOX1);
	create_box(10, 8, 0, OBJECT_PLANTBOX1);

	create_box(15, 10, 0, OBJECT_PLANTBOX1);
	create_box(14, 10, 0, OBJECT_PLANTBOX1);
	create_box(13, 10, 0, OBJECT_PLANTBOX1);
	create_box(11, 10, 0, OBJECT_PLANTBOX1);

	create_spawner((vec2){15, 5});
	create_spawner((vec2){3, 8});
	*/
	create_spawner((vec2){11, 18});
	create_wallitem((vec3f){14, 1, 0}, WALLITEM_GUN, (wall_item_data){.gun = GUN_NOVA});
}