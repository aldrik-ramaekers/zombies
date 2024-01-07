#include "../include/zombies.h"

static player get_closest_player_to_tile_x(float x, float y, float* buf_length) {
	float best_length = 99999;
	int best_index = -1;

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (!players[i].active) continue;
		if (players[i].interact_state == INTERACT_DEAD) continue;
		float dirx = (players[i].playerx - x);
		float diry = (players[i].playery - y);
		double length = sqrt(dirx * dirx + diry * diry);

		if (length < best_length) {
			best_length = length;
			best_index = i;
		}
	}
	*buf_length = best_length;
	if (best_index == -1) {
		return (player){-1};
	}
	else {
		return players[best_index];
	}
}

static player get_closest_player_to_tile(float x, float y) {
	float best_length = 99999;
	int best_index = -1;

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (!players[i].active) continue;
		if (players[i].interact_state == INTERACT_DEAD) continue;
		float dirx = (players[i].playerx - x);
		float diry = (players[i].playery - y);
		double length = sqrt(dirx * dirx + diry * diry);

		if (length < best_length) {
			best_length = length;
			best_index = i;
		}
	}
	if (best_index == -1) {
		return (player){-1};
	}
	else {
		return players[best_index];
	}
}

void create_spawner(vec2 position) {
	spawner s;
	s.active = true;
	s.position = position;
	s.sec_since_last_spawn = 999.0f;
	s.sprite = create_sprite(img_spawner, 14, 64, 64, 1.0f);

	for (int i = 0; i < MAX_SPAWNERS; i++) {
		spawner o = spawner_tiles[i];
		if (o.active) continue;
		spawner_tiles[i] = s;
		return;
	}
}

bool hit_zombie(int index, int damage) {
	zombies[index].health -= damage;

	// Random chunks when zombie is hit.
	if (rand() % 5 == 0) {
		spawn_zombie_chunk(get_center_of_square(zombies[index].position, zombies[index].size));
	}

	if (zombies[index].health <= 0) {
		vec3f center = get_center_of_square(zombies[index].position, zombies[index].size);
		spawn_zombie_splatter(center);

		// Random chunks when zombie dies.
		int chunk_count = rand() % 4 + 1;
		for (int c = 0; c < chunk_count; c++) spawn_zombie_chunk(center);

		add_zombie_audio_event_to_queue(EVENT_ZOMBIESCREECH, zombies[index].type, zombies[index].position);

		zombies[index].alive = false;
		spawn_drop(zombies[index].position);
		return true;
	}

	return false;
}

u32 number_of_zombies_active()
{
	u32 res = 0;
	for (int i = 0; i < SERVER_MAX_ZOMBIES; i++) {
		zombie o = zombies[i];
		if (!o.alive) continue;
		res++;
	}
	return res;
}

static vec2f get_random_target_for_zombie(zombie o) {
	vec2f target = {0,0};
	try_again:;
	int dist = 10;
	target.x = o.position.x + (rand() % dist) - (dist/2) +1;
	target.y = o.position.y + (rand() % dist) - (dist/2) +1;

	if (target.x <= 0 || target.y <= 0) goto try_again;
	if (target.x >= MAP_SIZE_X-1 || target.y >= MAP_SIZE_Y-1) goto try_again;
	
	object obj = get_object_at_tile(target.x, target.y);
	if (obj.active) goto try_again;
	
	return target;
}

static void set_enraged_zombie_stats(zombie *zombie) {
	zombie->health = 1500.0f;
	zombie->max_health = 1500.0f;
	zombie->size = (vec3f){1.8f, 1.8f, 1.8f};
	zombie->time_since_last_path = 0.0f;
	zombie->request.to_fill = &zombie->next_path;
	zombie->request.mutex = mutex_create();
	zombie->sprite_run = create_sprite(img_alien_run, 6, 32, 32, 0.1f);
	zombie->sprite_run.zoom = 1.20f;
	zombie->speed = 5.0f;
	zombie->attack_range = 0.8f;
	zombie->attack_rate = 0.7f;
	zombie->attack_damage = 250;

	add_zombie_audio_event_to_queue(EVENT_ZOMBIEROAR, ZOMBIE_TYPE_ENRAGED, zombie->position);
}

static void set_normal_zombie_stats(zombie *zombie) {
	zombie->health = 1000.0f;
	zombie->max_health = 1000.0f;
	zombie->size = (vec3f){1.5f, 1.5f, 1.5f};
	zombie->time_since_last_path = 0.0f;
	zombie->request.to_fill = &zombie->next_path;
	zombie->request.mutex = mutex_create();
	zombie->sprite_run = create_sprite(img_alien_run, 6, 32, 32, 0.1f);
	zombie->sprite_run.zoom = 1.20f;
	zombie->speed = 4.0f;
	zombie->attack_range = 0.8f;
	zombie->attack_rate = 0.7f;
	zombie->attack_damage = 100;
}

int normal_zombie_spawn_counter = 0;
void spawn_zombie(int x, int y) {
	normal_zombie_spawn_counter++;

	for (int i = 0; i < SERVER_MAX_ZOMBIES; i++) {
		zombie o = zombies[i];
		if (o.alive) continue;

		zombies[i].path = array_create(sizeof(vec2f));
		zombies[i].next_path = array_create(sizeof(vec2f));
		zombies[i].alive = true;
		zombies[i].type = (normal_zombie_spawn_counter % 5 == 0) ? ZOMBIE_TYPE_ENRAGED : ZOMBIE_TYPE_NORMAL;	
		zombies[i].position = (vec3f){x,y, 0};
		zombies[i].sec_since_last_step = 0.0f;
		zombies[i].sec_since_last_attack = 0.0f;
		switch(zombies[i].type) {
			case ZOMBIE_TYPE_NORMAL: set_normal_zombie_stats(&zombies[i]); break;
			case ZOMBIE_TYPE_ENRAGED: set_enraged_zombie_stats(&zombies[i]); break;
		}

		_current_round.zombies--;
		
		player closest_player = get_closest_player_to_tile(x, y);
		vec2f target = (vec2f){closest_player.playerx, closest_player.playery};
		// All players died, move around randomly
		if (closest_player.id == -1) {
			target = get_random_target_for_zombie(zombies[i]);
		}

		make_pathfinding_request((vec2f){x,y}, target, &zombies[i].next_path, &zombies[i].request);
		break;
	}
}

void update_spawners_server() {
	for (int x = 0; x < MAX_SPAWNERS; x++) {
		spawner spawner = spawner_tiles[x];
		if (!spawner.active) continue;
		update_sprite(&spawner_tiles[x].sprite);
		spawner_tiles[x].sec_since_last_spawn += SERVER_TICK_RATE;
		if (zombies_left_in_current_round() <= 0) continue;
		if (spawner_tiles[x].sec_since_last_spawn >= 1.0f) {
			spawn_zombie(spawner.position.x, spawner.position.y);
			spawner_tiles[x].sec_since_last_spawn = 0;
		}
	}
}

void draw_spawners(platform_window* window) {
	map_info info = get_map_info(window);

	for (int x = 0; x < MAX_SPAWNERS; x++) {
		spawner spawner = spawner_tiles[x];
		if (!spawner.active) continue;
		int render_x = (info.tile_width * spawner.position.x) + (info.px_incline * spawner.position.y);
		int render_y = info.tile_height * spawner.position.y;

		tile tile = loaded_map.heightmap[spawner.position.y][spawner.position.x];

		sprite_frame frame = sprite_get_frame(img_spawner, &spawner.sprite);

		renderer->render_set_rotation(spawner.sec_since_last_spawn*10.0f);
		renderer->render_image_quad_partial(img_spawner, 
			tile.tl.x, tile.tl.y,
			tile.bl.x, tile.bl.y, 
			tile.br.x, tile.br.y, 
			tile.tr.x, tile.tr.y, 
			frame.tl, frame.tr, frame.bl, frame.br);

		renderer->render_set_rotation(0.0f);

		/*
		renderer->render_quad(
				tile.tl.x, tile.tl.y, 
				tile.bl.x, tile.bl.y, 
				tile.br.x, tile.br.y, 
				tile.tr.x, tile.tr.y, 
				rgb(100, 150, 50));	

		renderer->render_line(tile.tl.x, tile.tl.y, tile.tr.x, tile.tr.y, 1, rgb(0,255,255)); // top
		renderer->render_line(tile.tl.x, tile.tl.y, tile.bl.x, tile.bl.y, 1, rgb(0,255,255)); // left
		renderer->render_line(tile.tr.x, tile.tr.y, tile.br.x, tile.br.y, 1, rgb(0,255,255)); // right
		renderer->render_line(tile.bl.x, tile.bl.y, tile.br.x, tile.br.y, 1, rgb(0,255,255)); // bottom*/
	}
}

static void draw_path_of_zombie(platform_window* window, zombie o) {
	map_info info = get_map_info(window);
	vec2f prev_pos = (vec2f){o.position.x, o.position.y};

	for (int i = 0; i < o.path.length; i++) {
		vec2f* pos = array_at(&o.path, o.path.length-1-i);

		int render_x = (info.tile_width * prev_pos.x) + (info.px_incline * prev_pos.y);
		int render_y = info.tile_height * prev_pos.y;

		int render_x2 = (info.tile_width * pos->x) + (info.px_incline * pos->y);
		int render_y2 = info.tile_height * pos->y;

		prev_pos = *pos;
		if (i == 0) {
			renderer->render_line(render_x, render_y, render_x2, render_y2, 2, rgb(0,255,255));
			continue;
		}
	
		renderer->render_line(render_x, render_y, render_x2, render_y2, 2, rgb(0,0,255));
	}
}

static vec2f get_direction_to_tile(zombie o, vec2f dest) {
	float dirx = (dest.x - o.position.x);
	float diry = (dest.y - o.position.y);
	if (dirx == 0 && diry == 0) return (vec2f){0,0};
	double length = sqrt(dirx * dirx + diry * diry);
	dirx /= length;
	diry /= length;
	return (vec2f){dirx, diry};
}

static vec2f get_direction_to_next_tile(zombie o) {
	vec2f dest = (vec2f){o.position.x, o.position.y};
	if (o.path.length > 0) {
		dest = *(vec2f*)array_at(&o.path, o.path.length-1);
	}

	return get_direction_to_tile(o, dest);
}

static bool is_within_tile(zombie o, vec2f dest) {
	if (fabs(o.position.x - dest.x) < 0.05f && fabs(o.position.y - dest.y) < 0.05f) {
		return true;
	}
	return false;
}

static bool is_within_next_tile(zombie o) {
	if (o.path.length > 0) {
		vec2f dest = *(vec2f*)array_at(&o.path, o.path.length-1);
		return is_within_tile(o, dest);
	}
	return false;
}

void update_zombies_client(platform_window* window) {
	float speed = 4.0f * update_delta;
	for (int i = 0; i < SERVER_MAX_ZOMBIES; i++) {
		zombie o = zombies[i];
		if (!o.alive) continue;
		if (o.next2tiles[0].x == -1 || o.next2tiles[0].y == -1) continue; // ran out of stored path.

		float height = get_height_of_tile_under_coords(zombies[i].position.x, zombies[i].position.y);

		vec2f dir = get_direction_to_tile(o, o.next2tiles[0]);
		zombies[i].position.x += dir.x*speed;
		zombies[i].position.y += dir.y*speed;
		zombies[i].position.z = height;

		if (is_within_tile(zombies[i], o.next2tiles[0])) {
			o.next2tiles[0] = o.next2tiles[1];
			o.next2tiles[1] = (vec2f){-1,-1};
		}
	}
}

static vec2f get_random_point_around_player(player p, zombie o) {
	 // Convert from degrees to radians via multiplication by PI/180     

	try_again:;
	float radius = 0.8f, angleInDegrees = (rand() % 360);
	vec2f origin = (vec2f){o.position.x, o.position.y}; 
	float x = (float)(radius * cos(angleInDegrees * M_PI / 180.0f)) + p.playerx;
	float y = (float)(radius * sin(angleInDegrees * M_PI / 180.0f)) + p.playery;

	object obj = get_object_at_tile(x, y);
	if (obj.active) goto try_again;
	
	return (vec2f){x, y};
}

static void update_zombie_attacks_server(zombie *zombie) {
	if (zombie->sec_since_last_attack >= zombie->attack_rate) {
		//add_zombie_audio_event_to_queue(EVENT_FOOTSTEP, o.type, o.position);
		
		float dist;
		player p = get_closest_player_to_tile_x(zombie->position.x, zombie->position.y, &dist);
		if (p.id != -1 && dist <= zombie->attack_range) {
			zombie->sec_since_last_attack = 0.0f;
			hurt_player(p.id, zombie->attack_damage);
		}
	}
}

void update_zombies_server(platform_window* window) {
	for (int i = 0; i < SERVER_MAX_ZOMBIES; i++) {
		zombie o = zombies[i];
		if (!o.alive) continue;

		update_sprite(&zombies[i].sprite_run);

		if (zombies[i].type == ZOMBIE_TYPE_ENRAGED && zombies[i].sec_since_last_step > 0.2f) {
			add_zombie_audio_event_to_queue(EVENT_FOOTSTEP, o.type, o.position);
			zombies[i].sec_since_last_step = 0.0f;
		}
		zombies[i].sec_since_last_step += SERVER_TICK_RATE;
		zombies[i].sec_since_last_attack += SERVER_TICK_RATE;

		// Update pathfinding
		zombies[i].time_since_last_path += SERVER_TICK_RATE;
		if (zombies[i].time_since_last_path > SERVER_PATHFINDING_INTERVAL) {
			player closest_player = get_closest_player_to_tile(o.position.x, o.position.y);
			vec2f target_tile = (vec2f){closest_player.playerx, closest_player.playery+(get_player_size_in_tile()/2)};
			// All players died, move around randomly
			if (closest_player.id == -1) {
				target_tile = get_random_target_for_zombie(o);
			}		

			array_clear(zombies[i].request.to_fill);
			make_pathfinding_request((vec2f){o.position.x,o.position.y}, target_tile, &zombies[i].next_path, &zombies[i].request);
			zombies[i].time_since_last_path = 0;
		}
		else {
			if (zombies[i].request.active)
			{
				if (zombies[i].request.to_fill->length) {
					mutex_lock(&zombies[i].request.mutex);

					array_destroy(&zombies[i].path);
					zombies[i].path = array_copy(zombies[i].request.to_fill);
					
					player closest_player = get_closest_player_to_tile(o.position.x, o.position.y);

					// All players died, move around randomly
					if (closest_player.id == -1) {
						vec2f target = get_random_target_for_zombie(o);
						closest_player.playerx = target.x;
						closest_player.playery = target.y;
					}
					vec2f final_pos = get_random_point_around_player(closest_player, zombies[i]);
					array_push_at(&zombies[i].path, (u8*)&final_pos, 0);

					zombies[i].request.active = false;

					mutex_unlock(&zombies[i].request.mutex);
				}
			}
		}

		if (is_within_next_tile(zombies[i])) {
			array_remove_at(&zombies[i].path, zombies[i].path.length-1);
		}

		// Update position
		{
			float speed = zombies[i].speed * SERVER_TICK_RATE;
			vec2f dir = get_direction_to_next_tile(o);
			float height = get_height_of_tile_under_coords(zombies[i].position.x, zombies[i].position.y);
			zombies[i].position.x += dir.x*speed;
			zombies[i].position.y += dir.y*speed;
			zombies[i].position.z = height;
			zombies[i].dir = dir;

			zombies[i].next2tiles[0] = (vec2f){-1,-1};
			zombies[i].next2tiles[1] = (vec2f){-1,-1};
			if (o.path.length > 0) {
				zombies[i].next2tiles[0] = *(vec2f*)array_at(&o.path, o.path.length-1);
				zombies[i].next2tiles[1] = zombies[i].next2tiles[0];
				if (o.path.length > 1) {
					zombies[i].next2tiles[1] = *(vec2f*)array_at(&o.path, o.path.length-2);
				}
			}
		}

		// Attacking
		update_zombie_attacks_server(&zombies[i]);
	}
}

void draw_zombies(platform_window* window) {
	map_info info = get_map_info(window);

	for (int i = 0; i < SERVER_MAX_ZOMBIES; i++) {
		zombie o = zombies[i];
		if (!o.alive) continue;

		sprite_frame frame = sprite_get_frame(img_alien_run, &o.sprite_run);
		box box = get_render_box_of_square(window, (vec3f){o.position.x, o.position.y, o.position.z}, o.size);
		vec2f zombie_pos = world_pos_to_screen_pos(window, o.position.x, o.position.y, o.position.z);
		float zombie_size = o.size.x * get_tile_width(window);

		float rads = -atan2(o.dir.x, o.dir.y);
		if (rads >= 0.0f) {
			frame = sprite_swap_frame_horizontally(frame);
		}

		renderer->render_image_quad_partial(img_alien_run, 
			zombie_pos.x, zombie_pos.y,
			zombie_pos.x, zombie_pos.y + zombie_size,
			zombie_pos.x + zombie_size, zombie_pos.y + zombie_size,
			zombie_pos.x + zombie_size, zombie_pos.y,
			frame.tl, frame.tr, frame.bl, frame.br);

		if (o.type == ZOMBIE_TYPE_ENRAGED) {
			renderer->render_image_quad_partial_tint(img_alien_run, 
				zombie_pos.x, zombie_pos.y,
				zombie_pos.x, zombie_pos.y + zombie_size,
				zombie_pos.x + zombie_size, zombie_pos.y + zombie_size,
				zombie_pos.x + zombie_size, zombie_pos.y,
				frame.tl, frame.tr, frame.bl, frame.br, rgba(255,0,0,150));
		}

		// health bar
		if (o.health < o.max_health) {
			int bar_h = zombie_size/8;
			int bar_w = zombie_size/2;
			float percentage = o.health/(float)o.max_health;
			renderer->render_rectangle(zombie_pos.x + (zombie_size/2) - (bar_w/2), zombie_pos.y - bar_h, bar_w, bar_h, rgb(0,0,0));
			renderer->render_rectangle(zombie_pos.x + (zombie_size/2) - (bar_w/2), zombie_pos.y - bar_h, bar_w*percentage, bar_h, rgb(100,0,0));
		}

		//if (global_state.server) draw_path_of_zombie(window, o);
	}
}

