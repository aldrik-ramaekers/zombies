#include "../include/throwables.h"
#include "../include/audio.h"


static image* get_throwable_explosion_from_type(throwable_type type) {
	switch(type) {
		case THROWABLE_GRENADE: return img_grenade_explode;
		case THROWABLE_MOLOTOV: return img_molotov_explode;
		default: return img_grenade_explode;
	}
}

static float get_throwable_explosion_time(throwable_type type) {
	switch(type) {
		case THROWABLE_GRENADE: return 3.2f;
		case THROWABLE_MOLOTOV: return 8.0f;
		default: return 0.0f;
	}
}

static float get_throwable_fly_time(throwable_type type) {
	switch(type) {
		case THROWABLE_GRENADE: return 2.0f;
		case THROWABLE_MOLOTOV: return 0.6f;
		default: return 0.0f;
	}
}

static vec3f get_throwable_explosionsize(throwable_type type) {
	switch(type) {
		case THROWABLE_GRENADE: return (vec3f){2.0f, 2.0f, 2.0f};
		case THROWABLE_MOLOTOV: return (vec3f){4.0f, 4.0f, 4.0f};
		default: return (vec3f){1.0f, 1.0f, 1.0f};;
	}
}

void throw_throwable(u32 id, throwable_type type, float dirx, float diry) {
	for (int i = 0; i < max_throwables; i++) {
		if (throwables[i].active) continue;

		player* p = get_player_by_id(id);
		if (!p) {
			log_info("User with unknown id throwing stuff");
		}

		throwable t = {.active = true, .state = THROWABLE_FLYING, .alive_time = 0.0f, .type = type, .direction = (vec3f){.x = 0.0f, .y = 0.0f, .z = 0.0f}, 
			.player_id = id, .position = (vec3f){.x = p->playerx, .y = p->playery, .z = p->height}};

		switch(type) {
			case THROWABLE_GRENADE: {
				p->throwables.grenades--;
				t.sprite = create_sprite(img_grenade_explode, 12, 96, 96, 0.1f);
				t.damage = 1500; 
				t.direction = (vec3f){.x = dirx*1.5f, .y = diry*1.5f, .z = -0.2f};
			} break;
			case THROWABLE_MOLOTOV: {
				p->throwables.molotovs--;
				t.sprite = create_sprite(img_molotov_explode, 32, 66, 119, 0.04f);
				t.damage = 300; 
				t.direction = (vec3f){.x = dirx*2.5f, .y = diry*2.5f, .z = -0.3f};
			} break;
			
		}

		throwables[i] = t;
		break;
	}
}

bool check_if_throwable_collided_with_object(throwable* b, vec3f oldpos, vec3f newpos, vec3f* direction) {
	bool result = false;

	for (int i = 0; i < MAX_OBJECTS; i++) {
		object o = loaded_map->objects[i];
		if (!o.active) continue;
		if (b->position.z <= o.position.z + o.size.z && b->position.z >= o.position.z) {
			box obj_box = get_box_of_square((vec3f){o.position.x, o.position.y, o.position.z}, o.size);

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
			
			vec3f grenade_center = get_center_of_square(t.position, get_throwable_explosionsize(t.type));
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

void explode_molotov(throwable b) {
	add_throwable_audio_event_to_queue(EVENT_FIRE, b.type, b.player_id, b.position);
}

void update_molotov(throwable* b) {
	if (b->sec_since_last_tick <= 0.3f) {
		return;
	}
	b->sec_since_last_tick = 0.0f;

	for (int i = 0; i < SERVER_MAX_ZOMBIES; i++) {
		zombie o = zombies[i];
		if (!o.alive) continue;

		vec3f size = get_throwable_explosionsize(b->type);
		float max_damage_range = size.x/2.0f;
		vec3f grenade_center = get_center_of_square(b->position, size);
		vec3f zombie_center = get_center_of_square(o.position, o.size);
		float dist_between_grenade_and_zombie = distance_between_3f(o.position, b->position);
		if (dist_between_grenade_and_zombie > max_damage_range) continue;

		if (hit_zombie(i, b->damage)) {
			player* p = get_player_by_id(b->player_id);
			if (p) p->kills++;
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
		throwables[i].sec_since_last_tick += SERVER_TICK_RATE;

		if (throwables[i].alive_time >= get_throwable_explosion_time(b.type)) {
			throwables[i].active = false;
			continue;
		}

		if (throwables[i].alive_time >= get_throwable_fly_time(b.type)) {
			if (throwables[i].state == THROWABLE_FLYING) {
				add_throwable_audio_event_to_queue(EVENT_EXPLODE_THROWABLE, b.type, b.player_id, b.position);

				switch(b.type) {
					case THROWABLE_GRENADE: explode_grenade(b); break;
					case THROWABLE_MOLOTOV: explode_molotov(b); break;
				}
			}
			else {
				// Update explosion.

				switch(b.type) {
					case THROWABLE_GRENADE: break;
					case THROWABLE_MOLOTOV: update_molotov(&throwables[i]); break;
				}
			}

			throwables[i].state = THROWABLE_EXPLODED;
			update_sprite(&throwables[i].sprite);
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

		float throwable_render_x = t.position.x*info.tile_width + (t.position.y*info.px_incline);
		float throwable_render_y = t.position.y*info.tile_height - (t.position.z*info.px_raised_per_h);

		if (t.state == THROWABLE_EXPLODED) {
			vec3f explode_location = t.position;
			vec3f explosion_size = get_throwable_explosionsize(t.type);
			explode_location.x -= explosion_size.x/2;
			explode_location.y -= explosion_size.y/2;
			box box = get_render_box_of_square(window, explode_location, explosion_size);
			
			image* img = get_throwable_explosion_from_type(t.type);
			sprite_frame frame = sprite_get_frame(img, &throwables[i].sprite);

			renderer->render_image_quad_partial(img, 
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