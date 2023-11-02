#include "../include/zombie_chunk.h"
#include "../include/math_helper.h"

static vec2f get_random_point_around_point(vec3f center, float distance) {
	float angleInDegrees = (rand() % 360);
	vec2f origin = (vec2f){center.x, center.y}; 
	float x = (float)(distance * cos(angleInDegrees * M_PI / 180.0f)) + center.x;
	float y = (float)(distance * sin(angleInDegrees * M_PI / 180.0f)) + center.y;
	
	return (vec2f){x, y};
}

static image* get_chunk_image_from_type(zombie_chunk_type type) {
	switch(type) {
		default:
		case CHUNK_HAND: return img_zombie_chunk_hand;
		case CHUNK_FOOT: return img_zombie_chunk_foot;
		case CHUNK_SPLATTER: return img_zombie_chunk_blood;
	}
}

static zombie_chunk_type get_random_chunk_type() {
	#define AVAILABLE_ZOMBIE_CHUNK_COUNT 2
	zombie_chunk_type available_zombie_chunks[AVAILABLE_ZOMBIE_CHUNK_COUNT] = {
		CHUNK_HAND,
		CHUNK_FOOT,
	};
	return available_zombie_chunks[rand() % AVAILABLE_ZOMBIE_CHUNK_COUNT];
}

void spawn_zombie_splatter(vec3f center) {
	for (int i = 0; i < MAX_ZOMBIE_CHUNKS; i++)
	{
		if (zombie_chunks[i].active) continue;

		float height = get_height_of_tile_under_coords(center.x, center.y);
		center.z = height;
		vec2f dir = (vec2f){0,0};
		
		zombie_chunk chunk = {.active = true, .start_position = center, .direction = dir, 
			.position = center, .duration = 0.0f, .target_position = center,
			.type = CHUNK_SPLATTER, .rotation = (float)rand()/(float)(RAND_MAX), .rotation_speed = 0.0f};
		zombie_chunks[i] = chunk;
		return;
	}
}

void spawn_zombie_chunk(vec3f center) {
	for (int i = 0; i < MAX_ZOMBIE_CHUNKS; i++)
	{
		if (zombie_chunks[i].active) continue;

		vec2f target = get_random_point_around_point(center, (float)rand()/(float)(RAND_MAX));
		vec2f dir = get_dir_of_line((vec2f){center.x, center.y}, (vec2f){target.x, target.y});
		float height = get_height_of_tile_under_coords(target.x, target.y);
		
		zombie_chunk chunk = {.active = true, .start_position = center, .direction = dir, 
			.position = center, .duration = 0.0f, .target_position = (vec3f){target.x, target.y, height},
			.type = get_random_chunk_type(), .rotation = (float)rand()/(float)(RAND_MAX), .rotation_speed = dir.x < 0.0f ? update_delta : -update_delta};
		zombie_chunks[i] = chunk;
		return;
	}
}

void update_zombie_chunks() {
	for (int i = 0; i < MAX_ZOMBIE_CHUNKS; i++)
	{
		if (!zombie_chunks[i].active) continue;

		zombie_chunks[i].duration += update_delta;
		if (zombie_chunks[i].duration >= CHUNK_MAX_DURATION) {
			zombie_chunks[i].active = false;
			continue;
		}
		if (zombie_chunks[i].duration >= CHUNK_DURATION_OF_DROP) {
			continue;
		}

		
		zombie_chunks[i].rotation += zombie_chunks[i].rotation_speed;

		float x = zombie_chunks[i].start_position.x + ((zombie_chunks[i].start_position.x - zombie_chunks[i].target_position.x) / CHUNK_DURATION_OF_DROP) * zombie_chunks[i].duration;
		float y = zombie_chunks[i].start_position.y + ((zombie_chunks[i].start_position.y - zombie_chunks[i].target_position.y) / CHUNK_DURATION_OF_DROP) * zombie_chunks[i].duration;
		float z = zombie_chunks[i].start_position.z + ((zombie_chunks[i].start_position.z - zombie_chunks[i].target_position.z) / CHUNK_DURATION_OF_DROP) * zombie_chunks[i].duration;

		zombie_chunks[i].position.x = x;
		zombie_chunks[i].position.y = y;
		zombie_chunks[i].position.z = z;
	}
}

void draw_zombie_chunks(platform_window* window) {
	for (int i = 0; i < MAX_ZOMBIE_CHUNKS; i++)
	{
		if (!zombie_chunks[i].active) continue;

		DROP_RENDER_DEPTH((int)(zombie_chunks[i].position.y));
		
		int alpha = 255;
		if (zombie_chunks[i].duration >= CHUNK_MAX_DURATION - CHUNK_FADE_TIME) {
			alpha = 255 - ((zombie_chunks[i].duration - (CHUNK_MAX_DURATION - CHUNK_FADE_TIME)) / CHUNK_FADE_TIME)*255;
		}
		if (alpha < 0) alpha = 0;

		box box = get_render_box_of_square(window, zombie_chunks[i].position, (vec3f){0.6f, 0.6f, 0.6f});
		//render_box_with_outline(box, rgba(200, 50, 50, alpha));

		renderer->render_set_rotation(zombie_chunks[i].rotation);
		renderer->render_image_tint(get_chunk_image_from_type(zombie_chunks[i].type), box.tl_d.x, box.tl_d.y, box.tr_d.x - box.tl_d.x, box.br_d.y - box.tr_d.y, rgba(255,255,255,alpha));
		renderer->render_set_rotation(0.0f);
	}
}