#include "../include/objects.h"
#include "../include/wall_item.h"
#include "../include/glass_doors.h"

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
	case OBJECT_SPACE_CONTROL_PANEL2:
		return img_space_control_panel2;
	case OBJECT_SPACE_CONTROL_PANEL:
		return img_space_control_panel;
	case OBJECT_SPACE_WINDOW:
		return img_space_window;
	case OBJECT_METAL_WALL:
		return img_metal_wall;
	case OBJECT_METAL_WALL2:
		return img_metal_wall2;
	case OBJECT_CHAIR_UP:
		return img_chair_up;
	case OBJECT_SPACE_WINDOW_H:
		return img_space_window_h;
	case OBJECT_ZOMBIE_SPAWNER:
		return img_zombie_spawner;
	case OBJECT_LAMP_EAST:
		return img_lamp_east;
	case OBJECT_LAMP_SOUTH:
		return img_lamp_south;
	case OBJECT_METAL_TABLE_H:
		return img_metal_table_h;
	case OBJECT_CANTINE_GLASS:
		return img_cantine_glass;
	case OBJECT_GLASS_DOOR_H:
		return img_glass_door_h_closed;
	case OBJECT_BIG_CONTROL_PANEL:
		return img_big_control_panel;
	case OBJECT_CHAIR_DOWN:
		return img_chair_down;
	case OBJECT_COMPUTER_RACK:
		return img_computer_rack;
	case OBJECT_GENERATOR:
		return img_generator;
	case OBJECT_SCHOOL_BENCH:
		return img_school_bench;
	case OBJECT_SCHOOL_TABLE:
		return img_school_table;
	case OBJECT_LOCKER:
		return img_locker;
	case OBJECT_BAR_H:
		return img_bar_h;
	case OBJECT_BAR_V:
		return img_bar_v;
	case OBJECT_BAR_HV:
		return img_bar_hv;
	case OBJECT_CLUB_WALL1:
		return img_club_wall1;
	case OBJECT_CLUB_WALL2:
		return img_club_wall2;
	case OBJECT_CLUB_SEAT1:
		return img_club_seat1;
	case OBJECT_CLUB_SEAT2:
		return img_club_seat2;
	case OBJECT_CLUB_SEAT3:
		return img_club_seat3;
	case OBJECT_CLUB_SEAT4:
		return img_club_seat4;
	case OBJECT_CLUB_SEAT5:
		return img_club_seat5;
	case OBJECT_CLUB_SEAT6:
		return img_club_seat6;
	case OBJECT_CLUB_SEAT7:
		return img_club_seat7;
	case OBJECT_CLUB_WALL3:
		return img_club_wall3;
	case OBJECT_BOWLING_LANE:
		return img_bowling_lane;
	case OBJECT_BOWLING_LANE_END:
		return img_bowling_lane_end;
	case OBJECT_GLASS_DOOR_V:
		return img_glass_door_v_closed;
	default:
		return 0;
	}
}

void draw_objects(platform_window* window) {
	map_info info = get_map_info(window);

	float prev_y = 0;
	for (int i = 0; i < MAX_OBJECTS; i++) {
		if (!loaded_map.objects[i].active) continue;
		object o = loaded_map.objects[i];

		if (o.type == OBJECT_GLASS_DOOR_H) continue;
		if (o.type == OBJECT_GLASS_DOOR_V) continue;
		
		box box = get_box_of_object(window, o);

		image* img = get_image_from_objecttype(o.type);
		if (img) {
			renderer->render_image(img, box.tl_u.x, box.tl_u.y, 
				box.br_d.x - box.tl_d.x, box.br_d.y - box.tr_u.y);
		}
		//render_box_outline(box, rgb(255,0,0));

		if (prev_y < o.position.y-1) {
			prev_y = o.position.y-1;
			draw_zombies(window, prev_y, prev_y);
			#ifdef MODE_DEBUG 
			if (!is_editing_map) 
			#endif
			draw_players(window, prev_y, prev_y);
			draw_glass_doors(window, prev_y, prev_y);
		}
	}
}

void create_objects() {
	//create_spawner((vec2){11, 18});
	//create_wallitem((vec3f){14, 1, 0}, WALLITEM_GUN, (wall_item_data){.gun = GUN_NOVA});
}