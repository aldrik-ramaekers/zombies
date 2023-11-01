#include "../include/throwables.h"
#include "../include/audio.h"

void throw_throwable(platform_window* window, u32 id, throwable_type type, float dirx, float diry) {
	for (int i = 0; i < max_throwables; i++) {
		if (throwables[i].active) continue;

		player* p = get_player_by_id(id);
		if (!p) {
			log_info("User with unknown id throwing stuff");
		}

		throwable t = {.active = true, .state = THROWABLE_FLYING, .alive_time = 0.0f, .type = type, .direction = (vec3f){.x = dirx*1.5f, .y = diry*1.5f, .z = -0.2f}, 
			.player_id = id, .position = (vec3f){.x = p->playerx, .y = p->playery, .z = p->height}};

		t.sprite = create_sprite(img_grenade_explode, 12, 96, 96, 0.1f);

		switch(type) {
			case THROWABLE_GRENADE: t.damage = 1500; break;
		}

		throwables[i] = t;
		break;
	}
}

bool check_if_throwable_collided_with_object(throwable* b, vec3f oldpos, vec3f newpos, vec3f* direction) {
	bool result = false;

	for (int i = 0; i < MAX_OBJECTS; i++) {
		object o = loaded_map.objects[i];
		if (!o.active) continue;
		if (b->position.z <= o.h + o.size.z && b->position.z >= o.h) {
			box obj_box = get_box_of_square((vec3f){o.position.x, o.position.y, o.h}, o.size);

			//if (o.type == OBJECT_PLANTBOX1 && oldpos.x < o.position.x && newpos.x > o.position.x) {
			//	log_infox("intersect {%.2f %.2f}{%.2f %.2f} {%.2f %.2f}{%.2f %.2f}",
			//		oldpos.x, oldpos.y, newpos.x, newpos.y, o.position.x, o.position.y, o.position.x, o.position.y + o.size.y);
			//}

			// top
			if (lines_intersect((vec2f){.x = oldpos.x, .y = oldpos.y}, (vec2f){.x = newpos.x, .y = newpos.y}, 
								(vec2f){.x = o.position.x, .y = o.position.y}, (vec2f){.x = o.position.x + o.size.x, .y = o.position.y})) {
				result = true;
				direction->y = -direction->y;
			}

			// bottom
			else if (lines_intersect((vec2f){.x = oldpos.x, .y = oldpos.y}, (vec2f){.x = newpos.x, .y = newpos.y}, 
								(vec2f){.x = o.position.x, .y = o.position.y + o.size.y}, (vec2f){.x = o.position.x + o.size.x, .y = o.position.y + o.size.y})) {
				result = true;
				direction->y = -direction->y;
			}

			// left
			else if (lines_intersect((vec2f){.x = oldpos.x, .y = oldpos.y}, (vec2f){.x = newpos.x, .y = newpos.y}, 
								(vec2f){.x = o.position.x, .y = o.position.y}, (vec2f){.x = o.position.x, .y = o.position.y + o.size.y})) {
				result = true;
				direction->x = -direction->x;
			}
			
			// right
			else if (lines_intersect((vec2f){.x = oldpos.x, .y = oldpos.y}, (vec2f){.x = newpos.x, .y = newpos.y}, 
								(vec2f){.x = o.position.x + o.size.x, .y = o.position.y}, (vec2f){.x = o.position.x + o.size.x, .y = o.position.y + o.size.y})) {
				result = true;
				direction->x = -direction->x;
			}

			if (result) {
				b->position = oldpos;	
				return true;
			}
		}
	}

	return result;
}

void explode_grenade(throwable t) {
	int max_explosion_range = 3;
	
	for (int i = 0; i < SERVER_MAX_ZOMBIES; i++) {
		zombie o = zombies[i];
		if (!o.alive) continue;

		if (t.position.z <= o.position.z + o.size.z && t.position.z >= t.position.z) {
			
			vec3f grenade_center = get_center_of_square(t.position, grenade_explosion_size);
			vec3f zombie_center = get_center_of_square(o.position, o.size);
			float dist_between_grenade_and_zombie = distance_between_3f(o.position, t.position);
			if (dist_between_grenade_and_zombie > max_explosion_range) continue;

			float damage_multiplier = 1.0f - (dist_between_grenade_and_zombie / max_explosion_range);
			if (hit_zombie(i, t.damage*damage_multiplier)) {
				player* p = get_player_by_id(t.player_id);
				if (p) p->kills++;
			}
		}
	}
}

void update_throwables_server() {
	float speed = 7.0f * SERVER_TICK_RATE;
	float gravity = 0.015f;
	float speed_decrease = 0.97f;

	for (int i = 0; i < max_throwables; i++) {
		throwable b = throwables[i];
		if (!b.active) continue;

		throwables[i].rotation += SERVER_TICK_RATE*3.0f*throwables[i].bounces;

		throwables[i].alive_time += SERVER_TICK_RATE;
		if (throwables[i].alive_time >= 2.0f) {

			if (throwables[i].state == THROWABLE_FLYING) {
				add_throwable_audio_event_to_queue(EVENT_EXPLODE_THROWABLE, b.type, b.player_id, b.position);

				switch(b.type) {
					case THROWABLE_GRENADE: explode_grenade(b); break;
				}
			}

			throwables[i].state = THROWABLE_EXPLODED;
			update_sprite(&throwables[i].sprite);
		}
		if (throwables[i].alive_time >= 3.2f) {
			throwables[i].active = false;
			continue;
		}

		player *p = get_player_by_id(b.player_id);
		if (!p) continue; 

		vec3f oldpos = throwables[i].position;
		
		// move forward
		throwables[i].position.x += throwables[i].direction.x * speed;
		throwables[i].position.y += throwables[i].direction.y * speed;

		// gravity
		if (throwables[i].direction.z != 0) throwables[i].direction.z += gravity;
		throwables[i].position.z -= throwables[i].direction.z;

		throwables[i].direction.x *= speed_decrease;
		throwables[i].direction.y *= speed_decrease;

		// bouncing off floor
		float floor = get_height_of_tile_under_coords(throwables[i].position.x, throwables[i].position.y);
		if (throwables[i].position.z < floor && throwables[i].direction.z != 0) {
			throwables[i].position.z = floor;
			throwables[i].direction.z = -throwables[i].direction.z*0.7;
			throwables[i].bounces++;

			add_audio_event_to_queue(EVENT_BOUNCE_THROWABLE, b.player_id, b.position);

			if (throwables[i].bounces >= 3) throwables[i].direction.z = 0;
		}

		if (check_if_throwable_collided_with_object(&throwables[i], oldpos, throwables[i].position, &throwables[i].direction)) {
			add_audio_event_to_queue(EVENT_BOUNCE_THROWABLE, b.player_id, b.position);
		}
	}
}


void draw_throwables(platform_window* window) {
	map_info info = get_map_info(window);

	for (int i = 0; i < max_throwables; i++) {
		throwable t = throwables[i];
		if (!t.active) continue;

		THROWABLE_RENDER_DEPTH((int)t.position.y);

		float throwable_render_x = t.position.x*info.tile_width + (t.position.y*info.px_incline);
		float throwable_render_y = t.position.y*info.tile_height - (t.position.z*info.px_raised_per_h);

		if (t.state == THROWABLE_EXPLODED) {
			vec3f explode_location = t.position;
			explode_location.x -= 0.9f;
			explode_location.y -= 0.9f;
			box box = get_render_box_of_square(window, explode_location, grenade_explosion_size);
			
			sprite_frame frame = sprite_get_frame(&throwables[i].sprite);

			renderer->render_image_quad_partial(img_grenade_explode, 
				box.tl_u.x, box.tl_u.y,
				box.bl_d.x, box.bl_d.y, 
				box.br_d.x, box.br_d.y, 
				box.tr_u.x, box.tr_u.y, 
				frame.tl, frame.tr, frame.bl, frame.br);
		}
		else if (t.state == THROWABLE_FLYING) {
			renderer->render_set_rotation(t.rotation);

			box full_box = get_render_box_of_square(window, t.position, (vec3f){0.2, 0.2, 0.2});
			renderer->render_image(img_grenade, full_box.tl_u.x, full_box.tl_u.y, 
			full_box.br_d.x - full_box.tl_d.x, full_box.br_d.y - full_box.tr_u.y);

			renderer->render_set_rotation(0.0f);
		}
	}
}