#include "include/zombies.h"

void spawn_zombie(int x, int y) {
	for (int i = 0; i < max_zombies; i++) {
		zombie o = zombies[i];
		if (o.alive) continue;

		zombies[i].alive = true;
		zombies[i].health = 100.0f;
		zombies[i].position = (vec2f){x,y};
		break;
	}
}

void draw_spawners(platform_window* window) {
	map_info info = get_map_info(window);

	for (int x = 0; x < 2; x++) {
		vec2 spawner_location = spawner_tiles[x];
		int render_x = (info.tile_width * spawner_location.x) + (info.px_incline * spawner_location.y);
		int render_y = info.tile_height * spawner_location.y;

		tile tile = map_loaded[spawner_location.y][spawner_location.x];
		renderer->render_quad(
				tile.tl.x, tile.tl.y, 
				tile.bl.x, tile.bl.y, 
				tile.br.x, tile.br.y, 
				tile.tr.x, tile.tr.y, 
				rgb(100, 150, 50));

		renderer->render_line(tile.tl.x, tile.tl.y, tile.tr.x, tile.tr.y, 1, rgb(0,255,255)); // top
		renderer->render_line(tile.tl.x, tile.tl.y, tile.bl.x, tile.bl.y, 1, rgb(0,255,255)); // left
		renderer->render_line(tile.tr.x, tile.tr.y, tile.br.x, tile.br.y, 1, rgb(0,255,255)); // right
		renderer->render_line(tile.bl.x, tile.bl.y, tile.br.x, tile.br.y, 1, rgb(0,255,255)); // bottom
	}
	
}

void draw_zombies_at_tile(platform_window* window, int x, int y) {
	map_info info = get_map_info(window);
	float zombie_size = get_player_size(window);
	float speed = 0.05f;

	for (int i = 0; i < max_zombies; i++) {
		zombie o = zombies[i];
		if (!o.alive) continue;
		if ((int)o.position.x != x || (int)o.position.y != y) continue;

		float dirx = (playerx - o.position.x);
		float diry = (playery - o.position.y);
		double length = sqrt(dirx * dirx + diry * diry);
		dirx /= length;
		diry /= length;

		zombies[i].position.x += dirx*speed;
		zombies[i].position.y += diry*speed;

		float height = get_height_of_tile_under_coords(window, zombies[i].position.x, zombies[i].position.y);
		int render_x = (info.tile_width * o.position.x) + (info.px_incline * o.position.y);
		int render_y = info.tile_height * o.position.y  - (height*info.px_raised_per_h);

		renderer->render_rectangle(render_x, render_y - zombie_size, zombie_size, zombie_size, rgb(40,0,255));
	}
}

