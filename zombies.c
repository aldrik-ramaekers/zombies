#include "include/zombies.h"

void spawn_zombie(int x, int y) {
	return;

	for (int i = 0; i < max_zombies; i++) {
		zombie o = zombies[i];
		if (o.alive) continue;

		zombies[i].alive = true;
		zombies[i].health = 100.0f;
		zombies[i].position = (vec3f){x,y, 0};
		zombies[i].size = (vec3f){0.4, 0.4, 1};
		break;
	}
}

void draw_spawners(platform_window* window) {
	map_info info = get_map_info(window);

	for (int x = 0; x < 2; x++) {
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
		if (spawner_tiles[x].sec_since_last_spawn >= 1.0f) {
			spawn_zombie(spawner.position.x, spawner.position.y);
			spawner_tiles[x].sec_since_last_spawn = 0;
		}

		renderer->render_line(tile.tl.x, tile.tl.y, tile.tr.x, tile.tr.y, 1, rgb(0,255,255)); // top
		renderer->render_line(tile.tl.x, tile.tl.y, tile.bl.x, tile.bl.y, 1, rgb(0,255,255)); // left
		renderer->render_line(tile.tr.x, tile.tr.y, tile.br.x, tile.br.y, 1, rgb(0,255,255)); // right
		renderer->render_line(tile.bl.x, tile.bl.y, tile.br.x, tile.br.y, 1, rgb(0,255,255)); // bottom
	}
}

void draw_zombies_at_tile(platform_window* window, int x, int y) {
	map_info info = get_map_info(window);
	float speed = 0.05f;

	for (int i = 0; i < max_zombies; i++) {
		zombie o = zombies[i];
		if (!o.alive) continue;
		if ((int)o.position.x != x || (int)ceil(o.position.y) != y) continue;

		float dirx = (playerx - o.position.x);
		float diry = (playery - o.position.y);
		double length = sqrt(dirx * dirx + diry * diry);
		dirx /= length;
		diry /= length;

		float height = get_height_of_tile_under_coords(window, zombies[i].position.x, zombies[i].position.y);
		zombies[i].position.x += dirx*speed;
		zombies[i].position.y += diry*speed;
		zombies[i].position.z = height;

		box box = get_render_box_of_square(window, (vec3f){o.position.x, o.position.y, height}, o.size);
		render_quad_with_outline(box.tl_b, box.tr_b, box.bl_b, box.br_b);
		render_quad_with_outline(box.tl_u, box.tr_u, box.bl_u, box.br_u);
		render_quad_with_outline(box.tl_u, box.tl_b, box.bl_u, box.bl_b);
		render_quad_with_outline(box.bl_u, box.br_u, box.bl_b, box.br_b);
	}
}

