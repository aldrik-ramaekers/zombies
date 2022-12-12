#include "../include/zombies.h"

static player get_closest_player_to_tile(int x, int y) {
	float best_length = 99999;
	int best_index = -1;

	for (int i = 0; i < max_players; i++) {
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

void spawn_zombie(int x, int y) {
	for (int i = 0; i < max_zombies; i++) {
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

		make_pathfinding_request((vec2f){x,y}, (vec2f){closest_player.playerx, closest_player.playery}, &zombies[i].path, &zombies[i].request);
		break;
	}
}

void draw_spawners(platform_window* window) {
	map_info info = get_map_info(window);

	for (int x = 0; x < 1; x++) {
		spawner spawner = spawner_tiles[x];
		int render_x = (info.tile_width * spawner.position.x) + (info.px_incline * spawner.position.y);
		int render_y = info.tile_height * spawner.position.y;

		tile tile = map_loaded[spawner.position.y][spawner.position.x];
		renderer->render_quad(
				tile.tl.x, tile.tl.y, 
				tile.bl.x, tile.bl.y, 
				tile.br.x, tile.br.y, 
				tile.tr.x, tile.tr.y, 
				rgb(100, 150, 50));	

		spawner_tiles[x].sec_since_last_spawn += update_delta;
		if (spawner_tiles[x].sec_since_last_spawn >= 2.0f) {
			spawn_zombie(spawner.position.x, spawner.position.y);
			spawner_tiles[x].sec_since_last_spawn = 0;
		}

		renderer->render_line(tile.tl.x, tile.tl.y, tile.tr.x, tile.tr.y, 1, rgb(0,255,255)); // top
		renderer->render_line(tile.tl.x, tile.tl.y, tile.bl.x, tile.bl.y, 1, rgb(0,255,255)); // left
		renderer->render_line(tile.tr.x, tile.tr.y, tile.br.x, tile.br.y, 1, rgb(0,255,255)); // right
		renderer->render_line(tile.bl.x, tile.bl.y, tile.br.x, tile.br.y, 1, rgb(0,255,255)); // bottom
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

static vec2f get_direction_to_next_tile(zombie o) {
	vec2f dest = (vec2f){o.position.x, o.position.y};
	if (o.path.length > 0) {
		dest = *(vec2f*)array_at(&o.path, o.path.length-1);
	}

	float dirx = (dest.x - o.position.x);
	float diry = (dest.y - o.position.y);
	if (dirx == 0 && diry == 0) return (vec2f){0,0};
	double length = sqrt(dirx * dirx + diry * diry);
	dirx /= length;
	diry /= length;
	return (vec2f){dirx, diry};
}

static bool is_within_next_tile(zombie o) {
	if (o.path.length > 0) {
		vec2f dest = *(vec2f*)array_at(&o.path, o.path.length-1);
		if (fabs(o.position.x - dest.x) < 0.05f && fabs(o.position.y - dest.y) < 0.05f) {
			return true;
		}
	}

	return false;
}

void draw_zombies_at_tile(platform_window* window, int x, int y) {
	map_info info = get_map_info(window);
	float speed = 0.05f;

	for (int i = 0; i < max_zombies; i++) {
		zombie o = zombies[i];
		if (!o.alive) continue;

		if ((int)o.position.x != x || (int)ceil(o.position.y) != y) continue;

		zombies[i].time_since_last_path += update_delta;
		if (zombies[i].time_since_last_path > 0.05f) {
			player closest_player = get_closest_player_to_tile(x, y);
			make_pathfinding_request((vec2f){o.position.x,o.position.y}, 
				(vec2f){closest_player.playerx, closest_player.playery + get_player_size_in_tile()}, 
				&zombies[i].next_path, &zombies[i].request);
			zombies[i].time_since_last_path = 0;
		}
		else {
			if (mutex_trylock(&zombies[i].request.mutex))
			{
				if (zombies[i].request.to_fill->length) {
					array_destroy(&zombies[i].path);
					zombies[i].path = array_copy(&zombies[i].next_path);
					array_clear(&zombies[i].next_path);
				}
			}
			mutex_unlock(&zombies[i].request.mutex);
		}

		vec2f dir = get_direction_to_next_tile(o);

		float height = get_height_of_tile_under_coords(window, zombies[i].position.x, zombies[i].position.y);
		zombies[i].position.x += dir.x*speed;
		zombies[i].position.y += dir.y*speed;
		zombies[i].position.z = height;

		if (is_within_next_tile(zombies[i])) {
			array_remove_at(&zombies[i].path, zombies[i].path.length-1);
		}

		box box = get_render_box_of_square(window, (vec3f){o.position.x, o.position.y, height}, o.size);
		render_quad_with_outline(box.tl_b, box.tr_b, box.bl_b, box.br_b);
		render_quad_with_outline(box.tl_u, box.tr_u, box.bl_u, box.br_u);
		render_quad_with_outline(box.tl_u, box.tl_b, box.bl_u, box.bl_b);
		render_quad_with_outline(box.bl_u, box.br_u, box.bl_b, box.br_b);

		draw_path_of_zombie(window, o);
	}
}

