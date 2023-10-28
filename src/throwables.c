#include "../include/throwables.h"
#include "../include/audio.h"

void clear_throwables() {
	for (int i = 0; i < max_throwables; i++) {
		if (!throwables[i].active) continue;
		throwables[i].alive_time += update_delta;
		if (throwables[i].alive_time >= 3.0f) {
			throwables[i].active = false;
		}
	}
}

void throw_throwable(platform_window* window, u32 id, throwable_type type, float dirx, float diry) {
	for (int i = 0; i < max_throwables; i++) {
		if (throwables[i].active) continue;

		player* p = get_player_by_id(id);
		if (!p) {
			log_info("User with unknown id throwing stuff");
		}

		throwable t = {.active = true, .alive_time = 0.0f, .type = type, .direction = (vec3f){.x = dirx, .y = diry, .z = -0.2f}, 
			.player_id = id, .position = (vec3f){.x = p->playerx, .y = p->playery, .z = p->height}};

		throwables[i] = t;
		break;
	}
}

bool check_if_throwable_collided_with_object(throwable* b, platform_window* window, vec3f oldpos, vec3f newpos, vec3f* direction) {
	map_info info = get_map_info(window);
	float size = get_bullet_size_in_tile(window);

	bool result = false;

	for (int i = 0; i < MAX_OBJECTS; i++) {
		object o = loaded_map.objects[i];
		if (!o.active) continue;
		if (b->position.z <= o.h + o.size.z && b->position.z >= o.h) {
			box obj_box = get_box_of_square((vec3f){o.position.x, o.position.y, o.h}, o.size);

			if (lines_intersect((vec2f){.x = oldpos.x, .y = oldpos.y}, (vec2f){.x = newpos.x, .y = newpos.y}, 
								(vec2f){.x = o.position.x, .y = o.position.y}, (vec2f){.x = o.position.x + o.size.x, .y = o.position.y})) {
					result = true;
			}
			if (lines_intersect((vec2f){.x = oldpos.x, .y = oldpos.y}, (vec2f){.x = newpos.x, .y = newpos.y}, 
								(vec2f){.x = o.position.x, .y = o.position.y + o.size.y}, (vec2f){.x = o.position.x + o.size.x, .y = o.position.y + o.size.y})) {
					result = true;
			}
			if (lines_intersect((vec2f){.x = oldpos.x, .y = oldpos.y}, (vec2f){.x = newpos.x, .y = newpos.y}, 
								(vec2f){.x = o.position.x, .y = o.position.y}, (vec2f){.x = o.position.x, .y = o.position.y + o.size.y})) {
					result = true;
			}
			if (lines_intersect((vec2f){.x = oldpos.x, .y = oldpos.y}, (vec2f){.x = newpos.x, .y = newpos.y}, 
								(vec2f){.x = o.position.x + o.size.x, .y = o.position.y}, (vec2f){.x = o.position.x + o.size.x, .y = o.position.y + o.position.y})) {
					result = true;
			}

			if (result) {
				b->position = oldpos;
				direction->x = -direction->x;
				direction->y = -direction->y;
				return true;
			}
		}
	}

	return result;
}

void update_throwables_server(platform_window* window) {
	float speed = 7.0f * SERVER_TICK_RATE;
	float gravity = 0.015f;

	for (int i = 0; i < max_throwables; i++) {
		throwable b = throwables[i];
		if (!b.active) continue;
		player *p = get_player_by_id(b.player_id);
		if (!p) continue; 

		vec3f oldpos = throwables[i].position;
		
		// move forward
		throwables[i].position.x += throwables[i].direction.x * speed;
		throwables[i].position.y += throwables[i].direction.y * speed;

		// gravity
		if (throwables[i].direction.z != 0) throwables[i].direction.z += gravity;
		throwables[i].position.z -= throwables[i].direction.z;

		// bouncing off floor
		float floor = get_height_of_tile_under_coords(throwables[i].position.x, throwables[i].position.y);
		if (throwables[i].position.z < floor && throwables[i].direction.z != 0) {
			throwables[i].position.z = floor;
			throwables[i].direction.z = -throwables[i].direction.z*0.7;
			throwables[i].bounces++;

			add_audio_event_to_queue(EVENT_BOUNCE_THROWABLE, b.player_id, b.position);

			if (throwables[i].bounces >= 3) throwables[i].direction.z = 0;
		}

		if (check_if_throwable_collided_with_object(&throwables[i], window, oldpos, throwables[i].position, &throwables[i].direction)) {
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

		renderer->render_rectangle(throwable_render_x, throwable_render_y, 10, 10, rgb(255, 51, 51));
	}
}