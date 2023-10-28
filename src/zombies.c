#include "../include/zombies.h"

static player get_closest_player_to_tile(int x, int y) {
	float best_length = 99999;
	int best_index = -1;

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (!players[i].active) continue;
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
	s.sprite = create_sprite(img_spawner, 14, 64, 64, 0.1f);

	for (int i = 0; i < MAX_SPAWNERS; i++) {
		spawner o = spawner_tiles[i];
		if (o.active) continue;
		spawner_tiles[i] = s;
		return;
	}
}

void spawn_zombie(int x, int y) {
	for (int i = 0; i < SERVER_MAX_ZOMBIES; i++) {
		zombie o = zombies[i];
		if (o.alive) continue;

		zombies[i].path = array_create(sizeof(vec2f));
		zombies[i].next_path = array_create(sizeof(vec2f));
		zombies[i].alive = true;
		zombies[i].health = 100.0f;
		zombies[i].position = (vec3f){x,y, 0};
		zombies[i].size = (vec3f){0.4, 0.4, 1};
		zombies[i].time_since_last_path = 0.0f;
		zombies[i].request.to_fill = &zombies[i].next_path;
		zombies[i].request.mutex = mutex_create();
		
		player closest_player = get_closest_player_to_tile(x, y);

		make_pathfinding_request((vec2f){x,y}, (vec2f){closest_player.playerx, closest_player.playery}, &zombies[i].next_path, &zombies[i].request);
		break;
	}
}

void update_spawners_server() {
	for (int x = 0; x < MAX_SPAWNERS; x++) {
		spawner spawner = spawner_tiles[x];
		if (!spawner.active) continue;
		update_sprite(&spawner_tiles[x].sprite);
		spawner_tiles[x].sec_since_last_spawn += SERVER_TICK_RATE;
		if (spawner_tiles[x].sec_since_last_spawn >= 2.0f) {
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

		sprite_frame frame = sprite_get_frame(&spawner.sprite);
		renderer->render_image_quad_partial(img_spawner, 
			tile.tl.x, tile.tl.y,
			tile.bl.x, tile.bl.y, 
			tile.br.x, tile.br.y, 
			tile.tr.x, tile.tr.y, 
			frame.tl, frame.tr, frame.bl, frame.br);

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

void update_zombies_server(platform_window* window) {
	float speed = 4.0f * SERVER_TICK_RATE;

	for (int i = 0; i < SERVER_MAX_ZOMBIES; i++) {
		zombie o = zombies[i];
		if (!o.alive) continue;

		zombies[i].time_since_last_path += SERVER_TICK_RATE;
		if (zombies[i].time_since_last_path > SERVER_PATHFINDING_INTERVAL) {
			player closest_player = get_closest_player_to_tile((int)o.position.x, (int)o.position.y);
			vec2f target_tile = (vec2f){closest_player.playerx, closest_player.playery+(get_player_size_in_tile()/2)};

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
					
					player closest_player = get_closest_player_to_tile((int)o.position.x, (int)o.position.y);
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

		vec2f dir = get_direction_to_next_tile(o);
		float height = get_height_of_tile_under_coords(zombies[i].position.x, zombies[i].position.y);
		zombies[i].position.x += dir.x*speed;
		zombies[i].position.y += dir.y*speed;
		zombies[i].position.z = height;

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
}

void draw_zombies(platform_window* window) {
	map_info info = get_map_info(window);

	for (int i = 0; i < SERVER_MAX_ZOMBIES; i++) {
		zombie o = zombies[i];
		if (!o.alive) continue;

		OBJECT_RENDER_DEPTH((int)o.position.y);

		box box = get_render_box_of_square(window, (vec3f){o.position.x, o.position.y, o.position.z}, o.size);
		render_quad_with_outline(box.tl_d, box.tr_d, box.bl_d, box.br_d, rgb(200,200,0));
		render_quad_with_outline(box.tl_u, box.tr_u, box.bl_u, box.br_u, rgb(200,200,0));
		render_quad_with_outline(box.tl_u, box.tl_d, box.bl_u, box.bl_d, rgb(200,200,0));
		render_quad_with_outline(box.bl_u, box.br_u, box.bl_d, box.br_d, rgb(200,200,0));

		//if (global_state.server) draw_path_of_zombie(window, o);
	}
}

