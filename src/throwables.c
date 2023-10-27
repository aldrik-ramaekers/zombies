#include "../include/throwables.h"

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


void update_throwables_server(platform_window* window) {
	float speed = 7.0f * SERVER_TICK_RATE;
	float gravity = 0.015f;

	float max_audible_throwable_dist = 10.0f;

	for (int i = 0; i < max_throwables; i++) {
		throwable b = throwables[i];
		if (!b.active) continue;
		player *p = get_player_by_id(b.player_id);
		if (!p) continue; 
		
		throwables[i].position.x += throwables[i].direction.x * speed;
		throwables[i].position.y += throwables[i].direction.y * speed;

		if (throwables[i].direction.z != 0) throwables[i].direction.z += gravity;
		throwables[i].position.z -= throwables[i].direction.z;

		float floor = get_height_of_tile_under_coords(throwables[i].position.x, throwables[i].position.y);
		if (throwables[i].position.z < floor && throwables[i].direction.z != 0) {
			throwables[i].position.z = floor;
			throwables[i].direction.z = -throwables[i].direction.z*0.7;

			// calculate volume
			int tiles_between_throwable_and_player = distance_between_3f((vec3f){.x = p->playerx, .y = p->playery, .z = p->height}, b.position);
			float volume = (tiles_between_throwable_and_player / max_audible_throwable_dist);
			if (volume > 1.0f) volume = 1.0f;

			// calculate angle
			float dirx = (throwables[i].position.x - p->playerx);
			float diry = (throwables[i].position.y - p->playery);
			float rads = atan2(diry, dirx) * 180.0f/M_PI;
			if (rads < 0) rads = 360 + rads;
			rads += 90;
			if (rads > 360) rads -= 360;

			log_infox("rads: %.2f", rads)
			
			Mix_SetPosition(0, rads, volume*255);
			Mix_PlayChannel(0, wav_throwable_bounce, 0);
			if (throwables[i].direction.z > -0.03f && throwables[i].direction.z < 0.03f) throwables[i].direction.z = 0;
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